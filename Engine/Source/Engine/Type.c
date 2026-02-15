#include <Yulduz/Engine/Type.h>

void YULDUZ_EnsureDenseCapacityInComponentTypeRegistry(YULDUZ_TypeRegistry *registry, uint32_t count);

bool YULDUZ_InitializeTypeRegistry(YULDUZ_TypeRegistry *registry, uint32_t initial_capacity) {
    SDL_zerop(registry);

    registry->DenseCapacity = initial_capacity;
    registry->DenseCount    = 0;
    registry->Dense         = SDL_malloc(sizeof(YULDUZ_TypeDescription) * registry->DenseCapacity);

    return true;
}

void YULDUZ_ReleaseTypeRegistry(YULDUZ_TypeRegistry *registry) {
    for (uint32_t i = 0; i < registry->DenseCount; i++) {
        SDL_free(registry->Dense[i].Name);
    }

    SDL_free(registry->Dense);

    SDL_zerop(registry);
}

bool YULDUZ_RegisterTypesInTypeRegistry(
    YULDUZ_TypeRegistry *registry, const YULDUZ_TypeDescription *type_descriptions,
    YULDUZ_NULLABLE YULDUZ_Type *types, uint32_t type_count) {
    YULDUZ_EnsureDenseCapacityInComponentTypeRegistry(registry, type_count);

    for (uint32_t j = 0; j < type_count; j++) {
        uint32_t index = registry->DenseCount;
        registry->DenseCount++;

        registry->Dense[index].Name      = SDL_strdup(type_descriptions[j].Name);
        registry->Dense[index].Size      = type_descriptions[j].Size;
        registry->Dense[index].Alignment = type_descriptions[j].Alignment;

        if (nullptr != types)
            types[j] = index;
    }

    return true;
}

bool YULDUZ_GetTypesInTypeRegistry(
    const YULDUZ_TypeRegistry *registry, const char **type_names, YULDUZ_Type *types, uint32_t type_count) {
    bool found_all = true;

    for (uint32_t j = 0; j < type_count; j++) {
        types[j] = YULDUZ_INVALID_COMPONENT_TYPE;

        bool found_j = false;
        for (uint32_t i = 0; i < registry->DenseCount; i++) {
            if (0 == SDL_strcmp(registry->Dense[i].Name, type_names[j])) {
                types[j] = i;
                found_j  = true;
                break;
            }
        }

        found_all = found_all && found_j;
    }

    return found_all;
}

bool YULDUZ_GetTypeInfosInTypeRegistry(
    const YULDUZ_TypeRegistry *registry, const char **type_names, YULDUZ_TypeInfo *type_infos, uint32_t type_count) {
    bool found_all = true;

    for (uint32_t j = 0; j < type_count; j++) {
        type_infos[j] = YULDUZ_NULL_TYPE_INFO;

        bool found_j = false;
        for (uint32_t i = 0; i < registry->DenseCount; i++) {
            YULDUZ_TypeDescription *description = &registry->Dense[i];
            if (0 == SDL_strcmp(description->Name, type_names[j])) {
                type_infos[j].Type      = i;
                type_infos[j].Size      = description->Size;
                type_infos[j].Alignment = description->Alignment;

                found_j = true;
                break;
            }
        }

        found_all = found_all && found_j;
    }

    return found_all;
}

bool YULDUZ_GetTypeDescriptionsInTypeRegistry(
    const YULDUZ_TypeRegistry *registry, const YULDUZ_Type *types,
    YULDUZ_TypeDescription *type_descriptions, uint32_t type_count) {
    bool found_all = true;

    SDL_memset(type_descriptions, 0, type_count * sizeof(YULDUZ_TypeDescription));

    for (uint32_t j = 0; j < type_count; j++) {
        uint32_t index = types[j];
        if (index >= registry->DenseCount) {
            found_all = false;
            continue;
        }

        type_descriptions[j] = registry->Dense[index];
    }

    return found_all;
}

int32_t YULDUZ_SDL_CompareTypes(const void *a, const void *b) {
    const YULDUZ_Type *type_a = a;
    const YULDUZ_Type *type_b = b;
    if (*type_a < *type_b) return -1;
    if (*type_a > *type_b) return +1;
    return +-0;
}

void YULDUZ_SDL_SortTypes(YULDUZ_Type *types, uint32_t count) {
    if (count > 1)
        SDL_qsort(types, count, sizeof(YULDUZ_Type), YULDUZ_SDL_CompareTypes);
}

void YULDUZ_SDL_SortTypeInfos(YULDUZ_TypeInfo *infos, uint32_t count) {
    if (count > 1)
        SDL_qsort(infos, count, sizeof(YULDUZ_TypeInfo), YULDUZ_SDL_CompareTypes);
}

void YULDUZ_SDL_SortTypeDataInfos(YULDUZ_TypeDataInfo *infos, uint32_t count) {
    if (count > 1)
        SDL_qsort(infos, count, sizeof(YULDUZ_TypeDataInfo), YULDUZ_SDL_CompareTypes);
}

void YULDUZ_EnsureDenseCapacityInComponentTypeRegistry(YULDUZ_TypeRegistry *registry, uint32_t count) {
    uint32_t new_count = registry->DenseCount + count;
    if (new_count < registry->DenseCapacity) {
        return;
    }

    uint32_t new_capacity = registry->DenseCapacity * 2;
    while (new_capacity <= new_count) {
        new_capacity *= 2;
    }

    YULDUZ_TypeDescription *new_dense = SDL_realloc(registry->Dense, sizeof(YULDUZ_TypeDescription) * new_capacity);

    registry->DenseCapacity = new_capacity;
    registry->Dense         = new_dense;
}