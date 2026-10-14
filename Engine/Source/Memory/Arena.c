#include <Yulduz/Memory/Arena.h>

YULDUZ_MemoryArena YULDUZ_MemoryArenaMake(Uint8 *memory, Uint64 size) {
    return (YULDUZ_MemoryArena){
        .Memory   = memory,
        .Capacity = size,
        .Offset   = 0,
        .Marker   = 0,
    };
}

bool YULDUZ_MemoryArenaCreate(YULDUZ_MemoryArena *arena, Uint64 size) {
    // Allocate is for creating a fresh, self-owning heap arena. If Memory is
    // already set, this call would stomp the old pointer without freeing it
    // first - call Release before Allocate-ing again.
    SDL_assert(nullptr == arena->Memory && "Arena already owns memory, Release it before calling Allocate again");

    Uint8 *memory = SDL_malloc(size);
    if (nullptr == memory)
        return false;

    *arena = YULDUZ_MemoryArenaMake(memory, size);

    return true;
}

void YULDUZ_MemoryArenaDestroy(YULDUZ_MemoryArena *arena) {
    SDL_free(arena->Memory);

    SDL_zerop(arena);
}

void YULDUZ_MemoryArenaReset(YULDUZ_MemoryArena *arena) {
    arena->Offset = 0;
    arena->Marker = 0;
}

bool YULDUZ_MemoryArenaAllocate(YULDUZ_MemoryArena *arena, Uint8 **memory, Uint64 size) {
    if (nullptr == arena->Memory)
        return false;
    if (size > arena->Capacity - arena->Offset)
        return false;

    *memory = &arena->Memory[arena->Offset];

    arena->Offset += size;

    return true;
}

bool YULDUZ_MemoryArenaAllocateAligned(YULDUZ_MemoryArena *arena, Uint8 **memory, Uint64 size, Uint32 alignment) {
    SDL_assert(alignment > 0 && (alignment & (alignment - 1)) == 0 && "alignment must be a power of two");
    if (nullptr == arena->Memory)
        return false;

    Uint64 current_address = (Uint64)arena->Memory + arena->Offset;
    Uint64 aligned_address = YULDUZ_AddressAlignUp(current_address, alignment);
    Uint64 aligned_count   = aligned_address - (Uint64)arena->Memory;

    if (aligned_count > arena->Capacity)
        return false;
    if (size > arena->Capacity - aligned_count)
        return false;

    *memory = &arena->Memory[aligned_count];

    arena->Offset = aligned_count + size;

    return true;
}

void YULDUZ_MemoryArenaMarkerPut(YULDUZ_MemoryArena *arena) {
    arena->Marker = arena->Offset;
}

void YULDUZ_MemoryArenaMarkerPop(YULDUZ_MemoryArena *arena) {
    arena->Offset = arena->Marker;
}
