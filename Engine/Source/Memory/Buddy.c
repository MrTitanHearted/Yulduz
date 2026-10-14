#include <Yulduz/Memory/Buddy.h>

static YULDUZ_MemoryBuddyBlock *YULDUZ_MemoryBuddyBlockNext(const YULDUZ_MemoryBuddyBlock *block);
static YULDUZ_MemoryBuddyBlock *YULDUZ_MemoryBuddyBlockSplit(YULDUZ_MemoryBuddyBlock *block, Uint64 size);

static YULDUZ_MemoryBuddyBlock *YULDUZ_MemoryBuddyFindBest(
    YULDUZ_MemoryBuddyBlock *head, const Uint8 *tail, Uint64 size);
static void YULDUZ_MemoryBuddyCoalescence(YULDUZ_MemoryBuddyBlock *head, const Uint8 *tail);

static Uint64 YULDUZ_MemoryBuddyGetRequiredBlockSize(const YULDUZ_MemoryBuddy *buddy, Uint64 size);

bool YULDUZ_MemoryBuddyCreate(YULDUZ_MemoryBuddy *buddy, Uint64 size, Uint32 alignment) {
    SDL_zerop(buddy);

    Uint8 *memory = SDL_aligned_alloc(alignment, size);
    if (nullptr == memory)
        return false;

    *buddy = YULDUZ_MemoryBuddyMake(memory, size, alignment);

    return true;
}

void YULDUZ_MemoryBuddyDestroy(YULDUZ_MemoryBuddy *buddy) {
    SDL_aligned_free(buddy->Memory);
    SDL_zerop(buddy);
}

void YULDUZ_MemoryBuddyReset(YULDUZ_MemoryBuddy *buddy) {
    YULDUZ_MemoryBuddyBlock *head = (void *)buddy->Memory;

    head->Size   = buddy->Capacity;
    head->IsFree = true;
}

bool YULDUZ_MemoryBuddyAllocate(YULDUZ_MemoryBuddy *buddy, Uint8 **memory, Uint64 size) {
    if (0 == size)
        return false;

    Uint64 required = YULDUZ_MemoryBuddyGetRequiredBlockSize(buddy, size);

    YULDUZ_MemoryBuddyBlock *found = YULDUZ_MemoryBuddyFindBest((void *)buddy->Memory, buddy->Tail, required);
    if (nullptr == found) {
        // No single block was big enough on its own; merging free buddies
        // back together might produce one that is, so sweep once and try
        // again before giving up.
        YULDUZ_MemoryBuddyCoalescence((void *)buddy->Memory, buddy->Tail);
        found = YULDUZ_MemoryBuddyFindBest((void *)buddy->Memory, buddy->Tail, required);
    }

    if (nullptr == found)
        return false;

    found->IsFree = false;

    *memory = (Uint8 *)found + buddy->Alignment;

    return true;
}

void YULDUZ_MemoryBuddyFree(YULDUZ_MemoryBuddy *buddy, Uint8 *memory) {
    if (nullptr == memory)
        return;

    SDL_assert(memory >= buddy->Memory && memory < buddy->Tail && "This memory doesn't belong to this buddy");

    YULDUZ_MemoryBuddyBlock *block = (void *)(memory - buddy->Alignment);

    SDL_assert(!block->IsFree && "Double free detected");

    block->IsFree = true;

    // Coalescing eagerly (rather than lazily, only once an allocation can't
    // otherwise be satisfied) keeps fragmentation from building up between
    // allocations, at the cost of walking the block list on every free
    // instead of just flipping a flag.
    YULDUZ_MemoryBuddyCoalescence((void *)buddy->Memory, buddy->Tail);
}

YULDUZ_MemoryBuddy YULDUZ_MemoryBuddyMake(Uint8 *memory, Uint64 size, Uint32 alignment) {
    SDL_assert(size > 0 && (size & (size - 1)) == 0 && "size must be a power of two");
    SDL_assert(alignment > 0 && (alignment & (alignment - 1)) == 0 && "alignment must be a power of two");
    SDL_assert(alignment <= size && "alignment can't exceed the size of the whole backing block");
    SDL_assert(
        alignment >= sizeof(YULDUZ_MemoryBuddyBlock) &&
        "alignment must be large enough to hold a YULDUZ_MemoryBuddyBlock header");

    YULDUZ_MemoryBuddy buddy = {0};
    SDL_zero(buddy);

    buddy.Memory    = memory;
    buddy.Tail      = memory + size;
    buddy.Capacity  = size;
    buddy.Alignment = alignment;

    YULDUZ_MemoryBuddyReset(&buddy);

    return buddy;
}

YULDUZ_MemoryBuddyBlock *YULDUZ_MemoryBuddyBlockNext(const YULDUZ_MemoryBuddyBlock *block) {
    return (void *)((Uint8 *)block + block->Size);
}

YULDUZ_MemoryBuddyBlock *YULDUZ_MemoryBuddyBlockSplit(YULDUZ_MemoryBuddyBlock *block, Uint64 size) {
    if (nullptr == block || 0 == size)
        return nullptr;

    // Halve the block repeatedly until it's the smallest power-of-two size
    // that can still hold `size`; each halving spawns a free "buddy" block
    // in the other, newly vacated half.
    while (size < block->Size) {
        Uint64 half = block->Size >> 1;

        block->Size = half;

        YULDUZ_MemoryBuddyBlock *buddy = YULDUZ_MemoryBuddyBlockNext(block);
        buddy->Size                    = half;
        buddy->IsFree                  = true;
    }

    if (size <= block->Size)
        return block;

    return nullptr;
}

YULDUZ_MemoryBuddyBlock *YULDUZ_MemoryBuddyFindBest(YULDUZ_MemoryBuddyBlock *head, const Uint8 *tail, Uint64 size) {
    YULDUZ_MemoryBuddyBlock *best  = nullptr;
    YULDUZ_MemoryBuddyBlock *block = head;
    YULDUZ_MemoryBuddyBlock *pair  = YULDUZ_MemoryBuddyBlockNext(block);

    // The common case: nothing has ever been split, so the whole backing
    // buffer is a single free block. Skip straight to splitting it instead
    // of walking a "list" that only has one entry anyway.
    if ((Uint8 *)pair == tail && block->IsFree)
        return YULDUZ_MemoryBuddyBlockSplit(block, size);

    while ((Uint8 *)block < tail && (Uint8 *)pair < tail) {
        if (block->IsFree && size <= block->Size && (nullptr == best || block->Size <= best->Size))
            best = block;

        if (pair->IsFree && size <= pair->Size && (nullptr == best || pair->Size < best->Size))
            // Ties go to `pair` rather than `block`, so equally-good
            // candidates settle on whichever one keeps the search moving
            // forward through memory instead of bouncing between the two.
            best = pair;

        if (block->Size <= pair->Size) {
            block = YULDUZ_MemoryBuddyBlockNext(pair);
            if ((Uint8 *)block < tail)
                pair = YULDUZ_MemoryBuddyBlockNext(block);
        } else {
            // `pair` (and everything inside it) is smaller than `block`,
            // meaning `pair` was itself split further by an earlier
            // allocation; step into it instead of jumping past it.
            block = pair;
            pair  = YULDUZ_MemoryBuddyBlockNext(pair);
        }
    }

    if (nullptr == best)
        return nullptr;

    return YULDUZ_MemoryBuddyBlockSplit(best, size);
}

void YULDUZ_MemoryBuddyCoalescence(YULDUZ_MemoryBuddyBlock *head, const Uint8 *tail) {
    for (;;) {
        YULDUZ_MemoryBuddyBlock *block = head;
        YULDUZ_MemoryBuddyBlock *pair  = YULDUZ_MemoryBuddyBlockNext(block);

        bool merged_any = false;

        while ((Uint8 *)block < tail && (Uint8 *)pair < tail) {
            if (block->IsFree && pair->IsFree && block->Size == pair->Size) {
                block->Size <<= 1;

                block = YULDUZ_MemoryBuddyBlockNext(block);
                if ((Uint8 *)block < tail) {
                    pair       = YULDUZ_MemoryBuddyBlockNext(block);
                    merged_any = true;
                }
            } else if (block->Size < pair->Size) {
                // `pair` is smaller than `block`, meaning it was itself
                // split further; descend into it instead of stepping over.
                block = pair;
                pair  = YULDUZ_MemoryBuddyBlockNext(pair);
            } else {
                block = YULDUZ_MemoryBuddyBlockNext(pair);
                if ((Uint8 *)block < tail)
                    pair = YULDUZ_MemoryBuddyBlockNext(block);
            }
        }

        // Merging two buddies can expose a further merge one level up (their
        // combined block might now be free and the same size as *its*
        // buddy), so keep sweeping until a full pass makes no progress.
        if (!merged_any)
            return;
    }
}

Uint64 YULDUZ_MemoryBuddyGetRequiredBlockSize(const YULDUZ_MemoryBuddy *buddy, Uint64 size) {
    Uint64 actual_size = buddy->Alignment;

    size += sizeof(YULDUZ_MemoryBuddyBlock);
    size = YULDUZ_AddressAlignUp(size, buddy->Alignment);

    while (size > actual_size)
        actual_size <<= 1;

    return actual_size;
}