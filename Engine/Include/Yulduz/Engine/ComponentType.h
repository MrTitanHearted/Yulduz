#pragma once

#include <Yulduz/Common.h>

typedef uint32_t YULDUZ_ComponentType;

typedef struct YULDUZ_ComponentTypeInfo     YULDUZ_ComponentTypeInfo;
typedef struct YULDUZ_ComponentTypeRegistry YULDUZ_ComponentTypeRegistry;
typedef struct YULDUZ_ComponentDataInfo     YULDUZ_ComponentDataInfo;
typedef struct YULDUZ_ComponentTypeDescription
    YULDUZ_ComponentTypeDescription;

struct YULDUZ_ComponentTypeInfo {
    char *Name;

    uint32_t Size;
    uint32_t Alignment;
};

struct YULDUZ_ComponentTypeRegistry {
    YULDUZ_ComponentTypeInfo *Dense;

    uint32_t DenseCapacity;
    uint32_t DenseCount;
};

struct YULDUZ_ComponentDataInfo {
    YULDUZ_ComponentType Type;

    uint32_t _yulduz_padding;

    const void *Data;
};

struct YULDUZ_ComponentTypeDescription {
    YULDUZ_ComponentType Type;

    uint32_t Size;
    uint32_t Alignment;
};

constexpr YULDUZ_ComponentType YULDUZ_INVALID_COMPONENT_TYPE = 0xFFFFFFFF;

YULDUZ_API bool YULDUZ_InitializeComponentTypeRegistry(YULDUZ_ComponentTypeRegistry *registry, uint32_t initial_capacity);
YULDUZ_API void YULDUZ_ReleaseComponentTypeRegistry(YULDUZ_ComponentTypeRegistry *registry);

YULDUZ_API bool YULDUZ_RegisterComponentTypesInComponentTypeRegistry(
    YULDUZ_ComponentTypeRegistry *registry, const YULDUZ_ComponentTypeInfo *type_infos,
    YULDUZ_NULLABLE YULDUZ_ComponentType *types, uint32_t type_count);
YULDUZ_API bool YULDUZ_GetComponentTypesInComponentTypeRegistry(
    YULDUZ_ComponentTypeRegistry *registry, const char **type_names,
    YULDUZ_ComponentType *types, uint32_t type_count);
YULDUZ_API bool YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(
    YULDUZ_ComponentTypeRegistry *registry, const YULDUZ_ComponentType *types,
    YULDUZ_ComponentTypeInfo *type_infos, uint32_t type_count);

YULDUZ_API int32_t YULDUZ_SDL_CompareComponentTypes(const void *a, const void *b);

YULDUZ_API void YULDUZ_SDL_SortComponentTypes(YULDUZ_ComponentType *types, uint32_t count);
YULDUZ_API void YULDUZ_SDL_SortComponentDataInfos(YULDUZ_ComponentDataInfo *infos, uint32_t count);
YULDUZ_API void YULDUZ_SDL_SortComponentTypeDescriptions(YULDUZ_ComponentTypeDescription *descriptions, uint32_t count);
