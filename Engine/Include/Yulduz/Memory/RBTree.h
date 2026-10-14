#pragma once

#include <Yulduz/Memory/Alignment.h>

/*
    Claude-Generated CODE!!! both RBTree.h and RBTree.c
*/

// A free-list allocator whose free blocks are kept in a red-black tree keyed
// by block size instead of a linked list, so best-fit lookup is O(log n)
// instead of O(n). Unlike YULDUZ_MemoryFreeList, freed blocks are *not*
// coalesced with their address-neighbours: the tree is ordered by size, not
// address, so finding an adjacent block would itself cost O(n). Prefer this
// over YULDUZ_MemoryFreeList when best-fit is needed and the free list is
// expected to get large; prefer YULDUZ_MemoryFreeList when coalescing matters
// more than lookup speed.

typedef Uint64 YULDUZ_MemoryRBTreeNodeColor;

typedef struct YULDUZ_MemoryRBTreeAllocationHeader YULDUZ_MemoryRBTreeAllocationHeader;
typedef struct YULDUZ_MemoryRBTreeNode             YULDUZ_MemoryRBTreeNode;
typedef struct YULDUZ_MemoryRBTree                 YULDUZ_MemoryRBTree;

enum YULDUZ_MemoryRBTreeNodeColor : Uint64 {
    YULDUZ_MEMORY_RB_TREE_NODE_COLOR_RED,
    YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK,
};

struct YULDUZ_MemoryRBTree {
    Uint8 *Memory;
    Uint64 Capacity;
    Uint64 Offset;

    YULDUZ_MemoryRBTreeNode *Root;
};

struct YULDUZ_MemoryRBTreeNode {
    YULDUZ_MemoryRBTreeNode *Parent;
    YULDUZ_MemoryRBTreeNode *Left;
    YULDUZ_MemoryRBTreeNode *Right;

    YULDUZ_MemoryRBTreeNodeColor Color;

    Uint64 BlockSize;
};

struct YULDUZ_MemoryRBTreeAllocationHeader {
    Uint64 BlockSize;
    Uint64 Padding;
};

YULDUZ_API bool YULDUZ_MemoryRBTreeCreate(YULDUZ_MemoryRBTree *tree, Uint64 size);
YULDUZ_API void YULDUZ_MemoryRBTreeDestroy(YULDUZ_MemoryRBTree *tree);
YULDUZ_API void YULDUZ_MemoryRBTreeReset(YULDUZ_MemoryRBTree *tree);

// Always allocates best-fit; that's the entire point of keeping free blocks
// in a tree keyed by size. Blocks are always at least 8-byte aligned since
// the free memory itself doubles up as YULDUZ_MemoryRBTreeNode storage.
YULDUZ_API bool YULDUZ_MemoryRBTreeAllocate(YULDUZ_MemoryRBTree *tree, Uint8 **memory, Uint64 size);
YULDUZ_API bool YULDUZ_MemoryRBTreeAllocateAligned(
    YULDUZ_MemoryRBTree *tree, Uint8 **memory, Uint64 size, Uint32 alignment);

YULDUZ_API void YULDUZ_MemoryRBTreeFree(YULDUZ_MemoryRBTree *tree, Uint8 *memory);

YULDUZ_API YULDUZ_MemoryRBTree YULDUZ_MemoryRBTreeMake(Uint8 *memory, Uint64 size);