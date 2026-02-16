#pragma once

#include <Yulduz/Common.h>

typedef uint32_t YULDUZ_Type;

typedef struct YULDUZ_TypeDescription YULDUZ_TypeDescription;
typedef struct YULDUZ_TypeRegistry    YULDUZ_TypeRegistry;
typedef struct YULDUZ_TypeDataInfo    YULDUZ_TypeDataInfo;
typedef struct YULDUZ_TypeInfo        YULDUZ_TypeInfo;

struct YULDUZ_TypeDescription {
    char *Name;

    uint32_t Size;
    uint32_t Alignment;
};

struct YULDUZ_TypeRegistry {
    YULDUZ_TypeDescription *Dense;

    uint32_t DenseCapacity;
    uint32_t DenseCount;

    SDL_PropertiesID HashMap;

    uint32_t _yulduz_padding;
};

struct YULDUZ_TypeDataInfo {
    YULDUZ_Type Type;

    uint32_t _yulduz_padding;

    const void *Data;
};

struct YULDUZ_TypeInfo {
    YULDUZ_Type Type;

    uint32_t Size;
    uint32_t Alignment;
};

constexpr YULDUZ_Type YULDUZ_INVALID_TYPE = 0xFFFFFFFF;

#define YULDUZ_NULL_TYPE_DESCRIPTION \
    (YULDUZ_TypeDescription) {       \
        .Name      = nullptr,        \
        .Size      = 0,              \
        .Alignment = 0,              \
    }
#define YULDUZ_NULL_TYPE_INFO                       \
    (YULDUZ_TypeInfo) {                             \
        .Type      = YULDUZ_INVALID_TYPE, \
        .Size      = 0,                             \
        .Alignment = 0,                             \
    }

#define YULDUZ_NULL_TYPE_DATA_INFO             \
    (YULDUZ_TypeDataInfo) {                    \
        .Type = YULDUZ_INVALID_TYPE, \
        .Data = nullptr,                       \
    }

YULDUZ_API bool YULDUZ_InitializeTypeRegistry(YULDUZ_TypeRegistry *registry, uint32_t initial_capacity);
YULDUZ_API void YULDUZ_ReleaseTypeRegistry(YULDUZ_TypeRegistry *registry);

YULDUZ_API bool YULDUZ_RegisterTypesInTypeRegistry(
    YULDUZ_TypeRegistry *registry, const YULDUZ_TypeDescription *type_descriptions,
    YULDUZ_NULLABLE YULDUZ_Type *types, uint32_t type_count);
YULDUZ_API bool YULDUZ_GetTypesInTypeRegistry(
    const YULDUZ_TypeRegistry *registry, const char **type_names,
    YULDUZ_Type *types, uint32_t type_count);
YULDUZ_API bool YULDUZ_GetTypeInfosInTypeRegistry(
    const YULDUZ_TypeRegistry *registry, const char **type_names,
    YULDUZ_TypeInfo *type_infos, uint32_t type_count);
YULDUZ_API bool YULDUZ_GetTypeDescriptionsInTypeRegistry(
    const YULDUZ_TypeRegistry *registry, const YULDUZ_Type *types,
    YULDUZ_TypeDescription *type_descriptions, uint32_t type_count);

YULDUZ_API int32_t YULDUZ_SDL_CompareTypes(const void *a, const void *b);

YULDUZ_API void YULDUZ_SDL_SortTypes(YULDUZ_Type *types, uint32_t count);
YULDUZ_API void YULDUZ_SDL_SortTypeInfos(YULDUZ_TypeInfo *infos, uint32_t count);
YULDUZ_API void YULDUZ_SDL_SortTypeDataInfos(YULDUZ_TypeDataInfo *infos, uint32_t count);
