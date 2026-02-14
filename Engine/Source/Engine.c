#include <Yulduz/Engine.h>

YULDUZ_Engine *g_pEngine = nullptr;

bool YULDUZ_InitializeEngine(YULDUZ_Engine *engine, YULDUZ_NULLABLE const YULDUZ_EngineInitializeInfo *initialize_info) {
    SDL_zerop(engine);

    uint32_t initial_entity_capacity         = 64;
    uint32_t initial_component_capacity      = 64;
    uint32_t initial_archetype_capacity      = 64;
    uint32_t initial_archetype_type_capacity = 64;

    if (nullptr != initialize_info) {
        uint32_t initial_entity_capacity         = initialize_info->InitialEntityCapacity;
        uint32_t initial_component_capacity      = initialize_info->InitialComponentCapacity;
        uint32_t initial_archetype_capacity      = initialize_info->InitialArchetypeCapacity;
        uint32_t initial_archetype_type_capacity = initialize_info->InitialArchetypeTypeCapacity;
    }

    YULDUZ_ECSRegistryInitializeInfo ecs_registry_initialize_info = {0};

    ecs_registry_initialize_info.InitialEntityCapacity    = initial_entity_capacity;
    ecs_registry_initialize_info.InitialComponentCapacity = initial_component_capacity;
    ecs_registry_initialize_info.InitialArchetypeCapacity = initial_archetype_capacity;

    if (!YULDUZ_InitializeECSRegistry(&engine->ECSRegistry, &ecs_registry_initialize_info)) {
        return false;
    }

    SDL_SetAtomicU32(&engine->IsRunning, false);

    g_pEngine = engine;

    return true;
}

void YULDUZ_ReleaseEngine(YULDUZ_Engine *engine) {
    YULDUZ_ReleaseECSRegistry(&engine->ECSRegistry);
    SDL_zerop(engine);
}

bool YULDUZ_RunEngine(YULDUZ_Engine *engine) {
    SDL_SetAtomicU32(&engine->IsRunning, true);

    return true;
}

YULDUZ_Engine *YULDUZ_GetEngine() {
    return g_pEngine;
}
