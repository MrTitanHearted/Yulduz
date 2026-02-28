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

    uint32_t SparseCapacity;
    uint32_t SparseCount;

    uint32_t FreeListCapacity;
    uint32_t FreeListCount;

    uint32_t EntityCount;

    uint32_t _yulduz_padding;
};

constexpr YULDUZ_EntityRecord YULDUZ_INVALID_ENTITY_RECORD = (YULDUZ_EntityRecord){
    .ArchetypeType  = YULDUZ_INVALID_ARCHETYPE_TYPE,
    .ArchetypeIndex = YULDUZ_INVALID_ARCHETYPE_INDEX,
};

YULDUZ_API bool YULDUZ_InitializeEntityRegistry(YULDUZ_EntityRegistry *registry, uint32_t initial_capacity);
YULDUZ_API void YULDUZ_ReleaseEntityRegistry(YULDUZ_EntityRegistry *registry);

YULDUZ_API bool YULDUZ_CreateEntityInEntityRegistry(
    YULDUZ_EntityRegistry *registry, YULDUZ_EntityRecord record, YULDUZ_Entity *entity);
YULDUZ_API bool YULDUZ_DestroyEntityInEntityRegistry(YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity);

YULDUZ_API void YULDUZ_DestroyEntityUnsafeInEntityRegistry(YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity);

YULDUZ_API bool YULDUZ_HasEntityInEntityRegistry(YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity);

YULDUZ_API bool YULDUZ_GetEntityRecordInEntityRegistry(
    const YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity, YULDUZ_EntityRecord *record);
YULDUZ_API bool YULDUZ_SetEntityRecordInEntityRegistry(
    const YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity, YULDUZ_EntityRecord record);

YULDUZ_API void YULDUZ_GetEntityRecordUnsafeInEntityRegistry(
    const YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity, YULDUZ_EntityRecord *record);
YULDUZ_API void YULDUZ_SetEntityRecordUnsafeInEntityRegistry(
    const YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity, YULDUZ_EntityRecord record);

YULDUZ_API uint32_t YULDUZ_GetEntityCountInEntityRegistry(const YULDUZ_EntityRegistry *registry);
