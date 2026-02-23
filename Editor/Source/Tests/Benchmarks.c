#include <Tests/Tests.h>
#include <Yulduz/Engine.h>

// ============================================================
// BENCHMARK TYPES
// ============================================================

typedef struct TestPosition {
    float x, y, z;
} TestPosition;

typedef struct TestVelocity {
    float x, y, z;
} TestVelocity;

typedef struct TestHealth {
    float current, max;
} TestHealth;

typedef struct TestDamage {
    float amount;
} TestDamage;

typedef struct TestSprite {
    uint32_t texture_id;
} TestSprite;

typedef struct TestTransform {
    float m[16];
} TestTransform;

typedef struct TestLifecycle {
    uint32_t id;
    uint32_t value;
} TestLifecycle;

// ============================================================
// LIFECYCLE HOOKS FOR BENCHMARKING
// ============================================================

static uint32_t g_hook_call_count = 0;

static void BenchLifecycle_OnCreate(void *component, void *user_data) {
    (void)component;
    (void)user_data;
    g_hook_call_count++;
}

static void BenchLifecycle_OnDestroy(void *component, void *user_data) {
    (void)component;
    (void)user_data;
    g_hook_call_count++;
}

// ============================================================
// HELPER FUNCTIONS
// ============================================================

static bool setup_bench_ecs(YULDUZ_ECSRegistry *ecs) {
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialEntityCapacity        = 1024,
        .InitialTagTypeCapacity       = 32,
        .InitialComponentTypeCapacity = 32,
        .InitialArchetypeCapacity     = 128,
        .InitialArchetypeTypeCapacity = 128,
    };

    return YULDUZ_InitializeECSRegistry(ecs, &info);
}

// ============================================================
// ENTITY BENCHMARKS
// ============================================================

static void bench_entity_create_destroy(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    const uint32_t ITERATIONS = 1000000;

    BENCHMARK_START("Entity: Create/Destroy (no components)", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
        YULDUZ_CreateEntityInECSRegistry(&ecs, &entity);
        YULDUZ_DestroyEntityInECSRegistry(&ecs, entity);
    }

    BENCHMARK_END("Entity: Create/Destroy");

    YULDUZ_ReleaseECSRegistry(&ecs);
}

static void bench_entity_create_batch(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    const uint32_t ITERATIONS = 100000;

    BENCHMARK_START("Entity: Batch Create", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
        YULDUZ_CreateEntityInECSRegistry(&ecs, &entity);
    }

    BENCHMARK_END("Entity: Batch Create");

    YULDUZ_ReleaseECSRegistry(&ecs);
}

static void bench_entity_generation_reuse(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    // Pre-create and destroy entities to fill free list
    YULDUZ_Entity entities[1000];
    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_CreateEntityInECSRegistry(&ecs, &entities[i]);
    }
    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_DestroyEntityInECSRegistry(&ecs, entities[i]);
    }

    const uint32_t ITERATIONS = 1000000;

    BENCHMARK_START("Entity: Create (with generation reuse)", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
        YULDUZ_CreateEntityInECSRegistry(&ecs, &entity);
        YULDUZ_DestroyEntityInECSRegistry(&ecs, entity);
    }

    BENCHMARK_END("Entity: Create (reuse)");

    YULDUZ_ReleaseECSRegistry(&ecs);
}

// ============================================================
// COMPONENT BENCHMARKS (NO HOOKS)
// ============================================================

static void bench_component_add_remove_pod(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type);

    // Create entity
    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    YULDUZ_CreateEntityInECSRegistry(&ecs, &entity);

    const uint32_t ITERATIONS = 1000000;
    TestPosition   pos        = {1.0f, 2.0f, 3.0f};

    BENCHMARK_START("Component: Add/Remove POD (no hooks)", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &pos);
        YULDUZ_RemoveComponentWithTypeInECSRegistry(&ecs, entity, pos_type);
    }

    BENCHMARK_END("Component: Add/Remove POD");

    YULDUZ_ReleaseECSRegistry(&ecs);
}

static void bench_component_add_with_hooks(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    g_hook_call_count = 0;

    YULDUZ_ComponentType lc_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Lifecycle", .Size = sizeof(TestLifecycle), .Alignment = _Alignof(TestLifecycle), .OnCreatePFN = BenchLifecycle_OnCreate, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &lc_type);

    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    YULDUZ_CreateEntityInECSRegistry(&ecs, &entity);

    const uint32_t ITERATIONS = 1000000;
    TestLifecycle  lc         = {0};

    BENCHMARK_START("Component: Add with OnCreate hook", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, lc_type, &lc);
        YULDUZ_RemoveComponentWithTypeInECSRegistry(&ecs, entity, lc_type);
    }

    BENCHMARK_END("Component: Add with hook");

    YULDUZ_LOG_INFO("  Hook called %u times", g_hook_call_count);

    YULDUZ_ReleaseECSRegistry(&ecs);
}

static void bench_component_remove_with_hooks(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    g_hook_call_count = 0;

    YULDUZ_ComponentType lc_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Lifecycle", .Size = sizeof(TestLifecycle), .Alignment = _Alignof(TestLifecycle), .OnCreatePFN = NULL, .OnDestroyPFN = BenchLifecycle_OnDestroy, .OnClonePFN = NULL, .UserData = NULL}, &lc_type);

    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    YULDUZ_CreateEntityInECSRegistry(&ecs, &entity);

    const uint32_t ITERATIONS = 1000000;
    TestLifecycle  lc         = {0};

    BENCHMARK_START("Component: Remove with OnDestroy hook", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, lc_type, &lc);
        YULDUZ_RemoveComponentWithTypeInECSRegistry(&ecs, entity, lc_type);
    }

    BENCHMARK_END("Component: Remove with hook");

    YULDUZ_LOG_INFO("  Hook called %u times", g_hook_call_count);

    YULDUZ_ReleaseECSRegistry(&ecs);
}

static void bench_component_get_set(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type);

    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    YULDUZ_CreateEntityInECSRegistry(&ecs, &entity);

    TestPosition pos = {1.0f, 2.0f, 3.0f};
    YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &pos);

    const uint32_t ITERATIONS = 10000000;
    TestPosition   retrieved  = {0};

    BENCHMARK_START("Component: Get", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &retrieved);
    }

    BENCHMARK_END("Component: Get");

    // Set benchmark
    BENCHMARK_START("Component: Set", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        pos.x += 1.0f;
        YULDUZ_SetComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &pos);
    }

    BENCHMARK_END("Component: Set");

    YULDUZ_ReleaseECSRegistry(&ecs);
}

// ============================================================
// TAG BENCHMARKS
// ============================================================

static void bench_tag_add_remove(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    YULDUZ_TagType tag = YULDUZ_INVALID_TAG_TYPE;
    YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "TestTag", &tag);

    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    YULDUZ_CreateEntityInECSRegistry(&ecs, &entity);

    const uint32_t ITERATIONS = 1000000;

    BENCHMARK_START("Tag: Add/Remove", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        YULDUZ_AddTagWithTypeInECSRegistry(&ecs, entity, tag);
        YULDUZ_RemoveTagWithTypeInECSRegistry(&ecs, entity, tag);
    }

    BENCHMARK_END("Tag: Add/Remove");

    YULDUZ_ReleaseECSRegistry(&ecs);
}

static void bench_tag_has_query(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    YULDUZ_TagType tag = YULDUZ_INVALID_TAG_TYPE;
    YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "TestTag", &tag);

    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    YULDUZ_CreateEntityInECSRegistry(&ecs, &entity);
    YULDUZ_AddTagWithTypeInECSRegistry(&ecs, entity, tag);

    const uint32_t ITERATIONS = 10000000;

    BENCHMARK_START("Tag: Has Query", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, tag);
    }

    BENCHMARK_END("Tag: Has Query");

    YULDUZ_ReleaseECSRegistry(&ecs);
}

// ============================================================
// ARCHETYPE BENCHMARKS
// ============================================================

static void bench_archetype_transitions(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType vel_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType hp_type  = YULDUZ_INVALID_COMPONENT_TYPE;

    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type);

    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Velocity", .Size = sizeof(TestVelocity), .Alignment = _Alignof(TestVelocity), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &vel_type);

    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Health", .Size = sizeof(TestHealth), .Alignment = _Alignof(TestHealth), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &hp_type);

    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    YULDUZ_CreateEntityInECSRegistry(&ecs, &entity);

    TestPosition pos = {0};
    TestVelocity vel = {0};
    TestHealth   hp  = {0};

    const uint32_t ITERATIONS = 100000;

    BENCHMARK_START("Archetype: Add Component Transitions", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        // [] → [Pos] → [Pos,Vel] → [Pos,Vel,HP]
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &pos);
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, vel_type, &vel);
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, hp_type, &hp);

        // [Pos,Vel,HP] → [Pos,Vel] → [Pos] → []
        YULDUZ_RemoveComponentWithTypeInECSRegistry(&ecs, entity, hp_type);
        YULDUZ_RemoveComponentWithTypeInECSRegistry(&ecs, entity, vel_type);
        YULDUZ_RemoveComponentWithTypeInECSRegistry(&ecs, entity, pos_type);
    }

    BENCHMARK_END("Archetype: Transitions");

    YULDUZ_ReleaseECSRegistry(&ecs);
}

static void bench_archetype_edge_caching(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type);

    // Pre-warm the edge cache
    YULDUZ_Entity warmup = YULDUZ_INVALID_ENTITY;
    YULDUZ_CreateEntityInECSRegistry(&ecs, &warmup);
    TestPosition pos = {0};
    YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, warmup, pos_type, &pos);
    YULDUZ_RemoveComponentWithTypeInECSRegistry(&ecs, warmup, pos_type);

    // Now benchmark with cached edges
    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    YULDUZ_CreateEntityInECSRegistry(&ecs, &entity);

    const uint32_t ITERATIONS = 1000000;

    BENCHMARK_START("Archetype: Edge Cache Hit", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &pos);
        YULDUZ_RemoveComponentWithTypeInECSRegistry(&ecs, entity, pos_type);
    }

    BENCHMARK_END("Archetype: Edge Cache");

    YULDUZ_ReleaseECSRegistry(&ecs);
}

// ============================================================
// SYSTEM BENCHMARKS
// ============================================================

static uint64_t g_system_entity_count = 0;

static void bench_system_callback(const YULDUZ_Archetype *archetype, void *user_data) {
    (void)user_data;
    g_system_entity_count += archetype->DenseCount;
}

static void bench_system_cold_start(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type);

    // Create 50 archetypes with entities
    TestPosition pos = {0};
    for (uint32_t i = 0; i < 50; i++) {
        YULDUZ_Entity e = YULDUZ_INVALID_ENTITY;
        YULDUZ_CreateEntityInECSRegistry(&ecs, &e);
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, pos_type, &pos);

        // Add tag to create different archetypes
        char tag_name[32];
        SDL_snprintf(tag_name, sizeof(tag_name), "Tag%u", i);
        YULDUZ_TagType tag = YULDUZ_INVALID_TAG_TYPE;
        YULDUZ_RegisterTagTypeInECSRegistry(&ecs, tag_name, &tag);
        YULDUZ_AddTagWithTypeInECSRegistry(&ecs, e, tag);
    }

    const uint32_t ITERATIONS = 10000;

    BENCHMARK_START("System: Cold Start (50 archetypes)", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        YULDUZ_Query query = {0};
        YULDUZ_InitializeQuery(&query, 4);
        YULDUZ_SetQueryWithComponentType(&query, pos_type, YULDUZ_QueryAccessType_Read);

        YULDUZ_System system = {0};
        YULDUZ_InitializeSystem(&system, "BenchSystem", NULL, &query, bench_system_callback);

        g_system_entity_count = 0;
        YULDUZ_RunSystem(&system, &ecs);

        YULDUZ_ReleaseSystem(&system);
        YULDUZ_ReleaseQuery(&query);
    }

    BENCHMARK_END("System: Cold Start");

    YULDUZ_ReleaseECSRegistry(&ecs);
}

static void bench_system_warm_execution(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type);

    // Create 10,000 entities in same archetype
    TestPosition pos = {0};
    for (uint32_t i = 0; i < 10000; i++) {
        YULDUZ_Entity e = YULDUZ_INVALID_ENTITY;
        YULDUZ_CreateEntityInECSRegistry(&ecs, &e);
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, pos_type, &pos);
    }

    // Create system
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 4);
    YULDUZ_SetQueryWithComponentType(&query, pos_type, YULDUZ_QueryAccessType_Read);

    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "BenchSystem", NULL, &query, bench_system_callback);

    // Warm up
    YULDUZ_RunSystem(&system, &ecs);

    const uint32_t ITERATIONS = 10000000;
    g_system_entity_count     = 0;

    BENCHMARK_START("System: Warm Execution (10k entities)", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        YULDUZ_RunSystem(&system, &ecs);
    }

    BENCHMARK_END("System: Warm Execution");

    YULDUZ_LOG_INFO("  Processed %llu entities total", (unsigned long long)g_system_entity_count);

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&ecs);
}

static void bench_system_with_filtering(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType vel_type = YULDUZ_INVALID_COMPONENT_TYPE;

    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type);

    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Velocity", .Size = sizeof(TestVelocity), .Alignment = _Alignof(TestVelocity), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &vel_type);

    YULDUZ_TagType active_tag = YULDUZ_INVALID_TAG_TYPE;
    YULDUZ_TagType dead_tag   = YULDUZ_INVALID_TAG_TYPE;
    YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "Active", &active_tag);
    YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "Dead", &dead_tag);

    TestPosition pos = {0};
    TestVelocity vel = {0};

    // Create entities with different configurations
    // 1000 with [Pos, Vel, Active] (matches)
    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_Entity e = YULDUZ_INVALID_ENTITY;
        YULDUZ_CreateEntityInECSRegistry(&ecs, &e);
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, pos_type, &pos);
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, vel_type, &vel);
        YULDUZ_AddTagWithTypeInECSRegistry(&ecs, e, active_tag);
    }

    // 1000 with [Pos, Vel, Dead] (filtered out)
    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_Entity e = YULDUZ_INVALID_ENTITY;
        YULDUZ_CreateEntityInECSRegistry(&ecs, &e);
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, pos_type, &pos);
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, vel_type, &vel);
        YULDUZ_AddTagWithTypeInECSRegistry(&ecs, e, dead_tag);
    }

    // 1000 with [Pos] (no Vel, filtered out)
    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_Entity e = YULDUZ_INVALID_ENTITY;
        YULDUZ_CreateEntityInECSRegistry(&ecs, &e);
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, pos_type, &pos);
        YULDUZ_AddTagWithTypeInECSRegistry(&ecs, e, active_tag);
    }

    // Query: WITH Pos, Vel, Active; WITHOUT Dead
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 8);
    YULDUZ_SetQueryWithComponentType(&query, pos_type, YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&query, vel_type, YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithTagType(&query, active_tag);
    YULDUZ_SetQueryWithoutTagType(&query, dead_tag);

    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "FilterSystem", NULL, &query, bench_system_callback);

    // Warm up
    YULDUZ_RunSystem(&system, &ecs);

    const uint32_t ITERATIONS = 1000000;
    g_system_entity_count     = 0;

    BENCHMARK_START("System: With Filtering (3k entities, 1k match)", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        YULDUZ_RunSystem(&system, &ecs);
    }

    BENCHMARK_END("System: With Filtering");

    YULDUZ_LOG_INFO("  Processed %llu entities total", (unsigned long long)g_system_entity_count);

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&ecs);
}

// ============================================================
// LARGE-SCALE BENCHMARKS
// ============================================================

static void bench_large_scale_entity_creation(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType vel_type = YULDUZ_INVALID_COMPONENT_TYPE;

    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type);

    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Velocity", .Size = sizeof(TestVelocity), .Alignment = _Alignof(TestVelocity), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &vel_type);

    const uint32_t ENTITY_COUNT = 100000;
    TestPosition   pos          = {0};
    TestVelocity   vel          = {0};

    BENCHMARK_START("Large Scale: Create 100k Entities with Components", ENTITY_COUNT);

    for (uint32_t i = 0; i < ENTITY_COUNT; i++) {
        YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
        YULDUZ_CreateEntityInECSRegistry(&ecs, &entity);
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &pos);
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, vel_type, &vel);
    }

    BENCHMARK_END("Large Scale: Creation");

    YULDUZ_ReleaseECSRegistry(&ecs);
}

static void bench_large_scale_system_iteration(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    YULDUZ_ComponentType types[5];

    for (uint32_t i = 0; i < 5; i++) {
        char name[32];
        SDL_snprintf(name, sizeof(name), "Component%u", i);

        YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = name, .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &types[i]);
    }

    // Create 10,000 entities across multiple archetypes
    TestPosition data = {0};
    for (uint32_t i = 0; i < 10000; i++) {
        YULDUZ_Entity e = YULDUZ_INVALID_ENTITY;
        YULDUZ_CreateEntityInECSRegistry(&ecs, &e);

        // Add components based on bit pattern
        uint32_t pattern = i % 32;
        for (uint32_t j = 0; j < 5; j++) {
            if ((pattern & (1u << j)) != 0) {
                YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, types[j], &data);
            }
        }
    }

    // Query for Component0
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 4);
    YULDUZ_SetQueryWithComponentType(&query, types[0], YULDUZ_QueryAccessType_Read);

    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "LargeSystem", NULL, &query, bench_system_callback);

    // Warm up
    YULDUZ_RunSystem(&system, &ecs);

    const uint32_t ITERATIONS = 100000;
    g_system_entity_count     = 0;

    BENCHMARK_START("Large Scale: System Iteration (10k entities, multi-archetype)", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        YULDUZ_RunSystem(&system, &ecs);
    }

    BENCHMARK_END("Large Scale: System");

    YULDUZ_LOG_INFO("  Processed %llu entities total", (unsigned long long)g_system_entity_count);

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&ecs);
}

static void bench_realistic_game_frame(void) {
    YULDUZ_ECSRegistry ecs = {0};
    setup_bench_ecs(&ecs);

    // Register components
    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType vel_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType hp_type  = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType dmg_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType spr_type = YULDUZ_INVALID_COMPONENT_TYPE;

    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type);

    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Velocity", .Size = sizeof(TestVelocity), .Alignment = _Alignof(TestVelocity), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &vel_type);

    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Health", .Size = sizeof(TestHealth), .Alignment = _Alignof(TestHealth), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &hp_type);

    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Damage", .Size = sizeof(TestDamage), .Alignment = _Alignof(TestDamage), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &dmg_type);

    YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Sprite", .Size = sizeof(TestSprite), .Alignment = _Alignof(TestSprite), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &spr_type);

    YULDUZ_TagType active_tag = YULDUZ_INVALID_TAG_TYPE;
    YULDUZ_TagType dead_tag   = YULDUZ_INVALID_TAG_TYPE;
    YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "Active", &active_tag);
    YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "Dead", &dead_tag);

    // Create 1000 entities with various configurations
    TestPosition pos = {0};
    TestVelocity vel = {0};
    TestHealth   hp  = {100.0f, 100.0f};
    TestDamage   dmg = {10.0f};
    TestSprite   spr = {1};

    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_Entity e = YULDUZ_INVALID_ENTITY;
        YULDUZ_CreateEntityInECSRegistry(&ecs, &e);

        // All have Position
        YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, pos_type, &pos);

        // 80% have Velocity
        if (i % 10 < 8) {
            YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, vel_type, &vel);
        }

        // 60% have Health
        if (i % 10 < 6) {
            YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, hp_type, &hp);
        }

        // 30% have Damage
        if (i % 10 < 3) {
            YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, dmg_type, &dmg);
        }

        // 50% have Sprite
        if (i % 2 == 0) {
            YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, spr_type, &spr);
        }

        // 90% are Active
        if (i % 10 < 9) {
            YULDUZ_AddTagWithTypeInECSRegistry(&ecs, e, active_tag);
        }
    }

    // Create 5 systems (typical game frame)
    // Movement System
    YULDUZ_Query movement_query = {0};
    YULDUZ_InitializeQuery(&movement_query, 4);
    YULDUZ_SetQueryWithComponentType(&movement_query, pos_type, YULDUZ_QueryAccessType_Write);
    YULDUZ_SetQueryWithComponentType(&movement_query, vel_type, YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithTagType(&movement_query, active_tag);

    YULDUZ_System movement_system = {0};
    YULDUZ_InitializeSystem(&movement_system, "Movement", NULL, &movement_query, bench_system_callback);

    // Render System
    YULDUZ_Query render_query = {0};
    YULDUZ_InitializeQuery(&render_query, 4);
    YULDUZ_SetQueryWithComponentType(&render_query, pos_type, YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&render_query, spr_type, YULDUZ_QueryAccessType_Read);

    YULDUZ_System render_system = {0};
    YULDUZ_InitializeSystem(&render_system, "Render", NULL, &render_query, bench_system_callback);

    // Health System
    YULDUZ_Query health_query = {0};
    YULDUZ_InitializeQuery(&health_query, 4);
    YULDUZ_SetQueryWithComponentType(&health_query, hp_type, YULDUZ_QueryAccessType_Write);
    YULDUZ_SetQueryWithTagType(&health_query, active_tag);
    YULDUZ_SetQueryWithoutTagType(&health_query, dead_tag);

    YULDUZ_System health_system = {0};
    YULDUZ_InitializeSystem(&health_system, "Health", NULL, &health_query, bench_system_callback);

    // Damage System
    YULDUZ_Query damage_query = {0};
    YULDUZ_InitializeQuery(&damage_query, 4);
    YULDUZ_SetQueryWithComponentType(&damage_query, hp_type, YULDUZ_QueryAccessType_Write);
    YULDUZ_SetQueryWithComponentType(&damage_query, dmg_type, YULDUZ_QueryAccessType_Read);

    YULDUZ_System damage_system = {0};
    YULDUZ_InitializeSystem(&damage_system, "Damage", NULL, &damage_query, bench_system_callback);

    // Cleanup System
    YULDUZ_Query cleanup_query = {0};
    YULDUZ_InitializeQuery(&cleanup_query, 4);
    YULDUZ_SetQueryWithTagType(&cleanup_query, dead_tag);

    YULDUZ_System cleanup_system = {0};
    YULDUZ_InitializeSystem(&cleanup_system, "Cleanup", NULL, &cleanup_query, bench_system_callback);

    // Warm up all systems
    YULDUZ_RunSystem(&movement_system, &ecs);
    YULDUZ_RunSystem(&render_system, &ecs);
    YULDUZ_RunSystem(&health_system, &ecs);
    YULDUZ_RunSystem(&damage_system, &ecs);
    YULDUZ_RunSystem(&cleanup_system, &ecs);

    const uint32_t ITERATIONS = 100000;
    g_system_entity_count     = 0;

    BENCHMARK_START("Realistic: Full Game Frame (5 systems, 1k entities)", ITERATIONS);

    for (uint32_t i = 0; i < ITERATIONS; i++) {
        YULDUZ_RunSystem(&movement_system, &ecs);
        YULDUZ_RunSystem(&render_system, &ecs);
        YULDUZ_RunSystem(&health_system, &ecs);
        YULDUZ_RunSystem(&damage_system, &ecs);
        YULDUZ_RunSystem(&cleanup_system, &ecs);
    }

    BENCHMARK_END("Realistic: Game Frame");

    YULDUZ_LOG_INFO("  Processed %llu entities total", (unsigned long long)g_system_entity_count);

    // Cleanup
    YULDUZ_ReleaseSystem(&movement_system);
    YULDUZ_ReleaseSystem(&render_system);
    YULDUZ_ReleaseSystem(&health_system);
    YULDUZ_ReleaseSystem(&damage_system);
    YULDUZ_ReleaseSystem(&cleanup_system);

    YULDUZ_ReleaseQuery(&movement_query);
    YULDUZ_ReleaseQuery(&render_query);
    YULDUZ_ReleaseQuery(&health_query);
    YULDUZ_ReleaseQuery(&damage_query);
    YULDUZ_ReleaseQuery(&cleanup_query);

    YULDUZ_ReleaseECSRegistry(&ecs);
}

// ============================================================
// ENTRY POINT
// ============================================================

void run_all_ecs_benchmarks(void) {
    YULDUZ_LOG_INFO("");
    YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
    YULDUZ_LOG_INFO("║           ECS COMPREHENSIVE BENCHMARKS                         ║");
    YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");
    YULDUZ_LOG_INFO("");

    // Entity benchmarks
    YULDUZ_LOG_INFO("--- Entity Operations ---");
    bench_entity_create_destroy();
    bench_entity_create_batch();
    bench_entity_generation_reuse();
    YULDUZ_LOG_INFO("");

    // Component benchmarks
    YULDUZ_LOG_INFO("--- Component Operations ---");
    bench_component_add_remove_pod();
    bench_component_add_with_hooks();
    bench_component_remove_with_hooks();
    bench_component_get_set();
    YULDUZ_LOG_INFO("");

    // Tag benchmarks
    YULDUZ_LOG_INFO("--- Tag Operations ---");
    bench_tag_add_remove();
    bench_tag_has_query();
    YULDUZ_LOG_INFO("");

    // Archetype benchmarks
    YULDUZ_LOG_INFO("--- Archetype Operations ---");
    bench_archetype_transitions();
    bench_archetype_edge_caching();
    YULDUZ_LOG_INFO("");

    // System benchmarks
    YULDUZ_LOG_INFO("--- System Execution ---");
    bench_system_cold_start();
    bench_system_warm_execution();
    bench_system_with_filtering();
    YULDUZ_LOG_INFO("");

    // Large-scale benchmarks
    YULDUZ_LOG_INFO("--- Large-Scale Scenarios ---");
    bench_large_scale_entity_creation();
    bench_large_scale_system_iteration();
    bench_realistic_game_frame();
    YULDUZ_LOG_INFO("");

    PRINT_BENCHMARK_RESULTS();
}