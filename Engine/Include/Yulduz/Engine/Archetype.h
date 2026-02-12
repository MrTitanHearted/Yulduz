#pragma once

#include <Yulduz/Engine/ComponentStore.h>
#include <Yulduz/Engine/ComponentType.h>
#include <Yulduz/Engine/Entity.h>

typedef uint32_t YULDUZ_ArchetypeIndex;
typedef uint32_t YULDUZ_ArchetypeType;

typedef struct YULDUZ_Archetype YULDUZ_Archetype;

struct YULDUZ_Archetype {
    YULDUZ_ComponentStore *Stores;

    YULDUZ_ComponentType *Tags;

    YULDUZ_Entity *Dense;

    uint32_t StoreCount;
    uint32_t TagCount;

    uint32_t DenseCapacity;
    uint32_t DenseCount;
};

// type descriptions must be sorted according to their type ids
YULDUZ_API bool YULDUZ_InitializeArchetype(
    YULDUZ_Archetype                      *archetype,
    const YULDUZ_ComponentTypeDescription *type_descriptions, uint32_t type_count,
    const YULDUZ_ComponentType *tags, uint32_t tag_count,
    uint32_t initial_capacity);
YULDUZ_API void YULDUZ_ReleaseArchetype(YULDUZ_Archetype *archetype);

// infos must be sorted according to their type ids
YULDUZ_API bool YULDUZ_AddInArchetype(
    YULDUZ_Archetype *archetype, YULDUZ_Entity entity,
    const YULDUZ_ComponentDataInfo *component_data, YULDUZ_ArchetypeIndex *index);
YULDUZ_API bool YULDUZ_RemoveInArchetype(
    YULDUZ_Archetype *archetype, YULDUZ_ArchetypeIndex index);

YULDUZ_API bool YULDUZ_MoveEntityInArchetype(
    YULDUZ_Archetype *src, YULDUZ_Archetype *dst,
    const YULDUZ_ComponentDataInfo *component_data, uint32_t component_count,
    YULDUZ_ArchetypeIndex src_index, YULDUZ_ArchetypeIndex *dst_index);

YULDUZ_API YULDUZ_ComponentStore *YULDUZ_QueryStoreInArchetype(YULDUZ_Archetype *archetype, YULDUZ_ComponentType type);
YULDUZ_API YULDUZ_ComponentType  *YULDUZ_QueryTagInArchetype(YULDUZ_Archetype *archetype, YULDUZ_ComponentType tag);

YULDUZ_API bool YULDUZ_QueryStoresInArchetype(
    YULDUZ_Archetype           *archetype,
    const YULDUZ_ComponentType *types, uint32_t type_count,
    const YULDUZ_ComponentType *tags, uint32_t tag_count,
    YULDUZ_ComponentStore **stores);