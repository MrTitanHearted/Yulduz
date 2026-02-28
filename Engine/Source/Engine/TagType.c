#include <Yulduz/Engine/TagType.h>

static void YULDUZ_EnsureDenseCapacityInTagTypeRegistry(YULDUZ_TagTypeRegistry *registry);

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

bool YULDUZ_RegisterTagTypeInTagTypeRegistry(
    YULDUZ_TagTypeRegistry *registry, const char *type_name, YULDUZ_NULLABLE YULDUZ_TagType *type) {
    YULDUZ_EnsureDenseCapacityInTagTypeRegistry(registry);

    if (nullptr != type)
        *type = YULDUZ_INVALID_TAG_TYPE;

    if (YULDUZ_INVALID_TAG_TYPE !=
        SDL_GetNumberProperty(registry->HashMap, type_name, YULDUZ_INVALID_TAG_TYPE)) {
        DYULDUZ_LOG_ENGINE_WARN("Tag Type with name '%s' already registered.", type_name);
        return false;
    }

    uint32_t index = registry->DenseCount;
    registry->DenseCount++;

    registry->Dense[index] = SDL_strdup(type_name);
    SDL_SetNumberProperty(registry->HashMap, registry->Dense[index], index);
    if (nullptr != type)
        *type = index;
    return true;
}

bool YULDUZ_GetTagTypeInTagTypeRegistry(
    const YULDUZ_TagTypeRegistry *registry, const char *type_name, YULDUZ_TagType *type) {
    *type = SDL_GetNumberProperty(registry->HashMap, type_name, YULDUZ_INVALID_TAG_TYPE);
    return YULDUZ_INVALID_TAG_TYPE != *type;
}

bool YULDUZ_GetTagTypeNameInTagTypeRegistry(
    const YULDUZ_TagTypeRegistry *registry, YULDUZ_TagType type, char **type_name) {
    *type_name = nullptr;

    if (type >= registry->DenseCount) {
        return false;
    }

    *type_name = registry->Dense[type];
    return true;
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

void YULDUZ_EnsureDenseCapacityInTagTypeRegistry(YULDUZ_TagTypeRegistry *registry) {
    if (registry->DenseCount < registry->DenseCapacity) {
        return;
    }

    uint32_t new_capacity = registry->DenseCapacity * 2;

    char **new_dense = SDL_realloc(registry->Dense, sizeof(char *) * new_capacity);

    registry->DenseCapacity = new_capacity;
    registry->Dense         = new_dense;
}