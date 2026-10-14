#include <Yulduz/Memory/Pool.h>

static Uint64 YULDUZ_MemoryPoolGetBlockIndex(const YULDUZ_MemoryPool *pool, const Uint8 *memory_block);
static Uint8 *YULDUZ_MemoryPoolGetBitmap(const YULDUZ_MemoryPool *pool);
static bool   YULDUZ_MemoryPoolBitTest(const Uint8 *bitmap, Uint64 index);
static void   YULDUZ_MemoryPoolBitSet(Uint8 *bitmap, Uint64 index);
static void   YULDUZ_MemoryPoolBitClear(Uint8 *bitmap, Uint64 index);

bool YULDUZ_MemoryPoolCreate(
    YULDUZ_MemoryPool *pool, Uint32 block_size, Uint32 block_count, Uint32 block_alignment) {
    SDL_assert(block_size >= sizeof(YULDUZ_MemoryPoolBlock));
    SDL_assert(block_alignment > 0 && (block_alignment & (block_alignment - 1)) == 0);
    SDL_assert(YULDUZ_AddressIsAligned(block_size, block_alignment));

    Uint64 required_memory_size = YULDUZ_MemoryPoolGetRequiredMemorySize(block_size, block_count);

    Uint8 *memory = SDL_aligned_alloc(block_alignment, required_memory_size);
    if (nullptr == memory)
        return false;

    *pool = YULDUZ_MemoryPoolMake(memory, block_size, block_count);

    return true;
}

void YULDUZ_MemoryPoolDestroy(YULDUZ_MemoryPool *pool) {
    SDL_aligned_free(pool->Memory);
    SDL_zerop(pool);
}

void YULDUZ_MemoryPoolReset(YULDUZ_MemoryPool *pool) {
    pool->Head = nullptr;

    SDL_memset(YULDUZ_MemoryPoolGetBitmap(pool), 0x00, YULDUZ_MemoryPoolGetRequiredBitmapSize(pool->BlockCount));

    for (Uint64 i = 0; i < pool->BlockCount; i++) {
        Uint8 *memory_block = &pool->Memory[pool->BlockSize * i];

        YULDUZ_MemoryPoolBlock *block = (void *)memory_block;

        block->Next = pool->Head;
        pool->Head  = block;
    }
}

bool YULDUZ_MemoryPoolAllocateBlock(YULDUZ_MemoryPool *pool, Uint8 **memory_block) {
    YULDUZ_MemoryPoolBlock *block = pool->Head;
    if (nullptr == block)
        return false;

    pool->Head = block->Next;

    Uint64 index  = YULDUZ_MemoryPoolGetBlockIndex(pool, (void *)block);
    Uint8 *bitmap = YULDUZ_MemoryPoolGetBitmap(pool);

    // No actual need for assert here because if ReleaseBlock works properly,
    // there will not be multiple free blocks to the same block of memory
    SDL_assert(!YULDUZ_MemoryPoolBitTest(bitmap, index));
    if (YULDUZ_MemoryPoolBitTest(bitmap, index))
        return false;

    YULDUZ_MemoryPoolBitSet(bitmap, index);
    *memory_block = (void *)block;
    return true;
}

bool YULDUZ_MemoryPoolReleaseBlock(YULDUZ_MemoryPool *pool, Uint8 *memory_block) {
    if (nullptr == memory_block)
        return false;

    Uint8 *start = pool->Memory;
    Uint8 *end   = &pool->Memory[(Uint64)pool->BlockSize * (Uint64)pool->BlockCount];

    SDL_assert(start <= memory_block && memory_block < end && "This Memory doesn't belong to this pool");
    SDL_assert((Uint64)(memory_block - start) % pool->BlockSize == 0 && "This is not start of the memory block!");

    if (memory_block < start || memory_block >= end)
        return false;
    if (((Uint64)(memory_block - start) % pool->BlockSize) != 0)
        return false;

    Uint64 index  = YULDUZ_MemoryPoolGetBlockIndex(pool, memory_block);
    Uint8 *bitmap = YULDUZ_MemoryPoolGetBitmap(pool);

    SDL_assert(YULDUZ_MemoryPoolBitTest(bitmap, index) && "Double free detected");
    if (!YULDUZ_MemoryPoolBitTest(bitmap, index))
        return false;

    YULDUZ_MemoryPoolBitClear(bitmap, index);

    YULDUZ_MemoryPoolBlock *block = (void *)memory_block;

    block->Next = pool->Head;
    pool->Head  = block;

    return true;
}

Uint64 YULDUZ_MemoryPoolGetRequiredMemorySize(Uint32 block_size, Uint32 block_count) {
    SDL_assert(block_size >= sizeof(YULDUZ_MemoryPoolBlock));
    return (Uint64)block_size * (Uint64)block_count + YULDUZ_MemoryPoolGetRequiredBitmapSize(block_count);
}

Uint64 YULDUZ_MemoryPoolGetRequiredBitmapSize(Uint32 block_count) {
    return ((Uint64)block_count + 7) / 8;
}

YULDUZ_MemoryPool YULDUZ_MemoryPoolMake(Uint8 *memory, Uint32 block_size, Uint32 block_count) {
    SDL_assert(block_size >= sizeof(YULDUZ_MemoryPoolBlock));

    YULDUZ_MemoryPool pool = {0};
    SDL_zero(pool);

    pool.Memory     = memory;
    pool.BlockSize  = block_size;
    pool.BlockCount = block_count;

    YULDUZ_MemoryPoolReset(&pool);

    return pool;
}

Uint64 YULDUZ_MemoryPoolGetBlockIndex(const YULDUZ_MemoryPool *pool, const Uint8 *memory_block) {
    return (Uint64)(memory_block - pool->Memory) / (Uint64)pool->BlockSize;
}

Uint8 *YULDUZ_MemoryPoolGetBitmap(const YULDUZ_MemoryPool *pool) {
    return &pool->Memory[(Uint64)pool->BlockSize * (Uint64)pool->BlockCount];
}

bool YULDUZ_MemoryPoolBitTest(const Uint8 *bitmap, Uint64 index) {
    return (bitmap[index / 8] >> (index & 7)) & 1;
}

void YULDUZ_MemoryPoolBitSet(Uint8 *bitmap, Uint64 index) {
    bitmap[index / 8] |= (Uint8)(1u << (index & 7));
}

void YULDUZ_MemoryPoolBitClear(Uint8 *bitmap, Uint64 index) {
    bitmap[index / 8] &= (Uint8) ~(1u << (index & 7));
}