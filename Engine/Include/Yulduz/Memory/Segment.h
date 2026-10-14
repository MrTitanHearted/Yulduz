#pragma once

#include <Yulduz/Memory/Alignment.h>

/*
    Claude-Generated CODE!!! both Segment.h and Segment.c
*/

// A hybrid of YULDUZ_MemoryFreeList and YULDUZ_MemoryPool: memory is divided
// up-front into fixed-size Blocks (like YULDUZ_MemoryPool), but unlike a pool,
// a single allocation can span a *run* of several contiguous blocks, and free
// runs are tracked and coalesced the same way YULDUZ_MemoryFreeList tracks
// free byte-ranges - just measured in blocks instead of bytes. This trades a
// bit of internal fragmentation (every allocation is rounded up to a whole
// number of blocks) for bookkeeping that's cheaper than a byte-granular free
// list: block counts are small integers instead of arbitrary byte sizes, and
// a run's node/header can always fit inside a single block, so there's no
// analogue of YULDUZ_MemoryFreeList's "is the leftover big enough to hold a
// node" check - every leftover block already qualifies by construction.
//
// Free runs are kept in an address-ordered linked list, exactly like
// YULDUZ_MemoryFreeList - which makes this a first-fit/best-fit allocator
// with O(n) search over the number of *free runs* (typically much smaller
// than the number of blocks). The same size-keyed-tree trick
// YULDUZ_MemoryRBTree applies to YULDUZ_MemoryFreeList could be applied here
// too - keying the tree by BlockCount instead of raw byte size - if best-fit
// search ever becomes the bottleneck.

typedef Uint64 YULDUZ_MemorySegmentNodePlacementPolicy;

typedef struct YULDUZ_MemorySegmentAllocationHeader YULDUZ_MemorySegmentAllocationHeader;
typedef struct YULDUZ_MemorySegmentNode             YULDUZ_MemorySegmentNode;
typedef struct YULDUZ_MemorySegment                 YULDUZ_MemorySegment;

enum YULDUZ_MemorySegmentNodePlacementPolicy : Uint64 {
    YULDUZ_MEMORY_SEGMENT_NODE_PLACEMENT_POLICY_FIRST,
    YULDUZ_MEMORY_SEGMENT_NODE_PLACEMENT_POLICY_BEST,
};

struct YULDUZ_MemorySegment {
    // [BlockCount]Block<BlockSize>, same physical layout as YULDUZ_MemoryPool
    Uint8 *Memory;
    Uint32 BlockSize;
    Uint32 BlockCount;

    Uint64 Offset;  // bytes currently in use, i.e. (blocks in use) * BlockSize

    YULDUZ_MemorySegmentNode *Head;

    YULDUZ_MemorySegmentNodePlacementPolicy Policy;
};

struct YULDUZ_MemorySegmentNode {
    YULDUZ_MemorySegmentNode *Next;

    Uint64 BlockCount;
};

struct YULDUZ_MemorySegmentAllocationHeader {
    Uint64 BlockCount;
    Uint64 Padding;
};

// block_size must be at least sizeof(YULDUZ_MemorySegmentNode): a free run's
// node lives inline in its first block, same as YULDUZ_MemoryFreeList's nodes
// live inline in free memory.
YULDUZ_API bool YULDUZ_MemorySegmentCreate(
    YULDUZ_MemorySegment *segment, Uint32 block_size, Uint32 block_count, Uint32 block_alignment);
YULDUZ_API void YULDUZ_MemorySegmentDestroy(YULDUZ_MemorySegment *segment);
YULDUZ_API void YULDUZ_MemorySegmentReset(YULDUZ_MemorySegment *segment);

// Allocates a run of ceil((size + padding) / BlockSize) contiguous blocks,
// using the segment's current placement Policy (first-fit by default).
YULDUZ_API bool YULDUZ_MemorySegmentAllocate(YULDUZ_MemorySegment *segment, Uint8 **memory, Uint64 size);
YULDUZ_API bool YULDUZ_MemorySegmentAllocateAligned(
    YULDUZ_MemorySegment *segment, Uint8 **memory, Uint64 size, Uint32 alignment);

YULDUZ_API void YULDUZ_MemorySegmentFree(YULDUZ_MemorySegment *segment, Uint8 *memory);
YULDUZ_API void YULDUZ_MemorySegmentSetPolicy(
    YULDUZ_MemorySegment *segment, YULDUZ_MemorySegmentNodePlacementPolicy policy);

YULDUZ_API YULDUZ_MemorySegmentNodePlacementPolicy YULDUZ_MemorySegmentGetPolicy(
    const YULDUZ_MemorySegment *segment);

YULDUZ_API YULDUZ_MemorySegment YULDUZ_MemorySegmentMake(Uint8 *memory, Uint32 block_size, Uint32 block_count);

YULDUZ_API Uint64 YULDUZ_MemorySegmentGetRequiredMemorySize(Uint32 block_size, Uint32 block_count);

YULDUZ_API Uint32 YULDUZ_MemorySegmentGetBlockSize(const YULDUZ_MemorySegment *segment);
YULDUZ_API Uint32 YULDUZ_MemorySegmentGetBlockCount(const YULDUZ_MemorySegment *segment);