#include <Yulduz/Engine.h>

YULDUZ_Engine *g_pEngine = nullptr;

bool YULDUZ_InitializeEngine(YULDUZ_Engine *engine, YULDUZ_NULLABLE const YULDUZ_EngineInitializeInfo *initialize_info) {
    SDL_zerop(engine);

    uint32_t initial_component_types_capacity = 64;
    uint32_t initial_entities_capacity        = 64;

    if (nullptr != initialize_info) {
        initial_entities_capacity        = initialize_info->InitialEntitiesCapacity;
        initial_component_types_capacity = initialize_info->InitialComponentTypesCapacity;
    }

    // if (!YULDUZ_InitializeEntityRegistry(&engine->EntityRegistry, initial_entities_capacity)) {
        // return false;
    // }
    // if (!YULDUZ_InitializeTypeRegistry(&engine->ComponentTypeRegistry, initial_component_types_capacity)) {
        // return false;
    // }

    SDL_SetAtomicU32(&engine->IsRunning, false);

    g_pEngine = engine;

    return true;
}

void YULDUZ_ReleaseEngine(YULDUZ_Engine *engine) {
    // YULDUZ_ReleaseTypeRegistry(&engine->ComponentTypeRegistry);
    // YULDUZ_ReleaseEntityRegistry(&engine->EntityRegistry);

    SDL_zerop(engine);
}

bool YULDUZ_RunEngine(YULDUZ_Engine *engine) {
    SDL_SetAtomicU32(&engine->IsRunning, true);

    return true;
}

YULDUZ_Engine *YULDUZ_GetEngine() {
    return g_pEngine;
}
