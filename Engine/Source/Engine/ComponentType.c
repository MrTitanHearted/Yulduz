#include <Yulduz/Engine/ComponentType.h>

void YULDUZ_EnsureDenseCapacityInComponentTypeRegistry(YULDUZ_ComponentTypeRegistry *registry, uint32_t count);

bool YULDUZ_InitializeComponentTypeRegistry(YULDUZ_ComponentTypeRegistry *registry, uint32_t initial_capacity) {
    SDL_zerop(registry);

    registry->DenseCapacity = initial_capacity;
    registry->DenseCount    = 0;
    registry->Dense         = SDL_malloc(sizeof(YULDUZ_ComponentTypeInfo) * registry->DenseCapacity);

    return true;
}

void YULDUZ_ReleaseComponentTypeRegistry(YULDUZ_ComponentTypeRegistry *registry) {
    for (uint32_t i = 0; i < registry->DenseCount; i++) {
        SDL_free(registry->Dense[i].Name);
    }

    SDL_free(registry->Dense);

    SDL_zerop(registry);
}

bool YULDUZ_RegisterComponentTypesInComponentTypeRegistry(
    YULDUZ_ComponentTypeRegistry *registry, const YULDUZ_ComponentTypeInfo *type_infos,
    YULDUZ_NULLABLE YULDUZ_ComponentType *types, uint32_t type_count) {
    YULDUZ_EnsureDenseCapacityInComponentTypeRegistry(registry, type_count);

    for (uint32_t j = 0; j < type_count; j++) {
        uint32_t index = registry->DenseCount;
        registry->DenseCount++;

        registry->Dense[index].Name      = SDL_strdup(type_infos[j].Name);
        registry->Dense[index].Size      = type_infos[j].Size;
        registry->Dense[index].Alignment = type_infos[j].Alignment;

        if (nullptr != types)
            types[j] = index;
    }

    return true;
}

bool YULDUZ_GetComponentTypesInComponentTypeRegistry(
    YULDUZ_ComponentTypeRegistry *registry, const char **type_names, YULDUZ_ComponentType *types, uint32_t type_count) {
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

bool YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(
    YULDUZ_ComponentTypeRegistry *registry, const YULDUZ_ComponentType *types,
    YULDUZ_ComponentTypeInfo *type_infos, uint32_t type_count) {
    bool found_all = true;

    SDL_memset(type_infos, 0, type_count * sizeof(YULDUZ_ComponentTypeInfo));

    for (uint32_t j = 0; j < type_count; j++) {
        uint32_t index = types[j];
        if (index >= registry->DenseCount) {
            found_all = false;
            continue;
        }

        type_infos[j] = registry->Dense[index];
    }

    return found_all;
}

int32_t YULDUZ_SDL_CompareComponentTypes(const void *a, const void *b) {
    const YULDUZ_ComponentType *type_a = a;
    const YULDUZ_ComponentType *type_b = b;
    if (*type_a < *type_b) return -1;
    if (*type_a > *type_b) return +1;
    return +-0;
}

void YULDUZ_SDL_SortComponentTypes(YULDUZ_ComponentType *types, uint32_t count) {
    if (count > 1)
        SDL_qsort(types, count, sizeof(YULDUZ_ComponentType), YULDUZ_SDL_CompareComponentTypes);
}

void YULDUZ_SDL_SortComponentDataInfos(YULDUZ_ComponentDataInfo *infos, uint32_t count) {
    if (count > 1)
        SDL_qsort(infos, count, sizeof(YULDUZ_ComponentDataInfo), YULDUZ_SDL_CompareComponentTypes);
}

void YULDUZ_SDL_SortComponentTypeDescriptions(YULDUZ_ComponentTypeDescription *descriptions, uint32_t count) {
    if (count > 1)
        SDL_qsort(descriptions, count, sizeof(YULDUZ_ComponentTypeDescription), YULDUZ_SDL_CompareComponentTypes);
}

void YULDUZ_EnsureDenseCapacityInComponentTypeRegistry(YULDUZ_ComponentTypeRegistry *registry, uint32_t count) {
    uint32_t new_count = registry->DenseCount + count;
    if (new_count < registry->DenseCapacity) {
        return;
    }

    uint32_t new_capacity = registry->DenseCapacity * 2;
    while (new_capacity <= new_count) {
        new_capacity *= 2;
    }

    YULDUZ_ComponentTypeInfo *new_dense = SDL_realloc(registry->Dense, sizeof(YULDUZ_ComponentTypeInfo) * new_capacity);

    registry->DenseCapacity = new_capacity;
    registry->Dense         = new_dense;
}