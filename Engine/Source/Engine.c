#include <Yulduz/Engine.h>

YULDUZ_Engine *YULDUZ_g_pEngine = nullptr;

bool YULDUZ_InitializeEngine(
    YULDUZ_Engine *engine, YULDUZ_NULLABLE const YULDUZ_EngineInitializeInfo *initialize_info) {
    SDL_zerop(engine);

    uint32_t initial_component_type_capacity = 64;
    uint32_t initial_tag_type_capacity       = 64;
    uint32_t initial_entity_capacity         = 64;

    YULDUZ_ECSRegistryInitializeInfo ecs_registry_initialize_info = (YULDUZ_ECSRegistryInitializeInfo){
        .ComponentTypeRegistry = &engine->ComponentTypeRegistry,
        .TagTypeRegistry       = &engine->TagTypeRegistry,
        .EntityRegistry        = &engine->EntityRegistry,

        .InitialArchetypeCapacity     = 64,
        .InitialArchetypeTypeCapacity = 64,
    };

    if (nullptr != initialize_info) {
        initial_component_type_capacity = initialize_info->InitialComponentTypeCapacity;
        initial_tag_type_capacity       = initialize_info->InitialTagTypeCapacity;
        initial_entity_capacity         = initialize_info->InitialEntityCapacity;

        ecs_registry_initialize_info.InitialArchetypeCapacity     = initialize_info->InitialArchetypeCapacity;
        ecs_registry_initialize_info.InitialArchetypeTypeCapacity = initialize_info->InitialArchetypeTypeCapacity;
    }

    if (!YULDUZ_InitializeComponentTypeRegistry(&engine->ComponentTypeRegistry, initial_component_type_capacity)) {
        return false;
    }
    if (!YULDUZ_InitializeTagTypeRegistry(&engine->TagTypeRegistry, initial_tag_type_capacity)) {
        return false;
    }
    if (!YULDUZ_InitializeEntityRegistry(&engine->EntityRegistry, initial_entity_capacity)) {
        return false;
    }
    if (!YULDUZ_InitializeECSRegistry(&engine->ECSRegistry, &ecs_registry_initialize_info)) {
        return false;
    }

    SDL_SetAtomicU32(&engine->IsRunning, false);

    YULDUZ_g_pEngine = engine;

    return true;
}

void YULDUZ_ReleaseEngine(YULDUZ_Engine *engine) {
    YULDUZ_ReleaseECSRegistry(&engine->ECSRegistry);
    YULDUZ_ReleaseEntityRegistry(&engine->EntityRegistry);
    YULDUZ_ReleaseTagTypeRegistry(&engine->TagTypeRegistry);
    YULDUZ_ReleaseComponentTypeRegistry(&engine->ComponentTypeRegistry);

    SDL_zerop(engine);
}

void YULDUZ_StartEngine(YULDUZ_Engine *engine) {
    SDL_SetAtomicU32(&engine->IsRunning, true);
}

void YULDUZ_StopEngine(YULDUZ_Engine *engine) {
    SDL_SetAtomicU32(&engine->IsRunning, false);
}

bool YULDUZ_IsRunningInEngine(YULDUZ_Engine *engine) {
    return SDL_GetAtomicU32(&engine->IsRunning);
}
