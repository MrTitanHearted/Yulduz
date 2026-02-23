#include <Yulduz/Engine/ComponentType.h>

static void YULDUZ_EnsureDenseCapacityInComponentTypeRegistry(YULDUZ_ComponentTypeRegistry *registry, uint32_t count);

bool YULDUZ_InitializeComponentTypeRegistry(YULDUZ_ComponentTypeRegistry *registry, uint32_t initial_capacity) {
    SDL_zerop(registry);

    registry->DenseCapacity = initial_capacity;
    registry->DenseCount    = 0;
    registry->Dense         = SDL_malloc(sizeof(YULDUZ_ComponentTypeDescription) * registry->DenseCapacity);

    registry->HashMap = SDL_CreateProperties();

    if (0 == registry->HashMap) {
        return false;
    }

    return true;
}

void YULDUZ_ReleaseComponentTypeRegistry(YULDUZ_ComponentTypeRegistry *registry) {
    SDL_DestroyProperties(registry->HashMap);
    for (uint32_t i = 0; i < registry->DenseCount; i++) {
        SDL_free(registry->Dense[i].Name);
    }

    SDL_free(registry->Dense);

    SDL_zerop(registry);
}

bool YULDUZ_RegisterComponentTypesInComponentTypeRegistry(
    YULDUZ_ComponentTypeRegistry *registry, const YULDUZ_ComponentTypeDescription *type_descriptions,
    YULDUZ_NULLABLE YULDUZ_ComponentType *types, uint32_t type_count) {
    YULDUZ_EnsureDenseCapacityInComponentTypeRegistry(registry, type_count);

    bool registered_all = true;
    for (uint32_t j = 0; j < type_count; j++) {
        if (nullptr != types) {
            types[j] = YULDUZ_INVALID_COMPONENT_TYPE;
        }

        if (0 == type_descriptions[j].Size) {
            DYULDUZ_LOG_ENGINE_WARN("Types must have size > 0: '%s'", type_descriptions[j].Name);
            registered_all = false;
            continue;
        }

        if (YULDUZ_INVALID_COMPONENT_TYPE !=
            SDL_GetNumberProperty(registry->HashMap, type_descriptions[j].Name, YULDUZ_INVALID_COMPONENT_TYPE)) {
            DYULDUZ_LOG_ENGINE_WARN("Type with name '%s' already exists", type_descriptions[j].Name);
            registered_all = false;
            continue;
        }

        uint32_t index = registry->DenseCount;
        registry->DenseCount++;

        registry->Dense[index] = type_descriptions[j];
        registry->Dense[index].Name =
            SDL_strdup(type_descriptions[j].Name);

        SDL_SetNumberProperty(registry->HashMap, registry->Dense[index].Name, index);

        if (nullptr != types)
            types[j] = index;
    }

    return registered_all;
}

bool YULDUZ_GetComponentTypesInComponentTypeRegistry(
    const YULDUZ_ComponentTypeRegistry *registry, const char **type_names, YULDUZ_ComponentType *types, uint32_t type_count) {
    bool found_all = true;

    for (uint32_t j = 0; j < type_count; j++) {
        types[j] = YULDUZ_INVALID_COMPONENT_TYPE;

        types[j] = SDL_GetNumberProperty(registry->HashMap, type_names[j], YULDUZ_INVALID_COMPONENT_TYPE);

        found_all = found_all && YULDUZ_INVALID_COMPONENT_TYPE != types[j];
    }

    return found_all;
}

bool YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(
    const YULDUZ_ComponentTypeRegistry *registry, const char **type_names,
    YULDUZ_ComponentTypeInfo *type_infos, uint32_t type_count) {
    bool found_all = true;

    for (uint32_t j = 0; j < type_count; j++) {
        type_infos[j] = YULDUZ_NULL_COMPONENT_TYPE_INFO;

        YULDUZ_ComponentType type = SDL_GetNumberProperty(
            registry->HashMap, type_names[j], YULDUZ_INVALID_COMPONENT_TYPE);

        if (YULDUZ_INVALID_COMPONENT_TYPE == type) {
            found_all = false;
            continue;
        }

        type_infos[j].Type      = type;
        type_infos[j].Size      = registry->Dense[type].Size;
        type_infos[j].Alignment = registry->Dense[type].Alignment;
    }

    return found_all;
}

bool YULDUZ_GetComponentTypeDescriptionsInComponentTypeRegistry(
    const YULDUZ_ComponentTypeRegistry *registry, const YULDUZ_ComponentType *types,
    YULDUZ_ComponentTypeDescription *type_descriptions, uint32_t type_count) {
    bool found_all = true;

    SDL_memset(type_descriptions, 0, type_count * sizeof(YULDUZ_ComponentTypeDescription));

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

void YULDUZ_GetComponentTypeDescriptionUnsafeInComponentTypeRegistry(
    const YULDUZ_ComponentTypeRegistry *registry, YULDUZ_ComponentType type,
    YULDUZ_ComponentTypeDescription *type_description) {
    *type_description = registry->Dense[type];
}

int32_t YULDUZ_SDL_CompareComponentTypes(const void *a, const void *b) {
    const YULDUZ_ComponentType *type_a = a;
    const YULDUZ_ComponentType *type_b = b;
    if (*type_a < *type_b) return -1;
    if (*type_a > *type_b) return +1;
    return +-0;
}

void YULDUZ_SDL_SortComponentTypes(YULDUZ_ComponentType *types, uint32_t count) {
    SDL_qsort(types, count, sizeof(YULDUZ_ComponentType), YULDUZ_SDL_CompareComponentTypes);
}

void YULDUZ_SDL_SortComponentTypeInfos(YULDUZ_ComponentTypeInfo *infos, uint32_t count) {
    SDL_qsort(infos, count, sizeof(YULDUZ_ComponentTypeInfo), YULDUZ_SDL_CompareComponentTypes);
}

void YULDUZ_SDL_SortComponentTypeDataInfos(YULDUZ_ComponentTypeDataInfo *infos, uint32_t count) {
    SDL_qsort(infos, count, sizeof(YULDUZ_ComponentTypeDataInfo), YULDUZ_SDL_CompareComponentTypes);
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

    YULDUZ_ComponentTypeDescription *new_dense = SDL_realloc(registry->Dense, sizeof(YULDUZ_ComponentTypeDescription) * new_capacity);

    registry->DenseCapacity = new_capacity;
    registry->Dense         = new_dense;
}