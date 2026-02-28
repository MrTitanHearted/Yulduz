#include <Yulduz/Engine/ComponentType.h>

static void YULDUZ_EnsureDenseCapacityInComponentTypeRegistry(YULDUZ_ComponentTypeRegistry *registry);

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

bool YULDUZ_RegisterComponentTypeInComponentTypeRegistry(
    YULDUZ_ComponentTypeRegistry *registry, const YULDUZ_ComponentTypeDescription *type_description,
    YULDUZ_NULLABLE YULDUZ_ComponentType *type) {
    YULDUZ_EnsureDenseCapacityInComponentTypeRegistry(registry);

    if (nullptr != type) {
        *type = YULDUZ_INVALID_COMPONENT_TYPE;
    }

    if (0 == type_description->Size) {
        DYULDUZ_LOG_ENGINE_WARN("Component Type must have size > 0: '%s'", type_description->Name);
        return false;
    }

    if (YULDUZ_INVALID_COMPONENT_TYPE !=
        SDL_GetNumberProperty(registry->HashMap, type_description->Name, YULDUZ_INVALID_COMPONENT_TYPE)) {
        DYULDUZ_LOG_ENGINE_WARN("Component Type with name '%s' already registered", type_description->Name);
        return false;
    }

    uint32_t index = registry->DenseCount;
    registry->DenseCount++;

    registry->Dense[index] = *type_description;
    registry->Dense[index].Name =
        SDL_strdup(type_description->Name);

    SDL_SetNumberProperty(registry->HashMap, registry->Dense[index].Name, index);

    if (nullptr != type)
        *type = index;

    return true;
}

bool YULDUZ_GetComponentTypeInComponentTypeRegistry(
    const YULDUZ_ComponentTypeRegistry *registry, const char *type_name, YULDUZ_ComponentType *type) {
    *type = SDL_GetNumberProperty(registry->HashMap, type_name, YULDUZ_INVALID_COMPONENT_TYPE);
    return YULDUZ_INVALID_COMPONENT_TYPE != *type;
}

bool YULDUZ_GetComponentTypeInfoInComponentTypeRegistry(
    const YULDUZ_ComponentTypeRegistry *registry, const char *type_name, YULDUZ_ComponentTypeInfo *type_info) {
    *type_info = YULDUZ_NULL_COMPONENT_TYPE_INFO;

    YULDUZ_ComponentType type = SDL_GetNumberProperty(registry->HashMap, type_name, YULDUZ_INVALID_COMPONENT_TYPE);

    if (YULDUZ_INVALID_COMPONENT_TYPE == type) {
        return false;
    }

    type_info->Type      = type;
    type_info->Size      = registry->Dense[type].Size;
    type_info->Alignment = registry->Dense[type].Alignment;

    return true;
}

bool YULDUZ_GetComponentTypeDescriptionInComponentTypeRegistry(
    const YULDUZ_ComponentTypeRegistry *registry, YULDUZ_ComponentType type,
    YULDUZ_ComponentTypeDescription *type_description) {
    if (type >= registry->DenseCount) {
        *type_description = YULDUZ_NULL_COMPONENT_TYPE_DESCRIPTION;
        return false;
    }

    *type_description = registry->Dense[type];

    return true;
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

void YULDUZ_EnsureDenseCapacityInComponentTypeRegistry(YULDUZ_ComponentTypeRegistry *registry) {
    if (registry->DenseCount < registry->DenseCapacity) {
        return;
    }

    uint32_t new_capacity = registry->DenseCapacity * 2;

    YULDUZ_ComponentTypeDescription *new_dense = SDL_realloc(
        registry->Dense, sizeof(YULDUZ_ComponentTypeDescription) * new_capacity);

    registry->DenseCapacity = new_capacity;
    registry->Dense         = new_dense;
}