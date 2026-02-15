#pragma once

#include <Yulduz/Engine/ECS.h>
#include <Yulduz/Engine/Query.h>

typedef void (*YULDUZ_SystemPFN)(YULDUZ_Archetype *archetype, const YULDUZ_QueryInfo *query, void *user_data);

typedef struct YULDUZ_System YULDUZ_System;

struct YULDUZ_System {
    char *Name;

    YULDUZ_QueryInfo Query;

    YULDUZ_SystemPFN SystemPFN;
};

YULDUZ_API bool YULDUZ_InitializeSystem(
    YULDUZ_System *system, const char *name, const YULDUZ_Query *query, YULDUZ_SystemPFN system_pfn);
YULDUZ_API void YULDUZ_ReleaseSystem(YULDUZ_System *system);

YULDUZ_API void YULDUZ_RunSystem(YULDUZ_System *system, YULDUZ_ECSRegistry *registry, void *user_data);