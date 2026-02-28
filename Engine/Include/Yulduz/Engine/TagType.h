#pragma once

#include <Yulduz/Common.h>

typedef uint32_t YULDUZ_TagType;

typedef struct YULDUZ_TagTypeRegistry YULDUZ_TagTypeRegistry;

struct YULDUZ_TagTypeRegistry {
    char **Dense;

    uint32_t DenseCapacity;
    uint32_t DenseCount;

    SDL_PropertiesID HashMap;

    uint32_t _yulduz_padding;
};

constexpr YULDUZ_TagType YULDUZ_INVALID_TAG_TYPE = 0xFFFFFFFF;

YULDUZ_API bool YULDUZ_InitializeTagTypeRegistry(
    YULDUZ_TagTypeRegistry *registry, uint32_t initial_capacity);
YULDUZ_API void YULDUZ_ReleaseTagTypeRegistry(YULDUZ_TagTypeRegistry *registry);

YULDUZ_API bool YULDUZ_RegisterTagTypeInTagTypeRegistry(
    YULDUZ_TagTypeRegistry *registry, const char *type_name, YULDUZ_NULLABLE YULDUZ_TagType *type);
YULDUZ_API bool YULDUZ_GetTagTypeInTagTypeRegistry(
    const YULDUZ_TagTypeRegistry *registry, const char *type_name, YULDUZ_TagType *type);
YULDUZ_API bool YULDUZ_GetTagTypeNameInTagTypeRegistry(
    const YULDUZ_TagTypeRegistry *registry, YULDUZ_TagType type, char **type_name);

YULDUZ_API int32_t YULDUZ_SDL_CompareTagTypes(const void *a, const void *b);

YULDUZ_API void YULDUZ_SDL_SortTagTypes(YULDUZ_TagType *types, uint32_t count);
