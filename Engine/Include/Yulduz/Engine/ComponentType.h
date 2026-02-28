#pragma once

#include <Yulduz/Common.h>

typedef uint32_t YULDUZ_ComponentType;

typedef struct YULDUZ_ComponentTypeDescription YULDUZ_ComponentTypeDescription;
typedef struct YULDUZ_ComponentTypeRegistry    YULDUZ_ComponentTypeRegistry;
typedef struct YULDUZ_ComponentTypeDataInfo    YULDUZ_ComponentTypeDataInfo;
typedef struct YULDUZ_ComponentTypeInfo        YULDUZ_ComponentTypeInfo;

typedef void (*YULDUZ_OnComponentCreatePFN)(void *component, void *user_data);
typedef void (*YULDUZ_OnComponentDestroyPFN)(void *component, void *user_data);
typedef void (*YULDUZ_OnComponentClonePFN)(void *src, void *dst, void *user_data);

struct YULDUZ_ComponentTypeDescription {
    char *Name;

    uint32_t Size;
    uint32_t Alignment;

    YULDUZ_OnComponentCreatePFN  OnCreatePFN;
    YULDUZ_OnComponentDestroyPFN OnDestroyPFN;
    YULDUZ_OnComponentClonePFN   OnClonePFN;

    void *UserData;
};

struct YULDUZ_ComponentTypeRegistry {
    YULDUZ_ComponentTypeDescription *Dense;

    uint32_t DenseCapacity;
    uint32_t DenseCount;

    SDL_PropertiesID HashMap;

    uint32_t _yulduz_padding;
};

struct YULDUZ_ComponentTypeDataInfo {
    YULDUZ_ComponentType Type;

    uint32_t _yulduz_padding;

    const void *Data;
};

struct YULDUZ_ComponentTypeInfo {
    YULDUZ_ComponentType Type;

    uint32_t Size;
    uint32_t Alignment;
};

constexpr YULDUZ_ComponentType YULDUZ_INVALID_COMPONENT_TYPE = 0xFFFFFFFF;

constexpr YULDUZ_ComponentTypeDescription YULDUZ_NULL_COMPONENT_TYPE_DESCRIPTION = (YULDUZ_ComponentTypeDescription){
    .Name         = nullptr,
    .Size         = 0,
    .Alignment    = 0,
    .OnCreatePFN  = nullptr,
    .OnDestroyPFN = nullptr,
    .OnClonePFN   = nullptr,
    .UserData     = nullptr,
};

constexpr YULDUZ_ComponentTypeInfo YULDUZ_NULL_COMPONENT_TYPE_INFO = (YULDUZ_ComponentTypeInfo){
    .Type      = YULDUZ_INVALID_COMPONENT_TYPE,
    .Size      = 0,
    .Alignment = 0,
};

constexpr YULDUZ_ComponentTypeDataInfo YULDUZ_NULL_COMPONENT_TYPE_DATA_INFO = (YULDUZ_ComponentTypeDataInfo){
    .Type = YULDUZ_INVALID_COMPONENT_TYPE,
    .Data = nullptr,
};

YULDUZ_API bool YULDUZ_InitializeComponentTypeRegistry(
    YULDUZ_ComponentTypeRegistry *registry, uint32_t initial_capacity);
YULDUZ_API void YULDUZ_ReleaseComponentTypeRegistry(YULDUZ_ComponentTypeRegistry *registry);

YULDUZ_API bool YULDUZ_RegisterComponentTypeInComponentTypeRegistry(
    YULDUZ_ComponentTypeRegistry *registry, const YULDUZ_ComponentTypeDescription *type_description,
    YULDUZ_NULLABLE YULDUZ_ComponentType *type);
YULDUZ_API bool YULDUZ_GetComponentTypeInComponentTypeRegistry(
    const YULDUZ_ComponentTypeRegistry *registry, const char *type_name, YULDUZ_ComponentType *type);
YULDUZ_API bool YULDUZ_GetComponentTypeInfoInComponentTypeRegistry(
    const YULDUZ_ComponentTypeRegistry *registry, const char *type_name, YULDUZ_ComponentTypeInfo *type_info);
YULDUZ_API bool YULDUZ_GetComponentTypeDescriptionInComponentTypeRegistry(
    const YULDUZ_ComponentTypeRegistry *registry, YULDUZ_ComponentType type,
    YULDUZ_ComponentTypeDescription *type_description);

YULDUZ_API void YULDUZ_GetComponentTypeDescriptionUnsafeInComponentTypeRegistry(
    const YULDUZ_ComponentTypeRegistry *registry, YULDUZ_ComponentType type,
    YULDUZ_ComponentTypeDescription *type_description);

YULDUZ_API int32_t YULDUZ_SDL_CompareComponentTypes(const void *a, const void *b);

YULDUZ_API void YULDUZ_SDL_SortComponentTypes(YULDUZ_ComponentType *types, uint32_t count);
YULDUZ_API void YULDUZ_SDL_SortComponentTypeInfos(YULDUZ_ComponentTypeInfo *infos, uint32_t count);
YULDUZ_API void YULDUZ_SDL_SortComponentTypeDataInfos(YULDUZ_ComponentTypeDataInfo *infos, uint32_t count);