#pragma once

#include <Yulduz/Engine/Archetype.h>
#include <Yulduz/Engine/ComponentType.h>
#include <Yulduz/Engine/TagType.h>

typedef enum YULDUZ_QueryAccessType YULDUZ_QueryAccessType;

typedef struct YULDUZ_Query     YULDUZ_Query;
typedef struct YULDUZ_QueryInfo YULDUZ_QueryInfo;

enum YULDUZ_QueryAccessType {
    YULDUZ_QueryAccessType_Read,
    YULDUZ_QueryAccessType_Write,
};

struct YULDUZ_Query {
    YULDUZ_QueryAccessType *WithComponentAccessTypes;
    YULDUZ_ComponentType   *WithComponentTypes;
    YULDUZ_ComponentType   *WithoutComponentTypes;

    YULDUZ_TagType *WithTagTypes;
    YULDUZ_TagType *WithoutTagTypes;

    uint32_t WithComponentCapacity;
    uint32_t WithComponentCount;
    uint32_t WithoutComponentCapacity;
    uint32_t WithoutComponentCount;

    uint32_t WithTagCapacity;
    uint32_t WithTagCount;
    uint32_t WithoutTagCapacity;
    uint32_t WithoutTagCount;
};

struct YULDUZ_QueryInfo {
    YULDUZ_ComponentType *WithComponentTypes;
    YULDUZ_ComponentType *WithoutComponentTypes;

    YULDUZ_TagType *WithTagTypes;
    YULDUZ_TagType *WithoutTagTypes;

    uint32_t WithComponentCount;
    uint32_t WithoutComponentCount;

    uint32_t WithTagCount;
    uint32_t WithoutTagCount;
};

YULDUZ_API bool YULDUZ_InitializeQuery(YULDUZ_Query *query, uint32_t initial_capacity);
YULDUZ_API void YULDUZ_ReleaseQuery(YULDUZ_Query *query);

YULDUZ_API bool YULDUZ_SetQueryWithComponentType(
    YULDUZ_Query *query, YULDUZ_ComponentType type, YULDUZ_QueryAccessType access_type);
YULDUZ_API bool YULDUZ_SetQueryWithoutComponentType(YULDUZ_Query *query, YULDUZ_ComponentType type);

YULDUZ_API bool YULDUZ_SetQueryWithTagType(YULDUZ_Query *query, YULDUZ_TagType type);
YULDUZ_API bool YULDUZ_SetQueryWithoutTagType(YULDUZ_Query *query, YULDUZ_TagType type);

YULDUZ_API bool YULDUZ_DeepCopyQuery(const YULDUZ_Query *src_query, YULDUZ_Query *dst_query);

YULDUZ_API bool YULDUZ_CreateQueryInfo(YULDUZ_QueryInfo *info, const YULDUZ_Query *query);
YULDUZ_API void YULDUZ_DestroyQueryInfo(YULDUZ_QueryInfo *info);

YULDUZ_API bool YULDUZ_ArchetypeSupportsQueryInfo(const YULDUZ_QueryInfo *query, const YULDUZ_Archetype *archetype);