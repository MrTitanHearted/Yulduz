#pragma once

#include <Yulduz/Engine/Entity.h>

typedef struct YULDUZ_EntityRecord   YULDUZ_EntityRecord;
typedef struct YULDUZ_EntityRegistry YULDUZ_EntityRegistry;

struct YULDUZ_EntityRecord {
    YULDUZ_ArchetypeType  ArchetypeType;
    YULDUZ_ArchetypeIndex ArchetypeIndex;

    bool Alive;
};

struct YULDUZ_EntityRegistry {
    YULDUZ_EntityRecord *Sparse;

    YULDUZ_Entity *FreeList;

    YULDUZ_Entity NextEntity;

    uint32_t SparseCapacity;

    uint32_t FreeListCapacity;
    uint32_t FreeListCount;
};

#define YULDUZ_NULL_ENTITY_RECORD                         \
    (YULDUZ_EntityRecord) {                               \
        .ArchetypeType  = YULDUZ_INVALID_ARCHETYPE_TYPE,  \
        .ArchetypeIndex = YULDUZ_INVALID_ARCHETYPE_INDEX, \
        .Alive          = false,                          \
    }

YULDUZ_API bool YULDUZ_InitializeEntityRegistry(YULDUZ_EntityRegistry *registry, uint32_t initial_capacity);
YULDUZ_API void YULDUZ_ReleaseEntityRegistry(YULDUZ_EntityRegistry *registry);

YULDUZ_API bool YULDUZ_CreateEntityInEntityRegistry(
    YULDUZ_EntityRegistry *registry, YULDUZ_Entity *entity);
YULDUZ_API bool YULDUZ_DestroyEntityInEntityRegistry(
    YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity);

YULDUZ_API bool YULDUZ_GetEntityRecordsInEntityRegistry(
    YULDUZ_EntityRegistry *registry, const YULDUZ_Entity *entities,
    YULDUZ_EntityRecord *records, uint32_t count);
YULDUZ_API bool YULDUZ_SetEntityRecordsInEntityRegistry(
    YULDUZ_EntityRegistry *registry, const YULDUZ_Entity *entities,
    const YULDUZ_EntityRecord *records, uint32_t count);