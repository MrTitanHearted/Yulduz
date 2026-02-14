#pragma once

#include <Yulduz/Engine/Type.h>

typedef enum YULDUZ_QueryAccessType YULDUZ_QueryAccessType;

typedef struct YULDUZ_Query YULDUZ_Query;

enum YULDUZ_QueryAccessType {
    YULDUZ_QueryAccessType_Read,
    YULDUZ_QueryAccessType_Write,
};

struct YULDUZ_Query {
    YULDUZ_QueryAccessType *RequiredComponentAccessTypes;

    YULDUZ_Type *RequiredComponentTypes;
    YULDUZ_Type *SortedRequiredComponentTypes;

    YULDUZ_Type *RequiredTagTypes;
    YULDUZ_Type *SortedRequiredTagTypes;

    uint32_t RequiredComponentCapacity;
    uint32_t RequiredComponentCount;

    uint32_t RequiredTagCapacity;
    uint32_t RequiredTagCount;
};

YULDUZ_API bool YULDUZ_InitializeQuery(
    YULDUZ_Query *query, uint32_t initial_component_capacity, uint32_t initial_tag_capacity);
YULDUZ_API void YULDUZ_ReleaseQuery(YULDUZ_Query *query);

YULDUZ_API bool YULDUZ_SetQueryWithComponentType(
    YULDUZ_Query *query, YULDUZ_Type component_type, YULDUZ_QueryAccessType access_type);
YULDUZ_API bool YULDUZ_SetQueryWithTagType(YULDUZ_Query *query, YULDUZ_Type tag_type);

YULDUZ_API bool YULDUZ_DeepCopyQuery(const YULDUZ_Query *src_query, YULDUZ_Query *dst_query);