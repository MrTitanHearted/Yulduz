#include <Yulduz/Engine/TagType.h>

void YULDUZ_EnsureDenseCapacityInTagTypeRegistry(YULDUZ_TagTypeRegistry *registry, uint32_t count);

bool YULDUZ_InitializeTagTypeRegistry(YULDUZ_TagTypeRegistry *registry, uint32_t initial_capacity) {
    SDL_zerop(registry);

    registry->DenseCapacity = initial_capacity;
    registry->DenseCount    = 0;
    registry->Dense         = SDL_malloc(sizeof(char *) * registry->DenseCapacity);

    registry->HashMap = SDL_CreateProperties();

    if (0 == registry->HashMap) {
        return false;
    }

    return true;
}

void YULDUZ_ReleaseTagTypeRegistry(YULDUZ_TagTypeRegistry *registry) {
    SDL_DestroyProperties(registry->HashMap);
    for (uint32_t i = 0; i < registry->DenseCount; i++) {
        SDL_free(registry->Dense[i]);
    }

    SDL_free(registry->Dense);

    SDL_zerop(registry);
}

bool YULDUZ_RegisterTagTypesInTagTypeRegistry(
    YULDUZ_TagTypeRegistry *registry, const char **type_names,
    YULDUZ_NULLABLE YULDUZ_TagType *types, uint32_t type_count) {
    YULDUZ_EnsureDenseCapacityInTagTypeRegistry(registry, type_count);

    bool registered_all = true;
    for (uint32_t j = 0; j < type_count; j++) {
        if (nullptr != types) {
            types[j] = YULDUZ_INVALID_TAG_TYPE;
        }

        if (YULDUZ_INVALID_TAG_TYPE !=
            SDL_GetNumberProperty(registry->HashMap, type_names[j], YULDUZ_INVALID_TAG_TYPE)) {
            DYULDUZ_LOG_ENGINE_WARN("Type with name '%s' already exists", type_names[j]);
            registered_all = false;
            continue;
        }

        uint32_t index = registry->DenseCount;
        registry->DenseCount++;

        registry->Dense[index] = SDL_strdup(type_names[j]);

        SDL_SetNumberProperty(registry->HashMap, registry->Dense[index], index);

        if (nullptr != types)
            types[j] = index;
    }

    return registered_all;
}

bool YULDUZ_GetTagTypesInTagTypeRegistry(
    const YULDUZ_TagTypeRegistry *registry, const char **type_names, YULDUZ_TagType *types, uint32_t type_count) {
    bool found_all = true;

    for (uint32_t j = 0; j < type_count; j++) {
        types[j] = YULDUZ_INVALID_TAG_TYPE;

        types[j] = SDL_GetNumberProperty(registry->HashMap, type_names[j], YULDUZ_INVALID_TAG_TYPE);

        found_all = found_all && YULDUZ_INVALID_TAG_TYPE != types[j];
    }

    return found_all;
}

bool YULDUZ_GetTagTypeNamesInTagTypeRegistry(
    const YULDUZ_TagTypeRegistry *registry, const YULDUZ_TagType *types,
    char **type_names, uint32_t type_count) {
    bool found_all = true;

    SDL_memset(type_names, 0x00, type_count * sizeof(char *));

    for (uint32_t j = 0; j < type_count; j++) {
        uint32_t index = types[j];
        if (index >= registry->DenseCount) {
            found_all = false;
            continue;
        }

        type_names[j] = registry->Dense[index];
    }

    return found_all;
}

int32_t YULDUZ_SDL_CompareTagTypes(const void *a, const void *b) {
    const YULDUZ_TagType *type_a = a;
    const YULDUZ_TagType *type_b = b;
    if (*type_a < *type_b) return -1;
    if (*type_a > *type_b) return +1;
    return +-0;
}

void YULDUZ_SDL_SortTagTypes(YULDUZ_TagType *types, uint32_t count) {
    SDL_qsort(types, count, sizeof(YULDUZ_TagType), YULDUZ_SDL_CompareTagTypes);
}

void YULDUZ_EnsureDenseCapacityInTagTypeRegistry(YULDUZ_TagTypeRegistry *registry, uint32_t count) {
    uint32_t new_count = registry->DenseCount + count;
    if (new_count < registry->DenseCapacity) {
        return;
    }

    uint32_t new_capacity = registry->DenseCapacity * 2;
    while (new_capacity <= new_count) {
        new_capacity *= 2;
    }

    char **new_dense = SDL_realloc(registry->Dense, sizeof(char *) * new_capacity);

    registry->DenseCapacity = new_capacity;
    registry->Dense         = new_dense;
}