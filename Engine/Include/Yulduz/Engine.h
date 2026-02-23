#pragma once

#include <Yulduz/Common.h>

#include <Yulduz/Engine/Archetype.h>
#include <Yulduz/Engine/Command.h>
#include <Yulduz/Engine/ComponentStore.h>
#include <Yulduz/Engine/ComponentType.h>
#include <Yulduz/Engine/ECS.h>
#include <Yulduz/Engine/Entity.h>
#include <Yulduz/Engine/EntityRegistry.h>
#include <Yulduz/Engine/Query.h>
#include <Yulduz/Engine/System.h>
#include <Yulduz/Engine/TagType.h>

typedef struct YULDUZ_Engine YULDUZ_Engine;
typedef struct YULDUZ_EngineInitializeInfo
    YULDUZ_EngineInitializeInfo;

struct YULDUZ_Engine {
    SDL_AtomicU32 IsRunning;
};

struct YULDUZ_EngineInitializeInfo {
};

YULDUZ_API bool YULDUZ_InitializeEngine(
    YULDUZ_Engine *engine, YULDUZ_NULLABLE const YULDUZ_EngineInitializeInfo *initialize_info);
YULDUZ_API void YULDUZ_ReleaseEngine(YULDUZ_Engine *engine);

YULDUZ_API extern YULDUZ_Engine *YULDUZ_g_pEngine;
