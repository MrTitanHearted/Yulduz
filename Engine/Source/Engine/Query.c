#include <Yulduz/Engine/Query.h>

void YULDUZ_EnsureComponentCapacityInQuery(YULDUZ_Query *query);
void YULDUZ_EnsureTagCapacityInQuery(YULDUZ_Query *query);

bool YULDUZ_InitializeQuery(YULDUZ_Query *query, uint32_t initial_component_capacity, uint32_t initial_tag_capacity) {
    SDL_zerop(query);

    query->RequiredComponentCapacity    = initial_component_capacity;
    query->RequiredComponentCount       = 0;
    query->RequiredComponentAccessTypes = SDL_malloc(sizeof(YULDUZ_QueryAccessType) * initial_component_capacity);
    query->RequiredComponentTypes       = SDL_malloc(sizeof(YULDUZ_Type) * initial_component_capacity);
    query->SortedRequiredComponentTypes = SDL_malloc(sizeof(YULDUZ_Type) * initial_component_capacity);

    query->RequiredTagCapacity    = initial_tag_capacity;
    query->RequiredTagCount       = 0;
    query->RequiredTagTypes       = SDL_malloc(sizeof(YULDUZ_Type) * initial_tag_capacity);
    query->SortedRequiredTagTypes = SDL_malloc(sizeof(YULDUZ_Type) * initial_tag_capacity);

    return true;
}

void YULDUZ_ReleaseQuery(YULDUZ_Query *query) {
    SDL_free(query->RequiredComponentAccessTypes);
    SDL_free(query->RequiredComponentTypes);
    SDL_free(query->SortedRequiredComponentTypes);
    SDL_free(query->RequiredTagTypes);
    SDL_free(query->SortedRequiredTagTypes);

    SDL_zerop(query);
}

bool YULDUZ_SetQueryWithComponentType(YULDUZ_Query *query, YULDUZ_Type component_type, YULDUZ_QueryAccessType access_type) {
    YULDUZ_EnsureComponentCapacityInQuery(query);

    uint32_t new_component = query->RequiredComponentCount;
    query->RequiredComponentCount++;
    query->RequiredComponentAccessTypes[new_component] = access_type;
    query->RequiredComponentTypes[new_component]       = component_type;
    query->SortedRequiredComponentTypes[new_component] = component_type;

    YULDUZ_SDL_SortTypes(query->SortedRequiredComponentTypes, query->RequiredComponentCount);

    return true;
}

bool YULDUZ_SetQueryWithTagType(YULDUZ_Query *query, YULDUZ_Type tag_type) {
    YULDUZ_EnsureTagCapacityInQuery(query);

    uint32_t new_tag = query->RequiredTagCount;
    query->RequiredTagCount++;
    query->RequiredTagTypes[new_tag]       = tag_type;
    query->SortedRequiredTagTypes[new_tag] = tag_type;

    YULDUZ_SDL_SortTypes(query->SortedRequiredTagTypes, query->RequiredTagCount);

    return true;
}

bool YULDUZ_DeepCopyQuery(const YULDUZ_Query *src_query, YULDUZ_Query *dst_query) {
    *dst_query = *src_query;

    dst_query->RequiredComponentAccessTypes = SDL_malloc(sizeof(YULDUZ_QueryAccessType) * dst_query->RequiredComponentCapacity);
    dst_query->RequiredComponentTypes       = SDL_malloc(sizeof(YULDUZ_Type) * dst_query->RequiredComponentCapacity);
    dst_query->SortedRequiredComponentTypes = SDL_malloc(sizeof(YULDUZ_Type) * dst_query->RequiredComponentCapacity);

    dst_query->RequiredTagTypes       = SDL_malloc(sizeof(YULDUZ_Type) * dst_query->RequiredTagCapacity);
    dst_query->SortedRequiredTagTypes = SDL_malloc(sizeof(YULDUZ_Type) * dst_query->RequiredTagCapacity);

    SDL_memcpy(
        dst_query->RequiredComponentAccessTypes, src_query->RequiredComponentAccessTypes,
        sizeof(YULDUZ_QueryAccessType) * src_query->RequiredComponentCount);
    SDL_memcpy(
        dst_query->RequiredComponentTypes, src_query->RequiredComponentTypes,
        sizeof(YULDUZ_Type) * src_query->RequiredComponentCount);
    SDL_memcpy(
        dst_query->SortedRequiredComponentTypes, src_query->SortedRequiredComponentTypes,
        sizeof(YULDUZ_Type) * src_query->RequiredComponentCount);

    SDL_memcpy(
        dst_query->RequiredTagTypes, src_query->RequiredTagTypes,
        sizeof(YULDUZ_Type) * src_query->RequiredTagCount);
    SDL_memcpy(
        dst_query->SortedRequiredTagTypes, src_query->SortedRequiredTagTypes,
        sizeof(YULDUZ_Type) * src_query->RequiredTagCount);

    return true;
}

bool YULDUZ_CreateQueryInfo(const YULDUZ_Query *query, YULDUZ_QueryInfo *info) {
    SDL_zerop(info);

    info->WithComponentCount = query->RequiredComponentCount;
    info->WithComponentTypes = nullptr;
    if (info->WithComponentCount > 0) {
        info->WithComponentTypes = SDL_malloc(sizeof(YULDUZ_Type) * info->WithComponentCount);
        SDL_memcpy(
            info->WithComponentTypes, query->SortedRequiredComponentTypes, sizeof(YULDUZ_Type) * info->WithComponentCount);
    }

    info->WithTagCount = query->RequiredTagCount;
    info->WithTagTypes = nullptr;
    if (info->WithTagCount > 0) {
        info->WithTagTypes = SDL_malloc(sizeof(YULDUZ_Type) * info->WithTagCount);
        SDL_memcpy(info->WithTagTypes, query->SortedRequiredTagTypes, sizeof(YULDUZ_Type) * info->WithTagCount);
    }

    return true;
}

void YULDUZ_DestroyQueryInfo(YULDUZ_QueryInfo *info) {
    SDL_free(info->WithComponentTypes);
    SDL_free(info->WithTagTypes);

    SDL_zerop(info);
}

bool YULDUZ_ArchetypeSupportsQueryInfo(const YULDUZ_QueryInfo *query, const YULDUZ_Archetype *archetype) {
    if (archetype->TagCount < query->WithTagCount) {
        return false;
    }
    if (archetype->StoreCount < query->WithComponentCount) {
        return false;
    }
    for (uint32_t i = 0; i < query->WithTagCount; i++) {
        if (nullptr == YULDUZ_QueryTagInArchetype(archetype, query->WithTagTypes[i])) {
            return false;
        }
    }
    for (uint32_t i = 0; i < query->WithComponentCount; i++) {
        if (nullptr == YULDUZ_QueryStoreInArchetype(archetype, query->WithComponentTypes[i])) {
            return false;
        }
    }

    return true;
}

void YULDUZ_EnsureComponentCapacityInQuery(YULDUZ_Query *query) {
    if (query->RequiredComponentCount < query->RequiredComponentCapacity) {
        return;
    }
    uint32_t old_capacity = query->RequiredComponentCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_QueryAccessType *new_access_types = SDL_realloc(
        query->RequiredComponentAccessTypes, sizeof(YULDUZ_QueryAccessType) * new_capacity);
    YULDUZ_Type *new_component_types = SDL_realloc(
        query->RequiredComponentTypes, sizeof(YULDUZ_Type) * new_capacity);
    YULDUZ_Type *new_sorted_component_types = SDL_realloc(
        query->SortedRequiredComponentTypes, sizeof(YULDUZ_Type) * new_capacity);

    query->RequiredComponentCapacity    = new_capacity;
    query->RequiredComponentAccessTypes = new_access_types;
    query->RequiredComponentTypes       = new_component_types;
    query->SortedRequiredComponentTypes = new_sorted_component_types;
}

void YULDUZ_EnsureTagCapacityInQuery(YULDUZ_Query *query) {
    if (query->RequiredTagCount < query->RequiredTagCapacity) {
        return;
    }

    uint32_t old_capacity = query->RequiredTagCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_Type *new_tag_types        = SDL_realloc(query->RequiredTagTypes, sizeof(YULDUZ_Type) * new_capacity);
    YULDUZ_Type *new_sorted_tag_types = SDL_realloc(query->SortedRequiredTagTypes, sizeof(YULDUZ_Type) * new_capacity);

    query->RequiredTagCapacity    = new_capacity;
    query->RequiredTagTypes       = new_tag_types;
    query->SortedRequiredTagTypes = new_sorted_tag_types;
}
