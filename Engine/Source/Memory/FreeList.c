#include <Yulduz/Memory/FreeList.h>

static Uint64 YULDUZ_MemoryFreeListGetPaddingWithHeader(const Uint8 *memory, Uint64 alignment, Uint64 header_size);

static void YULDUZ_MemoryFreeListNodeInsert(
    YULDUZ_MemoryFreeListNode **head, YULDUZ_MemoryFreeListNode *prev_node, YULDUZ_MemoryFreeListNode *new_node);
static void YULDUZ_MemoryFreeListNodeRemove(
    YULDUZ_MemoryFreeListNode **head, YULDUZ_MemoryFreeListNode *prev_node, YULDUZ_MemoryFreeListNode *del_node);

static YULDUZ_MemoryFreeListNode *YULDUZ_MemoryFreeListFindFirst(
    YULDUZ_MemoryFreeList *free_list, Uint64 size, Uint64 alignment, Uint64 *padding,
    YULDUZ_MemoryFreeListNode **prev_node);
static YULDUZ_MemoryFreeListNode *YULDUZ_MemoryFreeListFindBest(
    YULDUZ_MemoryFreeList *free_list, Uint64 size, Uint64 alignment, Uint64 *padding,
    YULDUZ_MemoryFreeListNode **prev_node);

static void YULDUZ_MemoryFreeListCoalescence(
    YULDUZ_MemoryFreeList *free_list, YULDUZ_MemoryFreeListNode *prev_node, YULDUZ_MemoryFreeListNode *free_node);

bool YULDUZ_MemoryFreeListCreate(YULDUZ_MemoryFreeList *free_list, Uint64 size) {
    SDL_zerop(free_list);

    Uint8 *memory = SDL_malloc(size);
    if (nullptr == memory)
        return false;

    *free_list = YULDUZ_MemoryFreeListMake(memory, size);

    return true;
}

void YULDUZ_MemoryFreeListDestroy(YULDUZ_MemoryFreeList *free_list) {
    SDL_free(free_list->Memory);
    SDL_zerop(free_list);
}

void YULDUZ_MemoryFreeListReset(YULDUZ_MemoryFreeList *free_list) {
    free_list->Offset = 0;

    YULDUZ_MemoryFreeListNode *first_node = (void *)free_list->Memory;

    first_node->BlockSize = free_list->Capacity;
    first_node->Next      = nullptr;

    free_list->Head = first_node;
}

bool YULDUZ_MemoryFreeListAllocate(YULDUZ_MemoryFreeList *free_list, Uint8 **memory, Uint64 size) {
    return YULDUZ_MemoryFreeListAllocateAligned(free_list, memory, size, sizeof(void *));
}

bool YULDUZ_MemoryFreeListAllocateAligned(
    YULDUZ_MemoryFreeList *free_list, Uint8 **memory, Uint64 size, Uint32 alignment) {
    SDL_assert(alignment > 0 && (alignment & (alignment - 1)) == 0 && "alignment must be a power of two");

    // A block always has to be able to hold a YULDUZ_MemoryFreeListNode once it
    // is freed again, and that node - and any header placed inside it - has to
    // stay naturally aligned, so both the requested size and alignment are
    // clamped up to what bookkeeping needs.
    if (size < sizeof(YULDUZ_MemoryFreeListNode))
        size = sizeof(YULDUZ_MemoryFreeListNode);
    if (alignment < 8)
        alignment = 8;
    // Round up to a multiple of 8 too: padding is always a multiple of 8 (since
    // alignment is), so this keeps required_space - and therefore any split-off
    // YULDUZ_MemoryFreeListNode's address - 8-byte aligned as well.
    // size = (size + 7ULL) & ~7ULL;
    size = YULDUZ_AddressAlignUp(size, 8);

    Uint64 padding = 0;

    YULDUZ_MemoryFreeListNode *previous_node = nullptr;
    YULDUZ_MemoryFreeListNode *current_node  = nullptr;

    if (YULDUZ_MEMORY_FREE_LIST_NODE_PLACEMENT_POLICY_BEST == free_list->Policy)
        current_node = YULDUZ_MemoryFreeListFindBest(free_list, size, alignment, &padding, &previous_node);
    else
        current_node = YULDUZ_MemoryFreeListFindFirst(free_list, size, alignment, &padding, &previous_node);

    if (nullptr == current_node)
        return false;

    Uint64 required_space = size + padding;

    Uint64 remaining = current_node->BlockSize - required_space;
    Uint64 consumed  = current_node->BlockSize;

    YULDUZ_MemoryFreeListNodeRemove(&free_list->Head, previous_node, current_node);

    // Only split off a new free node if what's left over is actually big
    // enough to be a node itself; otherwise the leftover bytes are absorbed
    // into this allocation as internal fragmentation.
    if (remaining > sizeof(YULDUZ_MemoryFreeListNode)) {
        YULDUZ_MemoryFreeListNode *new_node = (void *)((Uint8 *)current_node + required_space);
        new_node->BlockSize                 = remaining;
        YULDUZ_MemoryFreeListNodeInsert(&free_list->Head, previous_node, new_node);
        consumed = required_space;
    }

    YULDUZ_MemoryFreeListAllocationHeader *header =
        (void *)((Uint8 *)current_node + padding - sizeof(YULDUZ_MemoryFreeListAllocationHeader));
    header->BlockSize = consumed;
    header->Padding   = padding;

    free_list->Offset += consumed;

    *memory = (Uint8 *)current_node + padding;

    return true;
}

void YULDUZ_MemoryFreeListRelease(YULDUZ_MemoryFreeList *free_list, Uint8 *memory) {
    if (nullptr == memory)
        return;

    YULDUZ_MemoryFreeListAllocationHeader *header =
        (void *)(memory - sizeof(YULDUZ_MemoryFreeListAllocationHeader));

    Uint64 padding    = header->Padding;
    Uint64 block_size = header->BlockSize;

    YULDUZ_MemoryFreeListNode *free_node = (void *)(memory - padding);
    free_node->BlockSize                 = block_size;
    free_node->Next                      = nullptr;

    // Free nodes are kept in address order so adjacent blocks can be found -
    // and coalesced - in a single pass, without having to walk the whole list.
    YULDUZ_MemoryFreeListNode *current_node  = free_list->Head;
    YULDUZ_MemoryFreeListNode *previous_node = nullptr;

    while (current_node != nullptr && (Uint8 *)current_node < (Uint8 *)free_node) {
        previous_node = current_node;
        current_node  = current_node->Next;
    }

    YULDUZ_MemoryFreeListNodeInsert(&free_list->Head, previous_node, free_node);

    free_list->Offset -= free_node->BlockSize;

    YULDUZ_MemoryFreeListCoalescence(free_list, previous_node, free_node);
}

void YULDUZ_MemoryFreeListSetPolicy(
    YULDUZ_MemoryFreeList *free_list, YULDUZ_MemoryFreeListNodePlacementPolicy policy) {
    free_list->Policy = policy;
}

YULDUZ_MemoryFreeListNodePlacementPolicy YULDUZ_MemoryFreeListGetPolicy(const YULDUZ_MemoryFreeList *free_list) {
    return free_list->Policy;
}

YULDUZ_MemoryFreeList YULDUZ_MemoryFreeListMake(Uint8 *memory, Uint64 size) {
    YULDUZ_MemoryFreeList free_list = {0};
    SDL_zero(free_list);
    free_list.Memory   = memory;
    free_list.Capacity = size;
    free_list.Policy   = YULDUZ_MEMORY_FREE_LIST_NODE_PLACEMENT_POLICY_FIRST;
    YULDUZ_MemoryFreeListReset(&free_list);
    return free_list;
}

Uint64 YULDUZ_MemoryFreeListGetPaddingWithHeader(const Uint8 *memory, Uint64 alignment, Uint64 header_size) {
    SDL_assert(alignment > 0 && (alignment & (alignment - 1)) == 0 && "alignment must be a power of two");
    Uint64 current_address = (Uint64)memory;
    Uint64 aligned_address = YULDUZ_AddressAlignUp(current_address + header_size, alignment);
    return aligned_address - current_address;
}

void YULDUZ_MemoryFreeListNodeInsert(
    YULDUZ_MemoryFreeListNode **head, YULDUZ_MemoryFreeListNode *prev_node, YULDUZ_MemoryFreeListNode *new_node) {
    if (nullptr == prev_node) {
        new_node->Next = *head;
        *head          = new_node;
    } else {
        new_node->Next  = prev_node->Next;
        prev_node->Next = new_node;
    }
}

void YULDUZ_MemoryFreeListNodeRemove(
    YULDUZ_MemoryFreeListNode **head, YULDUZ_MemoryFreeListNode *prev_node, YULDUZ_MemoryFreeListNode *del_node) {
    if (nullptr == prev_node)
        *head = del_node->Next;
    else
        prev_node->Next = del_node->Next;
}

YULDUZ_MemoryFreeListNode *YULDUZ_MemoryFreeListFindFirst(
    YULDUZ_MemoryFreeList *free_list, Uint64 size, Uint64 alignment, Uint64 *padding,
    YULDUZ_MemoryFreeListNode **prev_node) {
    YULDUZ_MemoryFreeListNode *node     = free_list->Head;
    YULDUZ_MemoryFreeListNode *previous = nullptr;

    while (node != nullptr) {
        Uint64 node_padding = YULDUZ_MemoryFreeListGetPaddingWithHeader(
            (Uint8 *)node, alignment, sizeof(YULDUZ_MemoryFreeListAllocationHeader));
        Uint64 required_space = size + node_padding;

        if (node->BlockSize >= required_space) {
            *padding   = node_padding;
            *prev_node = previous;
            return node;
        }

        previous = node;
        node     = node->Next;
    }

    return nullptr;
}

YULDUZ_MemoryFreeListNode *YULDUZ_MemoryFreeListFindBest(
    YULDUZ_MemoryFreeList *free_list, Uint64 size, Uint64 alignment, Uint64 *padding,
    YULDUZ_MemoryFreeListNode **prev_node) {
    Uint64 smallest_diff = UINT64_MAX;

    YULDUZ_MemoryFreeListNode *node     = free_list->Head;
    YULDUZ_MemoryFreeListNode *previous = nullptr;
    YULDUZ_MemoryFreeListNode *best     = nullptr;

    // Best-fit has to walk the whole list, unlike first-fit, since any node
    // could turn out to be the tightest fit.
    while (node != nullptr) {
        Uint64 node_padding = YULDUZ_MemoryFreeListGetPaddingWithHeader(
            (Uint8 *)node, alignment, sizeof(YULDUZ_MemoryFreeListAllocationHeader));
        Uint64 required_space = size + node_padding;

        if (node->BlockSize >= required_space && (node->BlockSize - required_space) < smallest_diff) {
            best          = node;
            *prev_node    = previous;
            *padding      = node_padding;
            smallest_diff = node->BlockSize - required_space;
        }

        previous = node;
        node     = node->Next;
    }

    return best;
}

void YULDUZ_MemoryFreeListCoalescence(
    YULDUZ_MemoryFreeList *free_list, YULDUZ_MemoryFreeListNode *prev_node, YULDUZ_MemoryFreeListNode *free_node) {
    if (free_node->Next != nullptr && (Uint8 *)free_node + free_node->BlockSize == (Uint8 *)free_node->Next) {
        free_node->BlockSize += free_node->Next->BlockSize;
        YULDUZ_MemoryFreeListNodeRemove(&free_list->Head, free_node, free_node->Next);
    }

    if (prev_node != nullptr && (Uint8 *)prev_node + prev_node->BlockSize == (Uint8 *)free_node) {
        prev_node->BlockSize += free_node->BlockSize;
        YULDUZ_MemoryFreeListNodeRemove(&free_list->Head, prev_node, free_node);
    }
}