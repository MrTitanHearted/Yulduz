#pragma once

#include <Yulduz/Common.h>

#include <Yulduz/Engine/Command.h>
#include <Yulduz/Engine/ECS.h>
#include <Yulduz/Engine/Node.h>
#include <Yulduz/Engine/Query.h>
#include <Yulduz/Engine/Signal.h>
#include <Yulduz/Engine/System.h>

typedef struct YULDUZ_Engine YULDUZ_Engine;
typedef struct YULDUZ_EngineInitializeInfo
    YULDUZ_EngineInitializeInfo;

struct YULDUZ_Engine {
    YULDUZ_ComponentTypeRegistry ComponentTypeRegistry;
    YULDUZ_TagTypeRegistry       TagTypeRegistry;
    YULDUZ_EntityRegistry        EntityRegistry;
    YULDUZ_ECSRegistry           ECSRegistry;

    SDL_AtomicU32 IsRunning;
};

struct YULDUZ_EngineInitializeInfo {
    uint32_t InitialComponentTypeCapacity;
    uint32_t InitialTagTypeCapacity;
    uint32_t InitialEntityCapacity;
    uint32_t InitialArchetypeCapacity;
    uint32_t InitialArchetypeTypeCapacity;
};

YULDUZ_API bool YULDUZ_InitializeEngine(
    YULDUZ_Engine *engine, YULDUZ_NULLABLE const YULDUZ_EngineInitializeInfo *initialize_info);
YULDUZ_API void YULDUZ_ReleaseEngine(YULDUZ_Engine *engine);

YULDUZ_API void YULDUZ_StartEngine(YULDUZ_Engine *engine);
YULDUZ_API void YULDUZ_StopEngine(YULDUZ_Engine *engine);

YULDUZ_API bool YULDUZ_IsRunningInEngine(YULDUZ_Engine *engine);

YULDUZ_API extern YULDUZ_Engine *YULDUZ_g_pEngine;
