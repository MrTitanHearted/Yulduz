#pragma once

#include <Yulduz/Memory/Alignment.h>

typedef struct YULDUZ_MemoryArena YULDUZ_MemoryArena;

struct YULDUZ_MemoryArena {
    Uint8 *Memory;
    Uint64 Capacity;
    Uint64 Offset;
    Uint64 Marker;
};

YULDUZ_API YULDUZ_MemoryArena YULDUZ_MemoryArenaMake(Uint8 *memory, Uint64 size);

// Uses SDL_malloc and SDL_free (malloc/free from stdlib)
YULDUZ_API bool YULDUZ_MemoryArenaCreate(YULDUZ_MemoryArena *arena, Uint64 size);
YULDUZ_API void YULDUZ_MemoryArenaDestroy(YULDUZ_MemoryArena *arena);
YULDUZ_API void YULDUZ_MemoryArenaReset(YULDUZ_MemoryArena *arena);

YULDUZ_API bool YULDUZ_MemoryArenaAllocate(YULDUZ_MemoryArena *arena, Uint8 **memory, Uint64 size);
YULDUZ_API bool YULDUZ_MemoryArenaAllocateAligned(
    YULDUZ_MemoryArena *arena, Uint8 **memory, Uint64 size, Uint32 alignment);

YULDUZ_API void YULDUZ_MemoryArenaMarkerPut(YULDUZ_MemoryArena *arena);
YULDUZ_API void YULDUZ_MemoryArenaMarkerPop(YULDUZ_MemoryArena *arena);
