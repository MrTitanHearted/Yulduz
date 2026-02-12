#pragma once

#include <Yulduz/Common.h>

#include <Yulduz/Engine/Archetype.h>
#include <Yulduz/Engine/ComponentStore.h>
#include <Yulduz/Engine/ComponentType.h>
#include <Yulduz/Engine/Entity.h>
#include <Yulduz/Engine/EntityRegistry.h>

typedef struct YULDUZ_Engine               YULDUZ_Engine;
typedef struct YULDUZ_EngineInitializeInfo YULDUZ_EngineInitializeInfo;

struct YULDUZ_Engine {
    // YULDUZ_EntityRegistry EntityRegistry;

    // YULDUZ_ComponentTypeRegistry ComponentTypeRegistry;

    SDL_AtomicU32 IsRunning;
};

struct YULDUZ_EngineInitializeInfo {
    uint32_t InitialEntitiesCapacity;
    uint32_t InitialComponentTypesCapacity;
};

YULDUZ_API bool YULDUZ_InitializeEngine(
    YULDUZ_Engine *engine, YULDUZ_NULLABLE const YULDUZ_EngineInitializeInfo *initialize_info);
YULDUZ_API void YULDUZ_ReleaseEngine(YULDUZ_Engine *engine);

YULDUZ_API bool YULDUZ_RunEngine(YULDUZ_Engine *engine);

YULDUZ_API YULDUZ_Engine *YULDUZ_GetEngine();

