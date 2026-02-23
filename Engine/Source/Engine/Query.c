#include <Yulduz/Engine/Query.h>

static void YULDUZ_EnsureWithComponentCapacityInQuery(YULDUZ_Query *query);
static void YULDUZ_EnsureWithTagCapacityInQuery(YULDUZ_Query *query);
static void YULDUZ_EnsureWithoutComponentCapacityInQuery(YULDUZ_Query *query);
static void YULDUZ_EnsureWithoutTagCapacityInQuery(YULDUZ_Query *query);

bool YULDUZ_InitializeQuery(YULDUZ_Query *query, uint32_t initial_capacity) {
    SDL_zerop(query);

    query->WithComponentCapacity    = initial_capacity;
    query->WithComponentCount       = 0;
    query->WithComponentAccessTypes = SDL_malloc(sizeof(YULDUZ_QueryAccessType) * initial_capacity);
    query->WithComponentTypes       = SDL_malloc(sizeof(YULDUZ_ComponentType) * initial_capacity);

    query->WithoutComponentCapacity = initial_capacity;
    query->WithoutComponentCount    = 0;
    query->WithoutComponentTypes    = SDL_malloc(sizeof(YULDUZ_ComponentType) * initial_capacity);

    query->WithTagCapacity = initial_capacity;
    query->WithTagCount    = 0;
    query->WithTagTypes    = SDL_malloc(sizeof(YULDUZ_TagType) * initial_capacity);

    query->WithoutTagCapacity = initial_capacity;
    query->WithoutTagCount    = 0;
    query->WithoutTagTypes    = SDL_malloc(sizeof(YULDUZ_TagType) * initial_capacity);

    return true;
}

void YULDUZ_ReleaseQuery(YULDUZ_Query *query) {
    SDL_free(query->WithComponentAccessTypes);
    SDL_free(query->WithComponentTypes);
    SDL_free(query->WithoutComponentTypes);
    SDL_free(query->WithTagTypes);
    SDL_free(query->WithoutTagTypes);

    SDL_zerop(query);
}

bool YULDUZ_SetQueryWithComponentType(
    YULDUZ_Query *query, YULDUZ_ComponentType type, YULDUZ_QueryAccessType access_type) {
    YULDUZ_EnsureWithComponentCapacityInQuery(query);
    uint32_t new_component = query->WithComponentCount;
    query->WithComponentCount++;
    query->WithComponentAccessTypes[new_component] = access_type;
    query->WithComponentTypes[new_component]       = type;

    return true;
}

bool YULDUZ_SetQueryWithoutComponentType(YULDUZ_Query *query, YULDUZ_ComponentType type) {
    YULDUZ_EnsureWithoutComponentCapacityInQuery(query);
    uint32_t new_component = query->WithoutComponentCount;
    query->WithoutComponentCount++;
    query->WithoutComponentTypes[new_component] = type;

    return true;
}

bool YULDUZ_SetQueryWithTagType(YULDUZ_Query *query, YULDUZ_TagType type) {
    YULDUZ_EnsureWithTagCapacityInQuery(query);

    uint32_t new_tag = query->WithTagCount;
    query->WithTagCount++;
    query->WithTagTypes[new_tag] = type;

    return true;
}

bool YULDUZ_SetQueryWithoutTagType(YULDUZ_Query *query, YULDUZ_TagType type) {
    YULDUZ_EnsureWithoutTagCapacityInQuery(query);

    uint32_t new_tag = query->WithoutTagCount;
    query->WithoutTagCount++;
    query->WithoutTagTypes[new_tag] = type;

    return true;
}

bool YULDUZ_DeepCopyQuery(const YULDUZ_Query *src_query, YULDUZ_Query *dst_query) {
    *dst_query = *src_query;

    dst_query->WithComponentAccessTypes = SDL_malloc(sizeof(YULDUZ_QueryAccessType) * dst_query->WithComponentCapacity);
    dst_query->WithComponentTypes       = SDL_malloc(sizeof(YULDUZ_ComponentType) * dst_query->WithComponentCapacity);
    dst_query->WithoutComponentTypes    = SDL_malloc(sizeof(YULDUZ_ComponentType) * dst_query->WithoutComponentCapacity);

    dst_query->WithTagTypes    = SDL_malloc(sizeof(YULDUZ_TagType) * dst_query->WithTagCapacity);
    dst_query->WithoutTagTypes = SDL_malloc(sizeof(YULDUZ_TagType) * dst_query->WithoutTagCapacity);

    SDL_memcpy(
        dst_query->WithComponentAccessTypes, src_query->WithComponentAccessTypes,
        sizeof(YULDUZ_QueryAccessType) * src_query->WithComponentCount);
    SDL_memcpy(
        dst_query->WithComponentTypes, src_query->WithComponentTypes,
        sizeof(YULDUZ_ComponentType) * src_query->WithComponentCount);

    SDL_memcpy(
        dst_query->WithoutComponentTypes, src_query->WithoutComponentTypes,
        sizeof(YULDUZ_ComponentType) * src_query->WithoutComponentCount);

    SDL_memcpy(
        dst_query->WithTagTypes, src_query->WithTagTypes,
        sizeof(YULDUZ_TagType) * src_query->WithTagCount);

    SDL_memcpy(
        dst_query->WithoutTagTypes, src_query->WithoutTagTypes,
        sizeof(YULDUZ_TagType) * src_query->WithoutTagCount);

    return true;
}

bool YULDUZ_CreateQueryInfo(YULDUZ_QueryInfo *info, const YULDUZ_Query *query) {
    SDL_zerop(info);

    info->WithComponentCount = query->WithComponentCount;
    info->WithComponentTypes = nullptr;
    if (info->WithComponentCount > 0) {
        info->WithComponentTypes = SDL_malloc(sizeof(YULDUZ_ComponentType) * info->WithComponentCount);
        SDL_memcpy(
            info->WithComponentTypes, query->WithComponentTypes,
            sizeof(YULDUZ_ComponentType) * info->WithComponentCount);
    }

    info->WithoutComponentCount = query->WithoutComponentCount;
    info->WithoutComponentTypes = nullptr;
    if (info->WithoutComponentCount > 0) {
        info->WithoutComponentTypes = SDL_malloc(sizeof(YULDUZ_ComponentType) * info->WithoutComponentCount);
        SDL_memcpy(
            info->WithoutComponentTypes, query->WithoutComponentTypes,
            sizeof(YULDUZ_ComponentType) * info->WithoutComponentCount);
    }

    info->WithTagCount = query->WithTagCount;
    info->WithTagTypes = nullptr;
    if (info->WithTagCount > 0) {
        info->WithTagTypes = SDL_malloc(sizeof(YULDUZ_TagType) * info->WithTagCount);
        SDL_memcpy(info->WithTagTypes, query->WithTagTypes, sizeof(YULDUZ_TagType) * info->WithTagCount);
    }

    info->WithoutTagCount = query->WithoutTagCount;
    info->WithoutTagTypes = nullptr;
    if (info->WithoutTagCount > 0) {
        info->WithoutTagTypes = SDL_malloc(sizeof(YULDUZ_TagType) * info->WithoutTagCount);
        SDL_memcpy(info->WithoutTagTypes, query->WithoutTagTypes, sizeof(YULDUZ_TagType) * info->WithoutTagCount);
    }

    return true;
}

void YULDUZ_DestroyQueryInfo(YULDUZ_QueryInfo *info) {
    SDL_free(info->WithComponentTypes);
    SDL_free(info->WithTagTypes);
    SDL_free(info->WithoutComponentTypes);
    SDL_free(info->WithoutTagTypes);

    SDL_zerop(info);
}

bool YULDUZ_ArchetypeSupportsQueryInfo(const YULDUZ_QueryInfo *query, const YULDUZ_Archetype *archetype) {
    if (archetype->TagCount < query->WithTagCount) {
        return false;
    }
    if (archetype->StoreCount < query->WithComponentCount) {
        return false;
    }
    for (uint32_t i = 0; i < query->WithoutTagCount; i++) {
        if (nullptr != YULDUZ_QueryTagInArchetype(archetype, query->WithoutTagTypes[i])) {
            return false;
        }
    }
    for (uint32_t i = 0; i < query->WithoutComponentCount; i++) {
        if (nullptr != YULDUZ_QueryStoreInArchetype(archetype, query->WithoutComponentTypes[i])) {
            return false;
        }
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

void YULDUZ_EnsureWithComponentCapacityInQuery(YULDUZ_Query *query) {
    if (query->WithComponentCount < query->WithComponentCapacity) {
        return;
    }
    uint32_t old_capacity = query->WithComponentCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_QueryAccessType *new_access_types = SDL_realloc(
        query->WithComponentAccessTypes, sizeof(YULDUZ_QueryAccessType) * new_capacity);
    YULDUZ_ComponentType *new_component_types = SDL_realloc(
        query->WithComponentTypes, sizeof(YULDUZ_ComponentType) * new_capacity);

    query->WithComponentCapacity    = new_capacity;
    query->WithComponentAccessTypes = new_access_types;
    query->WithComponentTypes       = new_component_types;
}

void YULDUZ_EnsureWithTagCapacityInQuery(YULDUZ_Query *query) {
    if (query->WithTagCount < query->WithTagCapacity) {
        return;
    }

    uint32_t old_capacity = query->WithTagCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_ComponentType *new_tag_types = SDL_realloc(query->WithTagTypes, sizeof(YULDUZ_ComponentType) * new_capacity);

    query->WithTagCapacity = new_capacity;
    query->WithTagTypes    = new_tag_types;
}

void YULDUZ_EnsureWithoutComponentCapacityInQuery(YULDUZ_Query *query) {
    if (query->WithoutComponentCount < query->WithoutComponentCapacity) {
        return;
    }
    uint32_t old_capacity = query->WithoutComponentCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_TagType *new_component_types = SDL_realloc(
        query->WithoutComponentTypes, sizeof(YULDUZ_TagType) * new_capacity);

    query->WithoutComponentCapacity = new_capacity;
    query->WithoutComponentTypes    = new_component_types;
}

void YULDUZ_EnsureWithoutTagCapacityInQuery(YULDUZ_Query *query) {
    if (query->WithoutTagCount < query->WithoutTagCapacity) {
        return;
    }

    uint32_t old_capacity = query->WithoutTagCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_TagType *new_tag_types = SDL_realloc(
        query->WithoutTagTypes, sizeof(YULDUZ_TagType) * new_capacity);

    query->WithoutTagCapacity = new_capacity;
    query->WithoutTagTypes    = new_tag_types;
}
