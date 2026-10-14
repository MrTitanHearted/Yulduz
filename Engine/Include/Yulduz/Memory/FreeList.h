#pragma once

#include <Yulduz/Memory/Alignment.h>

/*
    Half Claude-Generated CODE
*/

typedef Uint64 YULDUZ_MemoryFreeListNodePlacementPolicy;

typedef struct YULDUZ_MemoryFreeListAllocationHeader YULDUZ_MemoryFreeListAllocationHeader;
typedef struct YULDUZ_MemoryFreeListNode             YULDUZ_MemoryFreeListNode;
typedef struct YULDUZ_MemoryFreeList                 YULDUZ_MemoryFreeList;

enum YULDUZ_MemoryFreeListNodePlacementPolicy : Uint64 {
    YULDUZ_MEMORY_FREE_LIST_NODE_PLACEMENT_POLICY_FIRST,
    YULDUZ_MEMORY_FREE_LIST_NODE_PLACEMENT_POLICY_BEST,
};

struct YULDUZ_MemoryFreeList {
    Uint8 *Memory;
    Uint64 Capacity;
    Uint64 Offset;

    YULDUZ_MemoryFreeListNode *Head;

    YULDUZ_MemoryFreeListNodePlacementPolicy Policy;
};

struct YULDUZ_MemoryFreeListNode {
    YULDUZ_MemoryFreeListNode *Next;

    Uint64 BlockSize;
};

struct YULDUZ_MemoryFreeListAllocationHeader {
    Uint64 BlockSize;
    Uint64 Padding;
};

YULDUZ_API bool YULDUZ_MemoryFreeListCreate(YULDUZ_MemoryFreeList *free_list, Uint64 size);
YULDUZ_API void YULDUZ_MemoryFreeListDestroy(YULDUZ_MemoryFreeList *free_list);
YULDUZ_API void YULDUZ_MemoryFreeListReset(YULDUZ_MemoryFreeList *free_list);

// Allocates using the free list's current placement Policy (first-fit by default).
// Blocks are always at least 8-byte aligned since the free memory itself doubles
// up as YULDUZ_MemoryFreeListNode storage.
YULDUZ_API bool YULDUZ_MemoryFreeListAllocate(YULDUZ_MemoryFreeList *free_list, Uint8 **memory, Uint64 size);
YULDUZ_API bool YULDUZ_MemoryFreeListAllocateAligned(
    YULDUZ_MemoryFreeList *free_list, Uint8 **memory, Uint64 size, Uint32 alignment);

YULDUZ_API void YULDUZ_MemoryFreeListRelease(YULDUZ_MemoryFreeList *free_list, Uint8 *memory);
YULDUZ_API void YULDUZ_MemoryFreeListSetPolicy(
    YULDUZ_MemoryFreeList *free_list, YULDUZ_MemoryFreeListNodePlacementPolicy policy);

YULDUZ_API YULDUZ_MemoryFreeListNodePlacementPolicy YULDUZ_MemoryFreeListGetPolicy(
    const YULDUZ_MemoryFreeList *free_list);

YULDUZ_API YULDUZ_MemoryFreeList YULDUZ_MemoryFreeListMake(Uint8 *memory, Uint64 size);