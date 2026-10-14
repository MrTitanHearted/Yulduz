#include <Yulduz/Memory/Segment.h>

static Uint64 YULDUZ_MemorySegmentGetPaddingWithHeader(const Uint8 *memory, Uint64 alignment, Uint64 header_size);

static void YULDUZ_MemorySegmentNodeInsert(
    YULDUZ_MemorySegmentNode **head, YULDUZ_MemorySegmentNode *prev_node, YULDUZ_MemorySegmentNode *new_node);
static void YULDUZ_MemorySegmentNodeRemove(
    YULDUZ_MemorySegmentNode **head, YULDUZ_MemorySegmentNode *prev_node, YULDUZ_MemorySegmentNode *del_node);

static YULDUZ_MemorySegmentNode *YULDUZ_MemorySegmentFindFirst(
    YULDUZ_MemorySegment *segment, Uint64 size, Uint64 alignment, Uint64 *padding,
    YULDUZ_MemorySegmentNode **prev_node);
static YULDUZ_MemorySegmentNode *YULDUZ_MemorySegmentFindBest(
    YULDUZ_MemorySegment *segment, Uint64 size, Uint64 alignment, Uint64 *padding,
    YULDUZ_MemorySegmentNode **prev_node);

static void YULDUZ_MemorySegmentCoalescence(
    YULDUZ_MemorySegment *segment, YULDUZ_MemorySegmentNode *prev_node, YULDUZ_MemorySegmentNode *free_node);

bool YULDUZ_MemorySegmentCreate(
    YULDUZ_MemorySegment *segment, Uint32 block_size, Uint32 block_count, Uint32 block_alignment) {
    SDL_assert(block_size >= sizeof(YULDUZ_MemorySegmentNode));
    SDL_assert(block_alignment > 0 && (block_alignment & (block_alignment - 1)) == 0);
    SDL_assert(YULDUZ_AddressIsAligned(block_size, block_alignment));

    Uint64 required_memory_size = YULDUZ_MemorySegmentGetRequiredMemorySize(block_size, block_count);

    Uint8 *memory = SDL_aligned_alloc(block_alignment, required_memory_size);
    if (nullptr == memory)
        return false;

    *segment = YULDUZ_MemorySegmentMake(memory, block_size, block_count);

    return true;
}

void YULDUZ_MemorySegmentDestroy(YULDUZ_MemorySegment *segment) {
    SDL_aligned_free(segment->Memory);
    SDL_zerop(segment);
}

void YULDUZ_MemorySegmentReset(YULDUZ_MemorySegment *segment) {
    segment->Offset = 0;

    YULDUZ_MemorySegmentNode *first_node = (void *)segment->Memory;

    first_node->BlockCount = segment->BlockCount;
    first_node->Next       = nullptr;

    segment->Head = first_node;
}

bool YULDUZ_MemorySegmentAllocate(YULDUZ_MemorySegment *segment, Uint8 **memory, Uint64 size) {
    return YULDUZ_MemorySegmentAllocateAligned(segment, memory, size, sizeof(void *));
}

bool YULDUZ_MemorySegmentAllocateAligned(
    YULDUZ_MemorySegment *segment, Uint8 **memory, Uint64 size, Uint32 alignment) {
    SDL_assert(alignment > 0 && (alignment & (alignment - 1)) == 0 && "alignment must be a power of two");

    if (alignment < 8)
        alignment = 8;

    Uint64 padding = 0;

    YULDUZ_MemorySegmentNode *previous_node = nullptr;
    YULDUZ_MemorySegmentNode *current_node  = nullptr;

    if (YULDUZ_MEMORY_SEGMENT_NODE_PLACEMENT_POLICY_BEST == segment->Policy)
        current_node = YULDUZ_MemorySegmentFindBest(segment, size, alignment, &padding, &previous_node);
    else
        current_node = YULDUZ_MemorySegmentFindFirst(segment, size, alignment, &padding, &previous_node);

    if (nullptr == current_node)
        return false;

    // Round the byte requirement up to whole blocks; this is the only place
    // a YULDUZ_MemoryFreeList-style "is the leftover big enough for a node"
    // check would go, and it's unnecessary here - every leftover block can
    // already hold a YULDUZ_MemorySegmentNode by construction (see Create).
    Uint64 required_bytes  = size + padding;
    Uint64 required_blocks = (required_bytes + segment->BlockSize - 1) / segment->BlockSize;

    Uint64 remaining_blocks = current_node->BlockCount - required_blocks;
    Uint64 consumed_blocks  = current_node->BlockCount;

    YULDUZ_MemorySegmentNodeRemove(&segment->Head, previous_node, current_node);

    if (remaining_blocks > 0) {
        YULDUZ_MemorySegmentNode *new_node =
            (void *)((Uint8 *)current_node + required_blocks * segment->BlockSize);
        new_node->BlockCount = remaining_blocks;
        YULDUZ_MemorySegmentNodeInsert(&segment->Head, previous_node, new_node);
        consumed_blocks = required_blocks;
    }

    YULDUZ_MemorySegmentAllocationHeader *header =
        (void *)((Uint8 *)current_node + padding - sizeof(YULDUZ_MemorySegmentAllocationHeader));
    header->BlockCount = consumed_blocks;
    header->Padding    = padding;

    segment->Offset += consumed_blocks * segment->BlockSize;

    *memory = (Uint8 *)current_node + padding;

    return true;
}

void YULDUZ_MemorySegmentFree(YULDUZ_MemorySegment *segment, Uint8 *memory) {
    if (nullptr == memory)
        return;

    YULDUZ_MemorySegmentAllocationHeader *header =
        (void *)(memory - sizeof(YULDUZ_MemorySegmentAllocationHeader));

    Uint64 padding     = header->Padding;
    Uint64 block_count = header->BlockCount;

    YULDUZ_MemorySegmentNode *free_node = (void *)(memory - padding);
    free_node->BlockCount               = block_count;
    free_node->Next                     = nullptr;

    // Free nodes are kept in address order so adjacent runs can be found -
    // and coalesced - in a single pass, without having to walk the whole list.
    YULDUZ_MemorySegmentNode *current_node  = segment->Head;
    YULDUZ_MemorySegmentNode *previous_node = nullptr;

    while (current_node != nullptr && (Uint8 *)current_node < (Uint8 *)free_node) {
        previous_node = current_node;
        current_node  = current_node->Next;
    }

    YULDUZ_MemorySegmentNodeInsert(&segment->Head, previous_node, free_node);

    segment->Offset -= free_node->BlockCount * segment->BlockSize;

    YULDUZ_MemorySegmentCoalescence(segment, previous_node, free_node);
}

void YULDUZ_MemorySegmentSetPolicy(YULDUZ_MemorySegment *segment, YULDUZ_MemorySegmentNodePlacementPolicy policy) {
    segment->Policy = policy;
}

YULDUZ_MemorySegmentNodePlacementPolicy YULDUZ_MemorySegmentGetPolicy(const YULDUZ_MemorySegment *segment) {
    return segment->Policy;
}

YULDUZ_MemorySegment YULDUZ_MemorySegmentMake(Uint8 *memory, Uint32 block_size, Uint32 block_count) {
    SDL_assert(block_size >= sizeof(YULDUZ_MemorySegmentNode));

    YULDUZ_MemorySegment segment = {0};
    SDL_zero(segment);

    segment.Memory     = memory;
    segment.BlockSize  = block_size;
    segment.BlockCount = block_count;
    segment.Policy     = YULDUZ_MEMORY_SEGMENT_NODE_PLACEMENT_POLICY_FIRST;

    YULDUZ_MemorySegmentReset(&segment);

    return segment;
}

Uint64 YULDUZ_MemorySegmentGetRequiredMemorySize(Uint32 block_size, Uint32 block_count) {
    return (Uint64)block_size * (Uint64)block_count;
}

Uint32 YULDUZ_MemorySegmentGetBlockSize(const YULDUZ_MemorySegment *segment) {
    return segment->BlockSize;
}

Uint32 YULDUZ_MemorySegmentGetBlockCount(const YULDUZ_MemorySegment *segment) {
    return segment->BlockCount;
}

Uint64 YULDUZ_MemorySegmentGetPaddingWithHeader(const Uint8 *memory, Uint64 alignment, Uint64 header_size) {
    SDL_assert(alignment > 0 && (alignment & (alignment - 1)) == 0 && "alignment must be a power of two");
    Uint64 current_address = (Uint64)memory;
    Uint64 aligned_address = YULDUZ_AddressAlignUp(current_address + header_size, alignment);
    return aligned_address - current_address;
}

void YULDUZ_MemorySegmentNodeInsert(
    YULDUZ_MemorySegmentNode **head, YULDUZ_MemorySegmentNode *prev_node, YULDUZ_MemorySegmentNode *new_node) {
    if (nullptr == prev_node) {
        new_node->Next = *head;
        *head          = new_node;
    } else {
        new_node->Next  = prev_node->Next;
        prev_node->Next = new_node;
    }
}

void YULDUZ_MemorySegmentNodeRemove(
    YULDUZ_MemorySegmentNode **head, YULDUZ_MemorySegmentNode *prev_node, YULDUZ_MemorySegmentNode *del_node) {
    if (nullptr == prev_node)
        *head = del_node->Next;
    else
        prev_node->Next = del_node->Next;
}

YULDUZ_MemorySegmentNode *YULDUZ_MemorySegmentFindFirst(
    YULDUZ_MemorySegment *segment, Uint64 size, Uint64 alignment, Uint64 *padding,
    YULDUZ_MemorySegmentNode **prev_node) {
    YULDUZ_MemorySegmentNode *node     = segment->Head;
    YULDUZ_MemorySegmentNode *previous = nullptr;

    while (node != nullptr) {
        Uint64 node_padding = YULDUZ_MemorySegmentGetPaddingWithHeader(
            (Uint8 *)node, alignment, sizeof(YULDUZ_MemorySegmentAllocationHeader));
        Uint64 required_blocks = (size + node_padding + segment->BlockSize - 1) / segment->BlockSize;

        if (node->BlockCount >= required_blocks) {
            *padding   = node_padding;
            *prev_node = previous;
            return node;
        }

        previous = node;
        node     = node->Next;
    }

    return nullptr;
}

YULDUZ_MemorySegmentNode *YULDUZ_MemorySegmentFindBest(
    YULDUZ_MemorySegment *segment, Uint64 size, Uint64 alignment, Uint64 *padding,
    YULDUZ_MemorySegmentNode **prev_node) {
    Uint64 smallest_diff = UINT64_MAX;

    YULDUZ_MemorySegmentNode *node     = segment->Head;
    YULDUZ_MemorySegmentNode *previous = nullptr;
    YULDUZ_MemorySegmentNode *best     = nullptr;

    // Best-fit has to walk the whole list, unlike first-fit, since any node
    // could turn out to be the tightest fit.
    while (node != nullptr) {
        Uint64 node_padding = YULDUZ_MemorySegmentGetPaddingWithHeader(
            (Uint8 *)node, alignment, sizeof(YULDUZ_MemorySegmentAllocationHeader));
        Uint64 required_blocks = (size + node_padding + segment->BlockSize - 1) / segment->BlockSize;

        if (node->BlockCount >= required_blocks && (node->BlockCount - required_blocks) < smallest_diff) {
            best          = node;
            *prev_node    = previous;
            *padding      = node_padding;
            smallest_diff = node->BlockCount - required_blocks;
        }

        previous = node;
        node     = node->Next;
    }

    return best;
}

void YULDUZ_MemorySegmentCoalescence(
    YULDUZ_MemorySegment *segment, YULDUZ_MemorySegmentNode *prev_node, YULDUZ_MemorySegmentNode *free_node) {
    if (free_node->Next != nullptr &&
        (Uint8 *)free_node + free_node->BlockCount * segment->BlockSize == (Uint8 *)free_node->Next) {
        free_node->BlockCount += free_node->Next->BlockCount;
        YULDUZ_MemorySegmentNodeRemove(&segment->Head, free_node, free_node->Next);
    }

    if (prev_node != nullptr &&
        (Uint8 *)prev_node + prev_node->BlockCount * segment->BlockSize == (Uint8 *)free_node) {
        prev_node->BlockCount += free_node->BlockCount;
        YULDUZ_MemorySegmentNodeRemove(&segment->Head, prev_node, free_node);
    }
}