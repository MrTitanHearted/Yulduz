#include <Yulduz/Engine/Archetype.h>

bool YULDUZ_EnsureDenseCapacityInArchetype(YULDUZ_Archetype *archetype);

bool YULDUZ_InitializeArchetype(
    YULDUZ_Archetype      *archetype,
    const YULDUZ_TypeInfo *component_types, uint32_t component_count,
    const YULDUZ_Type *tags, uint32_t tag_count,
    uint32_t initial_capacity) {
    SDL_zerop(archetype);

    archetype->StoreCount = component_count;
    archetype->Stores     = SDL_malloc(sizeof(YULDUZ_ComponentStore) * component_count);

    for (uint32_t i = 0; i < component_count; i++) {
        YULDUZ_InitializeComponentStore(&archetype->Stores[i], component_types[i], initial_capacity);
    }

    archetype->DenseCapacity = initial_capacity;
    archetype->DenseCount    = 0;
    archetype->Dense         = SDL_malloc(sizeof(YULDUZ_Entity) * initial_capacity);

    archetype->TagCount = tag_count;
    archetype->Tags     = SDL_malloc(sizeof(YULDUZ_Type) * tag_count);
    SDL_memcpy(archetype->Tags, tags, sizeof(YULDUZ_Type) * tag_count);

    return true;
}

void YULDUZ_ReleaseArchetype(YULDUZ_Archetype *archetype) {
    for (uint32_t i = 0; i < archetype->StoreCount; i++) {
        YULDUZ_ReleaseComponentStore(&archetype->Stores[i]);
    }

    SDL_free(archetype->Stores);
    SDL_free(archetype->Dense);
    SDL_free(archetype->Tags);

    SDL_zerop(archetype);
}

bool YULDUZ_AddInArchetype(
    YULDUZ_Archetype *archetype, YULDUZ_Entity entity,
    const YULDUZ_DataInfo *component_data, YULDUZ_ArchetypeIndex *index) {
    if (!YULDUZ_EnsureDenseCapacityInArchetype(archetype)) {
        return false;
    }

    YULDUZ_ArchetypeIndex dense_index = archetype->DenseCount;

    for (uint32_t i = 0; i < archetype->StoreCount; i++) {
        YULDUZ_ComponentStore *store = &archetype->Stores[i];

        void *store_element = YULDUZ_GetComponentInComponentStore(store, dense_index);

        if (nullptr != component_data[i].Data)
            SDL_memcpy(store_element, component_data[i].Data, store->TypeSize);
        else
            SDL_memset(store_element, 0, store->TypeSize);
    }

    archetype->Dense[dense_index] = entity;
    archetype->DenseCount++;
    *index = dense_index;
    return true;
}

bool YULDUZ_RemoveInArchetype(YULDUZ_Archetype *archetype, YULDUZ_ArchetypeIndex index) {
    YULDUZ_ArchetypeIndex last_index = archetype->DenseCount - 1;
    if (last_index != index) {
        archetype->Dense[index] = archetype->Dense[last_index];

        for (uint32_t i = 0; i < archetype->StoreCount; i++) {
            YULDUZ_CopyBackComponentInComponentStore(&archetype->Stores[i], index, last_index);
        }
    }
    archetype->DenseCount--;
    return true;
}

bool YULDUZ_MoveEntityInArchetype(
    YULDUZ_Archetype *src, YULDUZ_Archetype *dst,
    const YULDUZ_DataInfo *component_data, uint32_t component_count,
    YULDUZ_ArchetypeIndex src_index, YULDUZ_ArchetypeIndex *dst_index) {
    if (!YULDUZ_EnsureDenseCapacityInArchetype(dst)) {
        return false;
    }
    YULDUZ_ArchetypeIndex dst_dense_index = dst->DenseCount;

    for (uint32_t i = 0; i < dst->StoreCount; i++) {
        YULDUZ_ComponentStore *dst_store = &dst->Stores[i];

        void *dst_element = YULDUZ_GetComponentInComponentStore(dst_store, dst_dense_index);

        uint32_t dst_element_size = dst_store->TypeSize;

        YULDUZ_ComponentStore *src_store = YULDUZ_QueryStoreInArchetype(src, dst_store->Type);
        if (nullptr != src_store) {
            void *src_element = YULDUZ_GetComponentInComponentStore(src_store, src_index);
            SDL_memcpy(dst_element, src_element, dst_element_size);
            continue;
        }

        const YULDUZ_DataInfo *info = SDL_bsearch(
            &dst_store->Type, component_data, component_count, sizeof(YULDUZ_DataInfo), YULDUZ_SDL_CompareTypes);
        if (nullptr != info && nullptr != info->Data)
            SDL_memcpy(dst_element, info->Data, dst_element_size);
        else
            SDL_memset(dst_element, 0, dst_element_size);
    }

    YULDUZ_Entity src_entity = src->Dense[src_index];

    dst->Dense[dst_dense_index] = src_entity;
    dst->DenseCount++;
    *dst_index = dst_dense_index;
    YULDUZ_RemoveInArchetype(src, src_index);

    return true;
}

YULDUZ_Type *YULDUZ_QueryTagInArchetype(YULDUZ_Archetype *archetype, YULDUZ_Type tag) {
    return SDL_bsearch(
        &tag, archetype->Tags, archetype->TagCount, sizeof(YULDUZ_Type), YULDUZ_SDL_CompareTypes);
}

YULDUZ_ComponentStore *YULDUZ_QueryStoreInArchetype(YULDUZ_Archetype *archetype, YULDUZ_Type type) {
    return SDL_bsearch(
        &type, archetype->Stores, archetype->StoreCount, sizeof(YULDUZ_ComponentStore), YULDUZ_SDL_CompareTypes);
}
bool YULDUZ_QueryStoresInArchetype(
    YULDUZ_Archetype  *archetype,
    const YULDUZ_Type *component_types, uint32_t component_count,
    const YULDUZ_Type *tags, uint32_t tag_count,
    YULDUZ_ComponentStore **stores) {
    for (uint32_t i = 0; i < tag_count; i++) {
        if (nullptr == YULDUZ_QueryTagInArchetype(archetype, tags[i])) {
            return false;
        }
    }

    bool found_all = true;

    for (uint32_t i = 0; i < component_count; i++) {
        stores[i] = YULDUZ_QueryStoreInArchetype(archetype, component_types[i]);
        found_all = found_all && stores[i];
    }

    return found_all;
}

bool YULDUZ_EnsureDenseCapacityInArchetype(YULDUZ_Archetype *archetype) {
    if (archetype->DenseCount < archetype->DenseCapacity) {
        return true;
    }

    uint32_t old_capacity = archetype->DenseCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_Entity *old_dense = archetype->Dense;
    YULDUZ_Entity *new_dense = SDL_realloc(old_dense, sizeof(YULDUZ_Entity) * new_capacity);
    if (nullptr == new_dense) {
        return false;
    }

    archetype->DenseCapacity = new_capacity;
    archetype->Dense         = new_dense;

    bool reallocated_all = true;

    for (uint32_t i = 0; i < archetype->StoreCount; i++) {
        reallocated_all =
            YULDUZ_ReallocateComponentStore(&archetype->Stores[i], old_capacity, new_capacity) &&
            reallocated_all;
    }

    return reallocated_all;
}
