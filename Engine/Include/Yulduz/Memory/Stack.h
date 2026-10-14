#pragma once

#include <Yulduz/Memory/Alignment.h>

typedef struct YULDUZ_MemoryStackAllocationHeader YULDUZ_MemoryStackAllocationHeader;
typedef struct YULDUZ_MemoryStack                 YULDUZ_MemoryStack;

struct YULDUZ_MemoryStack {
    Uint8 *Memory;
    Uint64 Capacity;
    Uint64 Offset;
};

struct YULDUZ_MemoryStackAllocationHeader {
    Uint64 Padding;
    Uint64 Size;  // size of the user allocation this header precedes; used to verify LIFO free order
};

YULDUZ_API bool YULDUZ_MemoryStackCreate(YULDUZ_MemoryStack *stack, Uint64 size);
YULDUZ_API void YULDUZ_MemoryStackDestroy(YULDUZ_MemoryStack *stack);
YULDUZ_API void YULDUZ_MemoryStackReset(YULDUZ_MemoryStack *stack);

YULDUZ_API bool YULDUZ_MemoryStackAllocate(YULDUZ_MemoryStack *stack, Uint8 **memory, Uint64 size);
YULDUZ_API bool YULDUZ_MemoryStackAllocateAligned(
    YULDUZ_MemoryStack *stack, Uint8 **memory, Uint64 size, Uint32 alignment);

YULDUZ_API void YULDUZ_MemoryStackFree(YULDUZ_MemoryStack *stack, Uint8 *memory);

YULDUZ_API YULDUZ_MemoryStack YULDUZ_MemoryStackMake(Uint8 *memory, Uint64 size);