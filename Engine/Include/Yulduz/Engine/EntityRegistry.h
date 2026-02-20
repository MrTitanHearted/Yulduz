#pragma once

#include <Yulduz/Engine/Entity.h>

typedef struct YULDUZ_EntityRecord   YULDUZ_EntityRecord;
typedef struct YULDUZ_EntityRegistry YULDUZ_EntityRegistry;

struct YULDUZ_EntityRecord {
    YULDUZ_ArchetypeType  ArchetypeType;
    YULDUZ_ArchetypeIndex ArchetypeIndex;
};

struct YULDUZ_EntityRegistry {
    YULDUZ_EntityRecord *Sparse;

    uint32_t *SparseGenerations;

    uint32_t *FreeList;

    uint32_t NextEntity;

    uint32_t SparseCapacity;

    uint32_t FreeListCapacity;
    uint32_t FreeListCount;

    uint32_t EntityCount;

    uint32_t _yulduz_padding;
};

#define YULDUZ_INVALID_ENTITY_RECORD                      \
    (YULDUZ_EntityRecord) {                               \
        .ArchetypeType  = YULDUZ_INVALID_ARCHETYPE_TYPE,  \
        .ArchetypeIndex = YULDUZ_INVALID_ARCHETYPE_INDEX, \
    }

YULDUZ_API bool YULDUZ_InitializeEntityRegistry(YULDUZ_EntityRegistry *registry, uint32_t initial_capacity);
YULDUZ_API void YULDUZ_ReleaseEntityRegistry(YULDUZ_EntityRegistry *registry);

YULDUZ_API bool YULDUZ_CreateEntityInEntityRegistry(
    YULDUZ_EntityRegistry *registry,
    YULDUZ_ArchetypeType archetype_type, YULDUZ_ArchetypeIndex archetype_index,
    YULDUZ_Entity *entity);
YULDUZ_API bool YULDUZ_DestroyEntityInEntityRegistry(
    YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity);

YULDUZ_API bool YULDUZ_GetEntityRecordsInEntityRegistry(
    const YULDUZ_EntityRegistry *registry, const YULDUZ_Entity *entities,
    YULDUZ_EntityRecord *records, uint32_t count);
YULDUZ_API bool YULDUZ_SetEntityRecordsInEntityRegistry(
    const YULDUZ_EntityRegistry *registry, const YULDUZ_Entity *entities,
    const YULDUZ_EntityRecord *records, uint32_t count);

YULDUZ_API uint32_t YULDUZ_GetEntityCountInEntityRegistry(const YULDUZ_EntityRegistry *registry);