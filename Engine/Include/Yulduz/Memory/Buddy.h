#pragma once

#include <Yulduz/Memory/Alignment.h>

/*
    Claude-Generated CODE!!! both Buddy.h and Buddy.c
*/

// A buddy allocator: splits the backing block in half (repeatedly) until it
// finds the smallest power-of-two block that still fits the requested
// allocation, and merges ("coalesces") a freed block back with its buddy
// whenever that buddy is also free. Compared to YULDUZ_MemoryFreeList, this
// trades some internal fragmentation (every allocation is rounded up to a
// power of two) for cheap coalescing, since a block's buddy address is
// computed directly from its own address and size instead of being found by
// walking a list. Unlike the other allocators in this library, every
// allocation out of a given YULDUZ_MemoryBuddy shares the same alignment -
// the alignment is fixed when the buddy is created, since block boundaries
// are derived purely from repeatedly halving Capacity and have no room to
// shift per-allocation. Prefer this over YULDUZ_MemoryFreeList /
// YULDUZ_MemoryRBTree when O(1)-ish coalescing matters more than tight
// packing; prefer them when allocations are wildly different sizes and
// rounding up to a power of two would waste too much memory.

typedef struct YULDUZ_MemoryBuddyBlock YULDUZ_MemoryBuddyBlock;
typedef struct YULDUZ_MemoryBuddy      YULDUZ_MemoryBuddy;

struct YULDUZ_MemoryBuddyBlock {
    Uint64 Size;
    bool   IsFree;
};

struct YULDUZ_MemoryBuddy {
    Uint8 *Memory;
    Uint8 *Tail;  // sentinel one-past-the-end of Memory; not a real block

    Uint64 Capacity;
    Uint64 Alignment;
};

// Capacity must be a power of two: the whole algorithm works by repeatedly
// halving it, so a non-power-of-two backing size would leave a remainder
// that could never be split evenly and would just sit there unusable.
// Alignment must be a power of two at least as large as
// sizeof(YULDUZ_MemoryBuddyBlock), since every block's header lives at the
// very start of the block it describes and every allocation is offset past
// it by exactly Alignment bytes.
YULDUZ_API bool YULDUZ_MemoryBuddyCreate(YULDUZ_MemoryBuddy *buddy, Uint64 size, Uint32 alignment);
YULDUZ_API void YULDUZ_MemoryBuddyDestroy(YULDUZ_MemoryBuddy *buddy);
YULDUZ_API void YULDUZ_MemoryBuddyReset(YULDUZ_MemoryBuddy *buddy);

// There is no "Aligned" variant here, unlike the other allocators in this
// library - alignment is fixed for the lifetime of the YULDUZ_MemoryBuddy,
// set once at creation time instead of per-call.
YULDUZ_API bool YULDUZ_MemoryBuddyAllocate(YULDUZ_MemoryBuddy *buddy, Uint8 **memory, Uint64 size);
YULDUZ_API void YULDUZ_MemoryBuddyFree(YULDUZ_MemoryBuddy *buddy, Uint8 *memory);

YULDUZ_API YULDUZ_MemoryBuddy YULDUZ_MemoryBuddyMake(Uint8 *memory, Uint64 size, Uint32 alignment);