#pragma once

#include <Yulduz/Engine/ECS.h>
#include <Yulduz/Engine/Query.h>

typedef void (*YULDUZ_SystemPFN)(const YULDUZ_Archetype *archetype, void *user_data);

typedef struct YULDUZ_System     YULDUZ_System;
typedef struct YULDUZ_NodeSystem YULDUZ_NodeSystem;

struct YULDUZ_System {
    char *Name;
    void *UserData;

    YULDUZ_ArchetypeType *Dense;

    YULDUZ_SystemPFN SystemPFN;

    YULDUZ_QueryInfo Query;

    uint32_t DenseCapacity;
    uint32_t DenseCount;

    uint32_t LastArchetypeCount;
};

YULDUZ_API bool YULDUZ_InitializeSystem(
    YULDUZ_System *system, const char *name, void *user_data, const YULDUZ_Query *query, YULDUZ_SystemPFN system_pfn);
YULDUZ_API void YULDUZ_ReleaseSystem(YULDUZ_System *system);

YULDUZ_API void YULDUZ_RunSystem(YULDUZ_System *system, const YULDUZ_ECSRegistry *registry);