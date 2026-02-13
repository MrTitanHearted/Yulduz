#pragma once

#include <Yulduz/Engine/ComponentStore.h>
#include <Yulduz/Engine/Entity.h>
#include <Yulduz/Engine/Type.h>

typedef struct YULDUZ_Archetype YULDUZ_Archetype;

struct YULDUZ_Archetype {
    YULDUZ_ComponentStore *Stores;

    YULDUZ_Type *Tags;

    YULDUZ_Entity *Dense;

    uint32_t StoreCount;
    uint32_t TagCount;

    uint32_t DenseCapacity;
    uint32_t DenseCount;
};

// type infos must be sorted according to their type ids
YULDUZ_API bool YULDUZ_InitializeArchetype(
    YULDUZ_Archetype      *archetype,
    const YULDUZ_TypeInfo *component_types, uint32_t component_count,
    const YULDUZ_Type *tags, uint32_t tag_count,
    uint32_t initial_capacity);
YULDUZ_API void YULDUZ_ReleaseArchetype(YULDUZ_Archetype *archetype);

// type datas must be sorted according to their type ids
YULDUZ_API bool YULDUZ_AddInArchetype(
    YULDUZ_Archetype *archetype, YULDUZ_Entity entity,
    const YULDUZ_DataInfo *component_data, YULDUZ_ArchetypeIndex *index);
YULDUZ_API bool YULDUZ_RemoveInArchetype(
    YULDUZ_Archetype *archetype, YULDUZ_ArchetypeIndex index);

YULDUZ_API bool YULDUZ_MoveEntityInArchetype(
    YULDUZ_Archetype *src, YULDUZ_Archetype *dst,
    const YULDUZ_DataInfo *component_data, uint32_t component_count,
    YULDUZ_ArchetypeIndex src_index, YULDUZ_ArchetypeIndex *dst_index);

YULDUZ_API YULDUZ_Type *YULDUZ_QueryTagInArchetype(YULDUZ_Archetype *archetype, YULDUZ_Type tag);

YULDUZ_API YULDUZ_ComponentStore *YULDUZ_QueryStoreInArchetype(YULDUZ_Archetype *archetype, YULDUZ_Type type);

YULDUZ_API bool YULDUZ_QueryStoresInArchetype(
    YULDUZ_Archetype  *archetype,
    const YULDUZ_Type *component_types, uint32_t component_count,
    const YULDUZ_Type *tags, uint32_t tag_count,
    YULDUZ_ComponentStore **stores);