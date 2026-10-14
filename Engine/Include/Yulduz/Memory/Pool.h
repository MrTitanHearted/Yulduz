#pragma once

#include <Yulduz/Memory/Alignment.h>

typedef struct YULDUZ_MemoryPoolBlock YULDUZ_MemoryPoolBlock;
typedef struct YULDUZ_MemoryPool      YULDUZ_MemoryPool;

struct YULDUZ_MemoryPoolBlock {
    YULDUZ_MemoryPoolBlock *Next;
};

struct YULDUZ_MemoryPool {
    // [BlockCount]Block<BlockSize> + Bitmap<(BlockCount+7)/8>
    Uint8 *Memory;
    Uint32 BlockSize;
    Uint32 BlockCount;

    YULDUZ_MemoryPoolBlock *Head;
};

YULDUZ_API bool YULDUZ_MemoryPoolCreate(
    YULDUZ_MemoryPool *pool, Uint32 block_size, Uint32 block_count, Uint32 block_alignment);
YULDUZ_API void YULDUZ_MemoryPoolDestroy(YULDUZ_MemoryPool *pool);

YULDUZ_API void YULDUZ_MemoryPoolReset(YULDUZ_MemoryPool *pool);

YULDUZ_API bool YULDUZ_MemoryPoolAllocateBlock(YULDUZ_MemoryPool *pool, Uint8 **memory_block);
YULDUZ_API bool YULDUZ_MemoryPoolReleaseBlock(YULDUZ_MemoryPool *pool, Uint8 *memory_block);

YULDUZ_API Uint64 YULDUZ_MemoryPoolGetRequiredMemorySize(Uint32 block_size, Uint32 block_count);
YULDUZ_API Uint64 YULDUZ_MemoryPoolGetRequiredBitmapSize(Uint32 block_count);

YULDUZ_API YULDUZ_MemoryPool YULDUZ_MemoryPoolMake(Uint8 *memory, Uint32 block_size, Uint32 block_count);

YULDUZ_API Uint32 YULDUZ_MemoryPoolGetBlockSize(const YULDUZ_MemoryPool *pool);
YULDUZ_API Uint32 YULDUZ_MemoryPoolGetBlockCount(const YULDUZ_MemoryPool *pool);
