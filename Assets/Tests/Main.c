#include <Yulduz/Engine.h>

// Test component types
typedef struct Position {
    float x, y, z;
} Position;

typedef struct Velocity {
    float x, y, z;
} Velocity;

typedef struct Health {
    float current;
    float max;
} Health;

typedef struct Damage {
    float value;
} Damage;

typedef struct Name {
    char buffer[64];
} Name;

void AssertFN(void *user_data, YULDUZ_AssertEntry *entry);
void LogFN(YULDUZ_Logger *logger, YULDUZ_LogEntry *entry);

// Global component types
YULDUZ_Type g_position_type = YULDUZ_INVALID_TYPE;
YULDUZ_Type g_velocity_type = YULDUZ_INVALID_TYPE;
YULDUZ_Type g_health_type   = YULDUZ_INVALID_TYPE;
YULDUZ_Type g_damage_type   = YULDUZ_INVALID_TYPE;
YULDUZ_Type g_name_type     = YULDUZ_INVALID_TYPE;
YULDUZ_Type g_tag_player    = YULDUZ_INVALID_TYPE;
YULDUZ_Type g_tag_enemy     = YULDUZ_INVALID_TYPE;
YULDUZ_Type g_tag_dead      = YULDUZ_INVALID_TYPE;

// Test functions
void setup_component_registry(YULDUZ_TypeRegistry *registry);
void test_basic_add_remove(YULDUZ_TypeRegistry *registry);
void test_move_entities_between_archetypes(YULDUZ_TypeRegistry *registry);
void test_query_stores(YULDUZ_TypeRegistry *registry);
void test_capacity_growth(YULDUZ_TypeRegistry *registry);
void test_remove_with_swapback(YULDUZ_TypeRegistry *registry);
void test_mixed_operations(YULDUZ_TypeRegistry *registry);
void test_sorting_functions(void);

// New stress tests
void test_move_edge_cases(YULDUZ_TypeRegistry *registry);
void test_move_component_overlap(YULDUZ_TypeRegistry *registry);
void test_move_with_null_data(YULDUZ_TypeRegistry *registry);
void benchmark_add_performance(YULDUZ_TypeRegistry *registry);
void benchmark_remove_performance(YULDUZ_TypeRegistry *registry);
void benchmark_move_performance(YULDUZ_TypeRegistry *registry);
void benchmark_query_performance(YULDUZ_TypeRegistry *registry);

// ============================================================================
// ENTITY REGISTRY TEST FUNCTIONS
// Add these function declarations near line 53 in Main.c (after benchmark_query_performance)
// ============================================================================

void test_entity_registry_basic(void);
void test_entity_registry_reuse(void);
void test_entity_registry_records(void);
void test_entity_registry_invalid_ops(void);
void test_entity_registry_growth(void);
void test_entity_registry_free_list(void);
void test_entity_registry_archetype_assignment(void);
void test_entity_registry_batch_ops(void);
void stress_entity_registry_cycles(void);
void stress_entity_registry_random(void);
void benchmark_entity_creation(void);
void benchmark_entity_destruction(void);
void benchmark_entity_reuse(void);
void benchmark_entity_get_records(void);
void benchmark_entity_set_records(void);
void benchmark_entity_mixed_ops(void);

// ECS Registry Tests
void test_ecs_null_archetype(void);
void test_ecs_add_remove_tags(void);
void test_ecs_add_remove_components(void);
void test_ecs_component_migration(void);
void test_ecs_tag_component_combinations(void);
void test_ecs_archetype_reuse(void);
void test_ecs_set_get_components(void);
void test_ecs_invalid_operations(void);
void test_ecs_edge_cases(void);
void test_ecs_entity_destruction(void);
void test_ecs_archetype_growth(void);
void test_ecs_multiple_migrations(void);
void test_ecs_zero_size_components(void);
void stress_ecs_create_destroy_cycles(void);
void stress_ecs_component_churn(void);
void stress_ecs_archetype_explosion(void);
void benchmark_ecs_entity_spawn(void);
void benchmark_ecs_add_component(void);
void benchmark_ecs_remove_component(void);
void benchmark_ecs_add_tag(void);
void benchmark_ecs_get_component(void);
void benchmark_ecs_set_component(void);
void benchmark_ecs_full_workflow(void);
void benchmark_ecs_archetype_lookup(void);

int32_t main(int32_t argc, char **argv) {
    (void)argc;
    (void)argv;

    SDL_Init(SDL_INIT_VIDEO);

    // IMPORTANT: Need exactly 2 loggers - engine and application
    YULDUZ_Logger *loggers = SDL_stack_alloc(YULDUZ_Logger, 2);

    // Engine logger
    loggers[0].Name        = "ENGINE";
    loggers[0].LogPFN      = &LogFN;
    loggers[0].MinLogLevel = YULDUZ_LogLevel_Info;
    loggers[0].UserData    = nullptr;

    // Application logger
    loggers[1].Name        = "TEST";
    loggers[1].LogPFN      = &LogFN;
    loggers[1].MinLogLevel = YULDUZ_LogLevel_Info;
    loggers[1].UserData    = nullptr;

    YULDUZ_InitializeAsserts(&AssertFN, nullptr);
    YULDUZ_InitializeLogs(loggers);

    YULDUZ_LOG_INFO("=== ARCHETYPE STRESS TESTS ===");

    // Setup component type registry
    YULDUZ_TypeRegistry registry;
    YULDUZ_InitializeTypeRegistry(&registry, 16);
    setup_component_registry(&registry);

    // Basic correctness tests
    YULDUZ_LOG_INFO("\n--- CORRECTNESS TESTS ---");
    test_sorting_functions();
    test_basic_add_remove(&registry);
    test_move_entities_between_archetypes(&registry);
    test_query_stores(&registry);
    test_capacity_growth(&registry);
    test_remove_with_swapback(&registry);
    test_mixed_operations(&registry);

    // Move edge case tests
    YULDUZ_LOG_INFO("\n--- MOVE EDGE CASE TESTS ---");
    test_move_edge_cases(&registry);
    test_move_component_overlap(&registry);
    test_move_with_null_data(&registry);

    // Performance benchmarks
    YULDUZ_LOG_INFO("\n--- PERFORMANCE BENCHMARKS ---");
    benchmark_add_performance(&registry);
    benchmark_remove_performance(&registry);
    benchmark_move_performance(&registry);
    benchmark_query_performance(&registry);

    YULDUZ_LOG_INFO("\n--- ENTITY REGISTRY TESTS ---");
    test_entity_registry_basic();
    test_entity_registry_reuse();
    test_entity_registry_records();
    test_entity_registry_invalid_ops();
    test_entity_registry_growth();
    test_entity_registry_free_list();
    test_entity_registry_archetype_assignment();
    test_entity_registry_batch_ops();

    // Entity Registry Stress Tests
    YULDUZ_LOG_INFO("\n--- ENTITY REGISTRY STRESS TESTS ---");
    stress_entity_registry_cycles();
    stress_entity_registry_random();

    // Entity Registry Benchmarks
    YULDUZ_LOG_INFO("\n--- ENTITY REGISTRY BENCHMARKS ---");
    benchmark_entity_creation();
    benchmark_entity_destruction();
    benchmark_entity_reuse();
    benchmark_entity_get_records();
    benchmark_entity_set_records();
    benchmark_entity_mixed_ops();

    YULDUZ_LOG_INFO("\n--- ECS REGISTRY TESTS ---");
    test_ecs_null_archetype();
    test_ecs_add_remove_tags();
    test_ecs_add_remove_components();
    test_ecs_component_migration();
    test_ecs_tag_component_combinations();
    test_ecs_archetype_reuse();
    test_ecs_set_get_components();
    test_ecs_invalid_operations();
    test_ecs_edge_cases();
    test_ecs_entity_destruction();
    test_ecs_archetype_growth();
    test_ecs_multiple_migrations();
    test_ecs_zero_size_components();

    YULDUZ_LOG_INFO("\n--- ECS REGISTRY STRESS TESTS ---");
    stress_ecs_create_destroy_cycles();
    stress_ecs_component_churn();
    stress_ecs_archetype_explosion();

    YULDUZ_LOG_INFO("\n--- ECS REGISTRY BENCHMARKS ---");
    benchmark_ecs_entity_spawn();
    benchmark_ecs_add_component();
    benchmark_ecs_remove_component();
    benchmark_ecs_add_tag();
    benchmark_ecs_get_component();
    benchmark_ecs_set_component();
    benchmark_ecs_full_workflow();
    benchmark_ecs_archetype_lookup();

    YULDUZ_ReleaseTypeRegistry(&registry);

    YULDUZ_LOG_INFO("\n=== ALL TESTS PASSED ===");

    YULDUZ_ReleaseLogs();
    YULDUZ_ReleaseAsserts();
    SDL_stack_free(loggers);
    SDL_Quit();

    return 0;
}

void setup_component_registry(YULDUZ_TypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[SETUP] Registering Component Types");

    YULDUZ_TypeDescription type_infos[] = {
        {"Position", sizeof(Position), alignof(Position)},
        {"Velocity", sizeof(Velocity), alignof(Velocity)},
        {"Health", sizeof(Health), alignof(Health)},
        {"Damage", sizeof(Damage), alignof(Damage)},
        {"Name", sizeof(Name), alignof(Name)}};

    YULDUZ_Type types[5];

    bool result = YULDUZ_RegisterTypesInTypeRegistry(
        registry, type_infos, types, 5);

    SDL_assert(result);

    g_position_type = types[0];
    g_velocity_type = types[1];
    g_health_type   = types[2];
    g_damage_type   = types[3];
    g_name_type     = types[4];

    YULDUZ_LOG_INFO("  ✓ Registered 5 component types");
}

void test_sorting_functions(void) {
    YULDUZ_LOG_INFO("\n[TEST] Sorting Functions");

    YULDUZ_Type types[] = {5, 2, 8, 1, 3};
    YULDUZ_SDL_SortTypes(types, 5);
    SDL_assert(types[0] == 1 && types[4] == 8);

    YULDUZ_LOG_INFO("  ✓ Sorting Functions PASSED");
}

void test_basic_add_remove(YULDUZ_TypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Basic Add/Remove");

    YULDUZ_Type            query_types[] = {g_position_type, g_velocity_type};
    YULDUZ_TypeDescription type_infos[2];
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, query_types, type_infos, 2);

    YULDUZ_TypeInfo types[] = {
        {g_position_type, type_infos[0].Size, type_infos[0].Alignment},
        {g_velocity_type, type_infos[1].Size, type_infos[1].Alignment}};
    YULDUZ_SDL_SortTypeDescriptions(types, 2);

    YULDUZ_Archetype archetype;
    YULDUZ_InitializeArchetype(&archetype, types, 2, nullptr, 0, 4);

    for (uint32_t i = 0; i < 10; i++) {
        Position pos = {(float)i, (float)i * 2, (float)i * 3};
        Velocity vel = {1.0f, 2.0f, 3.0f};

        YULDUZ_DataInfo data[] = {
            {g_position_type, 0, &pos},
            {g_velocity_type, 0, &vel}};
        YULDUZ_SDL_SortDataInfos(data, 2);

        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, (YULDUZ_Entity)i, data, &index);
    }

    YULDUZ_ComponentStore *pos_store = YULDUZ_QueryStoreInArchetype(&archetype, g_position_type);
    Position              *positions = (Position *)pos_store->Dense;

    for (uint32_t i = 0; i < 10; i++) {
        SDL_assert(positions[i].x == (float)i);
    }

    YULDUZ_RemoveInArchetype(&archetype, 5);
    YULDUZ_RemoveInArchetype(&archetype, 0);
    SDL_assert(archetype.DenseCount == 8);

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_LOG_INFO("  ✓ Basic Add/Remove PASSED");
}

void test_move_entities_between_archetypes(YULDUZ_TypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Move Entities Between Archetypes");

    YULDUZ_Type src_query[] = {g_position_type, g_velocity_type};
    YULDUZ_Type dst_query[] = {g_position_type, g_velocity_type, g_health_type};

    YULDUZ_TypeDescription src_infos[2], dst_infos[3];
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, src_query, src_infos, 2);
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, dst_query, dst_infos, 3);

    YULDUZ_TypeInfo src_types[] = {
        {g_position_type, src_infos[0].Size, src_infos[0].Alignment},
        {g_velocity_type, src_infos[1].Size, src_infos[1].Alignment}};
    YULDUZ_SDL_SortTypeDescriptions(src_types, 2);

    YULDUZ_TypeInfo dst_types[] = {
        {g_position_type, dst_infos[0].Size, dst_infos[0].Alignment},
        {g_velocity_type, dst_infos[1].Size, dst_infos[1].Alignment},
        {g_health_type, dst_infos[2].Size, dst_infos[2].Alignment}};
    YULDUZ_SDL_SortTypeDescriptions(dst_types, 3);

    YULDUZ_Archetype src_arch, dst_arch;
    YULDUZ_InitializeArchetype(&src_arch, src_types, 2, nullptr, 0, 4);
    YULDUZ_InitializeArchetype(&dst_arch, dst_types, 3, nullptr, 0, 4);

    Position pos = {10.0f, 20.0f, 30.0f};
    Velocity vel = {1.0f, 2.0f, 3.0f};

    YULDUZ_DataInfo src_data[] = {
        {g_position_type, 0, &pos},
        {g_velocity_type, 0, &vel}};
    YULDUZ_SDL_SortDataInfos(src_data, 2);

    YULDUZ_ArchetypeIndex src_index;
    YULDUZ_AddInArchetype(&src_arch, 100, src_data, &src_index);

    Health          health     = {100.0f, 100.0f};
    YULDUZ_DataInfo new_data[] = {{g_health_type, 0, &health}};

    YULDUZ_ArchetypeIndex dst_index;
    YULDUZ_MoveEntityInArchetype(&src_arch, &dst_arch, new_data, 1, src_index, &dst_index);

    SDL_assert(src_arch.DenseCount == 0);
    SDL_assert(dst_arch.DenseCount == 1);

    YULDUZ_ComponentStore *dst_pos = YULDUZ_QueryStoreInArchetype(&dst_arch, g_position_type);
    Position              *p       = (Position *)YULDUZ_GetComponentInComponentStore(dst_pos, 0);
    SDL_assert(p->x == 10.0f && p->y == 20.0f && p->z == 30.0f);

    YULDUZ_ReleaseArchetype(&src_arch);
    YULDUZ_ReleaseArchetype(&dst_arch);
    YULDUZ_LOG_INFO("  ✓ Move Between Archetypes PASSED");
}

void test_query_stores(YULDUZ_TypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Query Stores");

    YULDUZ_Type            query[] = {g_position_type, g_velocity_type, g_health_type};
    YULDUZ_TypeDescription type_infos[3];
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, query, type_infos, 3);

    YULDUZ_TypeInfo types[] = {
        {g_position_type, type_infos[0].Size, type_infos[0].Alignment},
        {g_velocity_type, type_infos[1].Size, type_infos[1].Alignment},
        {g_health_type, type_infos[2].Size, type_infos[2].Alignment}};
    YULDUZ_SDL_SortTypeDescriptions(types, 3);

    YULDUZ_Archetype archetype;
    YULDUZ_InitializeArchetype(&archetype, types, 3, nullptr, 0, 4);

    YULDUZ_Type            query_types[] = {g_health_type, g_position_type};
    YULDUZ_ComponentStore *stores[2];

    bool result = YULDUZ_QueryStoresInArchetype(&archetype, query_types, 2, nullptr, 0, stores);
    SDL_assert(result);
    SDL_assert(stores[0]->Type == g_health_type);
    SDL_assert(stores[1]->Type == g_position_type);

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_LOG_INFO("  ✓ Query Stores PASSED");
}

void test_capacity_growth(YULDUZ_TypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Capacity Growth");

    YULDUZ_Type            query[] = {g_position_type};
    YULDUZ_TypeDescription type_info;
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, query, &type_info, 1);

    YULDUZ_TypeInfo types[] = {
        {g_position_type, type_info.Size, type_info.Alignment}};

    YULDUZ_Archetype archetype;
    YULDUZ_InitializeArchetype(&archetype, types, 1, nullptr, 0, 2);

    for (uint32_t i = 0; i < 100; i++) {
        Position              pos    = {(float)i, (float)i, (float)i};
        YULDUZ_DataInfo       data[] = {{g_position_type, 0, &pos}};
        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, (YULDUZ_Entity)i, data, &index);
    }

    SDL_assert(archetype.DenseCount == 100);

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_LOG_INFO("  ✓ Capacity Growth PASSED");
}

void test_remove_with_swapback(YULDUZ_TypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Remove with Swap-Back");

    YULDUZ_Type            query[] = {g_position_type};
    YULDUZ_TypeDescription type_info;
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, query, &type_info, 1);

    YULDUZ_TypeInfo types[] = {
        {g_position_type, type_info.Size, type_info.Alignment}};

    YULDUZ_Archetype archetype;
    YULDUZ_InitializeArchetype(&archetype, types, 1, nullptr, 0, 10);

    for (uint32_t i = 0; i < 5; i++) {
        Position              pos    = {(float)i * 100, 0, 0};
        YULDUZ_DataInfo       data[] = {{g_position_type, 0, &pos}};
        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, (YULDUZ_Entity)(1000 + i), data, &index);
    }

    YULDUZ_ComponentStore *store     = YULDUZ_QueryStoreInArchetype(&archetype, g_position_type);
    Position              *positions = (Position *)store->Dense;

    YULDUZ_RemoveInArchetype(&archetype, 2);
    SDL_assert(archetype.DenseCount == 4);
    SDL_assert(positions[2].x == 400.0f);

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_LOG_INFO("  ✓ Remove with Swap-Back PASSED");
}

void test_mixed_operations(YULDUZ_TypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Mixed Operations");

    YULDUZ_Type            query[] = {g_position_type, g_velocity_type, g_health_type};
    YULDUZ_TypeDescription type_infos[3];
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, query, type_infos, 3);

    YULDUZ_TypeInfo types[] = {
        {g_position_type, type_infos[0].Size, type_infos[0].Alignment},
        {g_velocity_type, type_infos[1].Size, type_infos[1].Alignment},
        {g_health_type, type_infos[2].Size, type_infos[2].Alignment}};
    YULDUZ_SDL_SortTypeDescriptions(types, 3);

    YULDUZ_Archetype archetype;
    YULDUZ_InitializeArchetype(&archetype, types, 3, nullptr, 0, 8);

    uint32_t operations = 1000;
    uint32_t seed       = (uint32_t)time(nullptr);

    for (uint32_t op = 0; op < operations; op++) {
        uint32_t choice = (seed * 1103515245 + 12345) % 100;
        seed            = choice;

        if (choice < 60 && archetype.DenseCount < 100) {
            Position pos    = {(float)op, (float)op, (float)op};
            Velocity vel    = {1, 1, 1};
            Health   health = {100, 100};

            YULDUZ_DataInfo data[] = {
                {g_position_type, 0, &pos},
                {g_velocity_type, 0, &vel},
                {g_health_type, 0, &health}};
            YULDUZ_SDL_SortDataInfos(data, 3);

            YULDUZ_ArchetypeIndex index;
            YULDUZ_AddInArchetype(&archetype, (YULDUZ_Entity)op, data, &index);
        } else if (archetype.DenseCount > 0) {
            uint32_t remove_idx = choice % archetype.DenseCount;
            YULDUZ_RemoveInArchetype(&archetype, remove_idx);
        }
    }

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_LOG_INFO("  ✓ Mixed Operations PASSED");
}

void test_move_edge_cases(YULDUZ_TypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Move Edge Cases");

    // Test 1: Move when destination has MORE components than source
    YULDUZ_Type src_q[] = {g_position_type};
    YULDUZ_Type dst_q[] = {g_position_type, g_velocity_type, g_health_type, g_damage_type};

    YULDUZ_TypeDescription src_infos[1], dst_infos[4];
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, src_q, src_infos, 1);
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, dst_q, dst_infos, 4);

    YULDUZ_TypeInfo src_types[] = {
        {g_position_type, src_infos[0].Size, src_infos[0].Alignment}};

    YULDUZ_TypeInfo dst_types[] = {
        {g_position_type, dst_infos[0].Size, dst_infos[0].Alignment},
        {g_velocity_type, dst_infos[1].Size, dst_infos[1].Alignment},
        {g_health_type, dst_infos[2].Size, dst_infos[2].Alignment},
        {g_damage_type, dst_infos[3].Size, dst_infos[3].Alignment}};
    YULDUZ_SDL_SortTypeDescriptions(dst_types, 4);

    YULDUZ_Archetype src, dst;
    YULDUZ_InitializeArchetype(&src, src_types, 1, nullptr, 0, 4);
    YULDUZ_InitializeArchetype(&dst, dst_types, 4, nullptr, 0, 4);

    Position        pos        = {42.0f, 43.0f, 44.0f};
    YULDUZ_DataInfo src_data[] = {{g_position_type, 0, &pos}};

    YULDUZ_ArchetypeIndex src_idx;
    YULDUZ_AddInArchetype(&src, 999, src_data, &src_idx);

    // Provide only 2 of the 3 new components
    Velocity        vel        = {5.0f, 6.0f, 7.0f};
    Health          health     = {50.0f, 100.0f};
    YULDUZ_DataInfo new_data[] = {
        {g_velocity_type, 0, &vel},
        {g_health_type, 0, &health}
        // Damage will be zero-initialized
    };
    YULDUZ_SDL_SortDataInfos(new_data, 2);

    YULDUZ_ArchetypeIndex dst_idx;
    YULDUZ_MoveEntityInArchetype(&src, &dst, new_data, 2, src_idx, &dst_idx);

    // Verify Position was preserved
    YULDUZ_ComponentStore *dst_pos = YULDUZ_QueryStoreInArchetype(&dst, g_position_type);
    Position              *p       = (Position *)YULDUZ_GetComponentInComponentStore(dst_pos, 0);
    SDL_assert(p->x == 42.0f && p->y == 43.0f && p->z == 44.0f);

    // Verify new components
    YULDUZ_ComponentStore *dst_vel = YULDUZ_QueryStoreInArchetype(&dst, g_velocity_type);
    Velocity              *v       = (Velocity *)YULDUZ_GetComponentInComponentStore(dst_vel, 0);
    SDL_assert(v->x == 5.0f);

    // Verify zero-initialized component
    YULDUZ_ComponentStore *dst_dmg = YULDUZ_QueryStoreInArchetype(&dst, g_damage_type);
    Damage                *d       = (Damage *)YULDUZ_GetComponentInComponentStore(dst_dmg, 0);
    SDL_assert(d->value == 0.0f);

    YULDUZ_ReleaseArchetype(&src);
    YULDUZ_ReleaseArchetype(&dst);

    YULDUZ_LOG_INFO("  ✓ Move with many new components works");
    YULDUZ_LOG_INFO("  ✓ Move Edge Cases PASSED");
}

void test_move_component_overlap(YULDUZ_TypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Move Component Overlap");

    // Test moving between archetypes with complete overlap
    YULDUZ_Type            query[] = {g_position_type, g_velocity_type};
    YULDUZ_TypeDescription type_infos[2];
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, query, type_infos, 2);

    YULDUZ_TypeInfo types[] = {
        {g_position_type, type_infos[0].Size, type_infos[0].Alignment},
        {g_velocity_type, type_infos[1].Size, type_infos[1].Alignment}};
    YULDUZ_SDL_SortTypeDescriptions(types, 2);

    YULDUZ_Archetype src, dst;
    YULDUZ_InitializeArchetype(&src, types, 2, nullptr, 0, 4);
    YULDUZ_InitializeArchetype(&dst, types, 2, nullptr, 0, 4);

    Position pos = {100.0f, 200.0f, 300.0f};
    Velocity vel = {10.0f, 20.0f, 30.0f};

    YULDUZ_DataInfo data[] = {
        {g_position_type, 0, &pos},
        {g_velocity_type, 0, &vel}};
    YULDUZ_SDL_SortDataInfos(data, 2);

    YULDUZ_ArchetypeIndex src_idx;
    YULDUZ_AddInArchetype(&src, 777, data, &src_idx);

    // Move with no new components (complete overlap)
    YULDUZ_ArchetypeIndex dst_idx;
    YULDUZ_MoveEntityInArchetype(&src, &dst, nullptr, 0, src_idx, &dst_idx);

    // Verify all data preserved
    YULDUZ_ComponentStore *dst_pos = YULDUZ_QueryStoreInArchetype(&dst, g_position_type);
    YULDUZ_ComponentStore *dst_vel = YULDUZ_QueryStoreInArchetype(&dst, g_velocity_type);

    Position *p = (Position *)YULDUZ_GetComponentInComponentStore(dst_pos, 0);
    Velocity *v = (Velocity *)YULDUZ_GetComponentInComponentStore(dst_vel, 0);

    SDL_assert(p->x == 100.0f && p->y == 200.0f && p->z == 300.0f);
    SDL_assert(v->x == 10.0f && v->y == 20.0f && v->z == 30.0f);

    YULDUZ_ReleaseArchetype(&src);
    YULDUZ_ReleaseArchetype(&dst);

    YULDUZ_LOG_INFO("  ✓ Move with complete component overlap works");
    YULDUZ_LOG_INFO("  ✓ Move Component Overlap PASSED");
}

void test_move_with_null_data(YULDUZ_TypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Move with NULL Data");

    YULDUZ_Type src_q[] = {g_position_type};
    YULDUZ_Type dst_q[] = {g_position_type, g_health_type};

    YULDUZ_TypeDescription src_infos[1], dst_infos[2];
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, src_q, src_infos, 1);
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, dst_q, dst_infos, 2);

    YULDUZ_TypeInfo src_types[] = {
        {g_position_type, src_infos[0].Size, src_infos[0].Alignment}};

    YULDUZ_TypeInfo dst_types[] = {
        {g_position_type, dst_infos[0].Size, dst_infos[0].Alignment},
        {g_health_type, dst_infos[1].Size, dst_infos[1].Alignment}};
    YULDUZ_SDL_SortTypeDescriptions(dst_types, 2);

    YULDUZ_Archetype src, dst;
    YULDUZ_InitializeArchetype(&src, src_types, 1, nullptr, 0, 4);
    YULDUZ_InitializeArchetype(&dst, dst_types, 2, nullptr, 0, 4);

    Position        pos        = {11.0f, 22.0f, 33.0f};
    YULDUZ_DataInfo src_data[] = {{g_position_type, 0, &pos}};

    YULDUZ_ArchetypeIndex src_idx;
    YULDUZ_AddInArchetype(&src, 555, src_data, &src_idx);

    // Pass NULL data for new component - should zero-initialize
    YULDUZ_DataInfo new_data[] = {{g_health_type, 0, nullptr}};

    YULDUZ_ArchetypeIndex dst_idx;
    YULDUZ_MoveEntityInArchetype(&src, &dst, new_data, 1, src_idx, &dst_idx);

    // Verify health was zero-initialized
    YULDUZ_ComponentStore *dst_health = YULDUZ_QueryStoreInArchetype(&dst, g_health_type);
    Health                *h          = (Health *)YULDUZ_GetComponentInComponentStore(dst_health, 0);
    SDL_assert(h->current == 0.0f && h->max == 0.0f);

    YULDUZ_ReleaseArchetype(&src);
    YULDUZ_ReleaseArchetype(&dst);

    YULDUZ_LOG_INFO("  ✓ NULL data zero-initializes correctly");
    YULDUZ_LOG_INFO("  ✓ Move with NULL Data PASSED");
}

void benchmark_add_performance(YULDUZ_TypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Add Performance");

    YULDUZ_Type            query[] = {g_position_type, g_velocity_type};
    YULDUZ_TypeDescription type_infos[2];
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, query, type_infos, 2);

    YULDUZ_TypeInfo types[] = {
        {g_position_type, type_infos[0].Size, type_infos[0].Alignment},
        {g_velocity_type, type_infos[1].Size, type_infos[1].Alignment}};
    YULDUZ_SDL_SortTypeDescriptions(types, 2);

    YULDUZ_Archetype archetype;
    YULDUZ_InitializeArchetype(&archetype, types, 2, nullptr, 0, 100000);

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < 100000; i++) {
        Position pos = {(float)i, (float)i, (float)i};
        Velocity vel = {1, 1, 1};

        YULDUZ_DataInfo data[] = {
            {g_position_type, 0, &pos},
            {g_velocity_type, 0, &vel}};
        YULDUZ_SDL_SortDataInfos(data, 2);

        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, (YULDUZ_Entity)i, data, &index);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double   ms  = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  100,000 adds: %.2f ms (%.0f ops/sec)", ms, 100000.0 / (ms / 1000.0));

    YULDUZ_ReleaseArchetype(&archetype);
}

void benchmark_remove_performance(YULDUZ_TypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Remove Performance");

    YULDUZ_Type            query[] = {g_position_type};
    YULDUZ_TypeDescription type_info;
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, query, &type_info, 1);

    YULDUZ_TypeInfo types[] = {
        {g_position_type, type_info.Size, type_info.Alignment}};

    YULDUZ_Archetype archetype;
    YULDUZ_InitializeArchetype(&archetype, types, 1, nullptr, 0, 50000);

    // Add 50000 entities
    for (uint32_t i = 0; i < 50000; i++) {
        Position              pos    = {(float)i, 0, 0};
        YULDUZ_DataInfo       data[] = {{g_position_type, 0, &pos}};
        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, (YULDUZ_Entity)i, data, &index);
    }

    uint64_t start = SDL_GetPerformanceCounter();

    // Remove all (from back for consistency)
    while (archetype.DenseCount > 0) {
        YULDUZ_RemoveInArchetype(&archetype, archetype.DenseCount - 1);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double   ms  = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  50,000 removes: %.2f ms (%.0f ops/sec)", ms, 50000.0 / (ms / 1000.0));

    YULDUZ_ReleaseArchetype(&archetype);
}

void benchmark_move_performance(YULDUZ_TypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Move Performance");

    YULDUZ_Type src_q[] = {g_position_type, g_velocity_type};
    YULDUZ_Type dst_q[] = {g_position_type, g_velocity_type, g_health_type};

    YULDUZ_TypeDescription src_infos[2], dst_infos[3];
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, src_q, src_infos, 2);
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, dst_q, dst_infos, 3);

    YULDUZ_TypeInfo src_types[] = {
        {g_position_type, src_infos[0].Size, src_infos[0].Alignment},
        {g_velocity_type, src_infos[1].Size, src_infos[1].Alignment}};
    YULDUZ_SDL_SortTypeDescriptions(src_types, 2);

    YULDUZ_TypeInfo dst_types[] = {
        {g_position_type, dst_infos[0].Size, dst_infos[0].Alignment},
        {g_velocity_type, dst_infos[1].Size, dst_infos[1].Alignment},
        {g_health_type, dst_infos[2].Size, dst_infos[2].Alignment}};
    YULDUZ_SDL_SortTypeDescriptions(dst_types, 3);

    YULDUZ_Archetype src, dst;
    YULDUZ_InitializeArchetype(&src, src_types, 2, nullptr, 0, 10000);
    YULDUZ_InitializeArchetype(&dst, dst_types, 3, nullptr, 0, 10000);

    // Add 10000 entities to source
    for (uint32_t i = 0; i < 10000; i++) {
        Position pos = {(float)i, (float)i, (float)i};
        Velocity vel = {1, 1, 1};

        YULDUZ_DataInfo data[] = {
            {g_position_type, 0, &pos},
            {g_velocity_type, 0, &vel}};
        YULDUZ_SDL_SortDataInfos(data, 2);

        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&src, (YULDUZ_Entity)i, data, &index);
    }

    Health          health     = {100, 100};
    YULDUZ_DataInfo new_data[] = {{g_health_type, 0, &health}};

    uint64_t start = SDL_GetPerformanceCounter();

    // Move all entities
    while (src.DenseCount > 0) {
        YULDUZ_ArchetypeIndex dst_idx;
        YULDUZ_MoveEntityInArchetype(&src, &dst, new_data, 1, 0, &dst_idx);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double   ms  = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  10,000 moves: %.2f ms (%.0f ops/sec)", ms, 10000.0 / (ms / 1000.0));

    YULDUZ_ReleaseArchetype(&src);
    YULDUZ_ReleaseArchetype(&dst);
}

void benchmark_query_performance(YULDUZ_TypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Query Performance");

    YULDUZ_Type            query[] = {g_position_type, g_velocity_type, g_health_type};
    YULDUZ_TypeDescription type_infos[3];
    YULDUZ_GetTypeDescriptionsInTypeRegistry(registry, query, type_infos, 3);

    YULDUZ_TypeInfo types[] = {
        {g_position_type, type_infos[0].Size, type_infos[0].Alignment},
        {g_velocity_type, type_infos[1].Size, type_infos[1].Alignment},
        {g_health_type, type_infos[2].Size, type_infos[2].Alignment}};
    YULDUZ_SDL_SortTypeDescriptions(types, 3);

    YULDUZ_Archetype archetype;
    YULDUZ_InitializeArchetype(&archetype, types, 3, nullptr, 0, 4);

    YULDUZ_Type            query_types[] = {g_velocity_type, g_position_type};
    YULDUZ_ComponentStore *stores[2];

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < 1000000; i++) {
        YULDUZ_QueryStoresInArchetype(&archetype, query_types, 2, nullptr, 0, stores);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double   ms  = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  1,000,000 queries: %.2f ms (%.0f ops/sec)", ms, 1000000.0 / (ms / 1000.0));

    YULDUZ_ReleaseArchetype(&archetype);
}

// ============================================================================
// ENTITY REGISTRY TESTS - Add these implementations at the end of Main.c
// ============================================================================

void test_entity_registry_basic(void) {
    YULDUZ_LOG_INFO("\n[TEST] Entity Registry - Basic Create/Destroy");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    // Create entities
    YULDUZ_Entity entities[10];
    for (uint32_t i = 0; i < 10; i++) {
        SDL_assert(YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]));
        SDL_assert(entities[i] == i);
    }

    SDL_assert(registry.NextEntity == 10);
    SDL_assert(registry.FreeListCount == 0);

    // Destroy some entities
    SDL_assert(YULDUZ_DestroyEntityInEntityRegistry(&registry, entities[5]));
    SDL_assert(YULDUZ_DestroyEntityInEntityRegistry(&registry, entities[3]));
    SDL_assert(YULDUZ_DestroyEntityInEntityRegistry(&registry, entities[8]));

    SDL_assert(registry.FreeListCount == 3);

    // Verify destroyed entities are not alive
    YULDUZ_EntityRecord records[3];
    YULDUZ_Entity       check_entities[] = {entities[3], entities[5], entities[8]};
    YULDUZ_GetEntityRecordsInEntityRegistry(&registry, check_entities, records, 3);

    SDL_assert(!records[0].Alive);
    SDL_assert(!records[1].Alive);
    SDL_assert(!records[2].Alive);

    YULDUZ_ReleaseEntityRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Entity Registry Basic PASSED");
}

void test_entity_registry_reuse(void) {
    YULDUZ_LOG_INFO("\n[TEST] Entity Registry - Entity Reuse");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    // Create and destroy entities to build up free list
    YULDUZ_Entity e0, e1, e2, e3, e4;
    YULDUZ_CreateEntityInEntityRegistry(&registry, &e0);
    YULDUZ_CreateEntityInEntityRegistry(&registry, &e1);
    YULDUZ_CreateEntityInEntityRegistry(&registry, &e2);
    YULDUZ_CreateEntityInEntityRegistry(&registry, &e3);
    YULDUZ_CreateEntityInEntityRegistry(&registry, &e4);

    YULDUZ_DestroyEntityInEntityRegistry(&registry, e1);
    YULDUZ_DestroyEntityInEntityRegistry(&registry, e3);
    YULDUZ_DestroyEntityInEntityRegistry(&registry, e0);

    SDL_assert(registry.FreeListCount == 3);

    // Create new entities - should reuse from free list (LIFO)
    YULDUZ_Entity new_e0, new_e1, new_e2;
    YULDUZ_CreateEntityInEntityRegistry(&registry, &new_e0);
    YULDUZ_CreateEntityInEntityRegistry(&registry, &new_e1);
    YULDUZ_CreateEntityInEntityRegistry(&registry, &new_e2);

    SDL_assert(new_e0 == 0);
    SDL_assert(new_e1 == 3);
    SDL_assert(new_e2 == 1);
    SDL_assert(registry.FreeListCount == 0);

    YULDUZ_Entity new_e3;
    YULDUZ_CreateEntityInEntityRegistry(&registry, &new_e3);
    SDL_assert(new_e3 == 5);

    YULDUZ_ReleaseEntityRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Entity Reuse PASSED");
}

void test_entity_registry_records(void) {
    YULDUZ_LOG_INFO("\n[TEST] Entity Registry - Get/Set Records");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    // Create entities
    YULDUZ_Entity entities[5];
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]);
    }

    // Set archetype info for entities
    YULDUZ_EntityRecord records_to_set[5];
    for (uint32_t i = 0; i < 5; i++) {
        records_to_set[i].ArchetypeType  = i * 10;
        records_to_set[i].ArchetypeIndex = i * 100;
        records_to_set[i].Alive          = true;
    }

    SDL_assert(YULDUZ_SetEntityRecordsInEntityRegistry(&registry, entities, records_to_set, 5));

    // Get and verify
    YULDUZ_EntityRecord retrieved[5];
    SDL_assert(YULDUZ_GetEntityRecordsInEntityRegistry(&registry, entities, retrieved, 5));

    for (uint32_t i = 0; i < 5; i++) {
        SDL_assert(retrieved[i].ArchetypeType == i * 10);
        SDL_assert(retrieved[i].ArchetypeIndex == i * 100);
        SDL_assert(retrieved[i].Alive);
    }

    YULDUZ_ReleaseEntityRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Get/Set Records PASSED");
}

void test_entity_registry_invalid_ops(void) {
    YULDUZ_LOG_INFO("\n[TEST] Entity Registry - Invalid Operations");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    YULDUZ_Entity e0, e1;
    YULDUZ_CreateEntityInEntityRegistry(&registry, &e0);
    YULDUZ_CreateEntityInEntityRegistry(&registry, &e1);

    // Try to destroy non-existent entity
    YULDUZ_Entity invalid = 9999;
    bool          result  = YULDUZ_DestroyEntityInEntityRegistry(&registry, invalid);
    SDL_assert(!result);

    // Try to destroy same entity twice
    YULDUZ_DestroyEntityInEntityRegistry(&registry, e0);
    result = YULDUZ_DestroyEntityInEntityRegistry(&registry, e0);
    SDL_assert(!result);

    // Try to get records for invalid entity
    YULDUZ_EntityRecord record;
    result = YULDUZ_GetEntityRecordsInEntityRegistry(&registry, &invalid, &record, 1);
    SDL_assert(!result);

    // Try to set records for invalid entity
    YULDUZ_EntityRecord set_record = {0, 0, true};
    result                         = YULDUZ_SetEntityRecordsInEntityRegistry(&registry, &invalid, &set_record, 1);
    SDL_assert(!result);

    YULDUZ_ReleaseEntityRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Invalid Operations PASSED");
}

void test_entity_registry_growth(void) {
    YULDUZ_LOG_INFO("\n[TEST] Entity Registry - Growth");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 4);

    uint32_t initial_capacity = registry.SparseCapacity;

    // Create enough entities to force growth
    YULDUZ_Entity entities[100];
    for (uint32_t i = 0; i < 100; i++) {
        SDL_assert(YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]));
    }

    SDL_assert(registry.SparseCapacity > initial_capacity);
    SDL_assert(registry.NextEntity == 100);

    // Verify all entities are valid
    YULDUZ_EntityRecord records[100];
    SDL_assert(YULDUZ_GetEntityRecordsInEntityRegistry(&registry, entities, records, 100));

    for (uint32_t i = 0; i < 100; i++) {
        SDL_assert(records[i].Alive);
    }

    YULDUZ_ReleaseEntityRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Registry Growth PASSED");
}

void test_entity_registry_free_list(void) {
    YULDUZ_LOG_INFO("\n[TEST] Entity Registry - Free List");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    const uint32_t count = 1000;
    YULDUZ_Entity  entities[count];

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]);
    }

    // Destroy every other entity
    for (uint32_t i = 0; i < count; i += 2) {
        YULDUZ_DestroyEntityInEntityRegistry(&registry, entities[i]);
    }

    SDL_assert(registry.FreeListCount == count / 2);

    // Recreate - should reuse
    YULDUZ_Entity new_entities[count / 2];
    for (uint32_t i = 0; i < count / 2; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &new_entities[i]);
    }

    SDL_assert(registry.FreeListCount == 0);

    // Verify all valid
    YULDUZ_EntityRecord records[count / 2];
    YULDUZ_GetEntityRecordsInEntityRegistry(&registry, new_entities, records, count / 2);

    for (uint32_t i = 0; i < count / 2; i++) {
        SDL_assert(records[i].Alive);
    }

    YULDUZ_ReleaseEntityRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Free List PASSED");
}

void test_entity_registry_archetype_assignment(void) {
    YULDUZ_LOG_INFO("\n[TEST] Entity Registry - Archetype Assignment");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    YULDUZ_Entity entities[10];
    for (uint32_t i = 0; i < 10; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]);
    }

    // Assign different archetypes
    YULDUZ_EntityRecord records[10];
    for (uint32_t i = 0; i < 10; i++) {
        records[i].ArchetypeType  = i % 3;
        records[i].ArchetypeIndex = i;
        records[i].Alive          = true;
    }
    YULDUZ_SetEntityRecordsInEntityRegistry(&registry, entities, records, 10);

    // Verify
    YULDUZ_EntityRecord retrieved[10];
    YULDUZ_GetEntityRecordsInEntityRegistry(&registry, entities, retrieved, 10);

    for (uint32_t i = 0; i < 10; i++) {
        SDL_assert(retrieved[i].ArchetypeType == i % 3);
        SDL_assert(retrieved[i].ArchetypeIndex == i);
    }

    // Change archetype (simulate moving)
    for (uint32_t i = 0; i < 10; i++) {
        records[i].ArchetypeType  = (i % 3) + 1;
        records[i].ArchetypeIndex = i + 100;
    }
    YULDUZ_SetEntityRecordsInEntityRegistry(&registry, entities, records, 10);

    YULDUZ_GetEntityRecordsInEntityRegistry(&registry, entities, retrieved, 10);
    for (uint32_t i = 0; i < 10; i++) {
        SDL_assert(retrieved[i].ArchetypeType == (i % 3) + 1);
        SDL_assert(retrieved[i].ArchetypeIndex == i + 100);
    }

    // Test invalid archetype values
    records[0].ArchetypeType  = YULDUZ_INVALID_ARCHETYPE_TYPE;
    records[0].ArchetypeIndex = YULDUZ_INVALID_ARCHETYPE_INDEX;
    YULDUZ_SetEntityRecordsInEntityRegistry(&registry, &entities[0], &records[0], 1);

    YULDUZ_GetEntityRecordsInEntityRegistry(&registry, &entities[0], &retrieved[0], 1);
    SDL_assert(retrieved[0].ArchetypeType == YULDUZ_INVALID_ARCHETYPE_TYPE);
    SDL_assert(retrieved[0].ArchetypeIndex == YULDUZ_INVALID_ARCHETYPE_INDEX);

    YULDUZ_ReleaseEntityRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Archetype Assignment PASSED");
}

void test_entity_registry_batch_ops(void) {
    YULDUZ_LOG_INFO("\n[TEST] Entity Registry - Batch Operations");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    const uint32_t batch_size = 50;
    YULDUZ_Entity  entities[batch_size];

    for (uint32_t i = 0; i < batch_size; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]);
    }

    // Batch set
    YULDUZ_EntityRecord records[batch_size];
    for (uint32_t i = 0; i < batch_size; i++) {
        records[i].ArchetypeType  = i % 10;
        records[i].ArchetypeIndex = i;
        records[i].Alive          = true;
    }
    SDL_assert(YULDUZ_SetEntityRecordsInEntityRegistry(&registry, entities, records, batch_size));

    // Batch get
    YULDUZ_EntityRecord retrieved[batch_size];
    SDL_assert(YULDUZ_GetEntityRecordsInEntityRegistry(&registry, entities, retrieved, batch_size));

    for (uint32_t i = 0; i < batch_size; i++) {
        SDL_assert(retrieved[i].ArchetypeType == i % 10);
        SDL_assert(retrieved[i].ArchetypeIndex == i);
        SDL_assert(retrieved[i].Alive);
    }

    YULDUZ_ReleaseEntityRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Batch Operations PASSED");
}

void stress_entity_registry_cycles(void) {
    YULDUZ_LOG_INFO("\n[STRESS] Entity Registry - Create/Destroy Cycles");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    const uint32_t cycles             = 100;
    const uint32_t entities_per_cycle = 100;

    for (uint32_t cycle = 0; cycle < cycles; cycle++) {
        YULDUZ_Entity entities[entities_per_cycle];

        for (uint32_t i = 0; i < entities_per_cycle; i++) {
            SDL_assert(YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]));
        }

        for (uint32_t i = 0; i < entities_per_cycle; i++) {
            SDL_assert(YULDUZ_DestroyEntityInEntityRegistry(&registry, entities[i]));
        }
    }

    SDL_assert(registry.FreeListCount > 0);

    YULDUZ_ReleaseEntityRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Cycles PASSED (%u cycles)", cycles);
}

void stress_entity_registry_random(void) {
    YULDUZ_LOG_INFO("\n[STRESS] Entity Registry - Random Operations");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    const uint32_t max_entities = 500;
    YULDUZ_Entity  entities[max_entities];
    bool           entity_alive[max_entities] = {};
    SDL_memset(entities, 0x00, sizeof(bool) * SDL_arraysize(entities));
    uint32_t alive_count = 0;

    srand(12345);

    for (uint32_t op = 0; op < 5000; op++) {
        int operation = rand() % 3;

        if (operation == 0 && alive_count < max_entities) {
            YULDUZ_Entity new_entity;
            if (YULDUZ_CreateEntityInEntityRegistry(&registry, &new_entity)) {
                entities[alive_count]     = new_entity;
                entity_alive[alive_count] = true;
                alive_count++;
            }
        } else if (operation == 1 && alive_count > 0) {
            uint32_t idx = rand() % alive_count;
            if (entity_alive[idx]) {
                YULDUZ_DestroyEntityInEntityRegistry(&registry, entities[idx]);
                entity_alive[idx] = false;

                entities[idx]     = entities[alive_count - 1];
                entity_alive[idx] = entity_alive[alive_count - 1];
                alive_count--;
            }
        } else if (operation == 2 && alive_count > 0) {
            uint32_t idx = rand() % alive_count;
            if (entity_alive[idx]) {
                YULDUZ_EntityRecord record = {
                    .ArchetypeType  = rand() % 20,
                    .ArchetypeIndex = rand() % 100,
                    .Alive          = true};
                YULDUZ_SetEntityRecordsInEntityRegistry(&registry, &entities[idx], &record, 1);
            }
        }
    }

    YULDUZ_ReleaseEntityRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Random Operations PASSED (5000 ops)");
}

void benchmark_entity_creation(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Entity Creation");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 100000);

    const uint32_t count    = 100000;
    YULDUZ_Entity *entities = SDL_malloc(count * sizeof(YULDUZ_Entity));

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double   ms  = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  100,000 creates: %.2f ms (%.0f ops/sec)", ms, 100000.0 / (ms / 1000.0));

    SDL_free(entities);
    YULDUZ_ReleaseEntityRegistry(&registry);
}

void benchmark_entity_destruction(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Entity Destruction");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 100000);

    const uint32_t count    = 100000;
    YULDUZ_Entity *entities = SDL_malloc(count * sizeof(YULDUZ_Entity));

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]);
    }

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_DestroyEntityInEntityRegistry(&registry, entities[i]);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double   ms  = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  100,000 destroys: %.2f ms (%.0f ops/sec)", ms, 100000.0 / (ms / 1000.0));

    SDL_free(entities);
    YULDUZ_ReleaseEntityRegistry(&registry);
}

void benchmark_entity_reuse(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Entity Reuse");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 100000);

    const uint32_t count    = 50000;
    YULDUZ_Entity *entities = SDL_malloc(count * sizeof(YULDUZ_Entity));

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]);
    }
    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_DestroyEntityInEntityRegistry(&registry, entities[i]);
    }

    SDL_assert(registry.FreeListCount == count);

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double   ms  = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  50,000 creates (reused): %.2f ms (%.0f ops/sec)", ms, 50000.0 / (ms / 1000.0));

    SDL_free(entities);
    YULDUZ_ReleaseEntityRegistry(&registry);
}

void benchmark_entity_get_records(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Get Entity Records");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 10000);

    const uint32_t       entity_count = 10000;
    YULDUZ_Entity       *entities     = SDL_malloc(entity_count * sizeof(YULDUZ_Entity));
    YULDUZ_EntityRecord *records      = SDL_malloc(entity_count * sizeof(YULDUZ_EntityRecord));

    for (uint32_t i = 0; i < entity_count; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]);
    }

    const uint32_t iterations = 10000;

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t iter = 0; iter < iterations; iter++) {
        YULDUZ_GetEntityRecordsInEntityRegistry(&registry, entities, records, entity_count);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double   ms  = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  100,000,000 gets: %.2f ms (%.0f ops/sec)", ms, 100000000.0 / (ms / 1000.0));

    SDL_free(entities);
    SDL_free(records);
    YULDUZ_ReleaseEntityRegistry(&registry);
}

void benchmark_entity_set_records(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Set Entity Records");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 10000);

    const uint32_t       entity_count = 10000;
    YULDUZ_Entity       *entities     = SDL_malloc(entity_count * sizeof(YULDUZ_Entity));
    YULDUZ_EntityRecord *records      = SDL_malloc(entity_count * sizeof(YULDUZ_EntityRecord));

    for (uint32_t i = 0; i < entity_count; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]);
        records[i].ArchetypeType  = i % 100;
        records[i].ArchetypeIndex = i;
        records[i].Alive          = true;
    }

    const uint32_t iterations = 10000;

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t iter = 0; iter < iterations; iter++) {
        YULDUZ_SetEntityRecordsInEntityRegistry(&registry, entities, records, entity_count);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double   ms  = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  100,000,000 sets: %.2f ms (%.0f ops/sec)", ms, 100000000.0 / (ms / 1000.0));

    SDL_free(entities);
    SDL_free(records);
    YULDUZ_ReleaseEntityRegistry(&registry);
}

void benchmark_entity_mixed_ops(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Mixed Entity Operations");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 50000);

    const uint32_t      iterations = 50000;
    YULDUZ_Entity       entity;
    YULDUZ_EntityRecord record;

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < iterations; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entity);

        record.ArchetypeType  = i % 20;
        record.ArchetypeIndex = i;
        record.Alive          = true;
        YULDUZ_SetEntityRecordsInEntityRegistry(&registry, &entity, &record, 1);

        YULDUZ_GetEntityRecordsInEntityRegistry(&registry, &entity, &record, 1);

        if (i % 2 == 0) {
            YULDUZ_DestroyEntityInEntityRegistry(&registry, entity);
        }
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double   ms  = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  200,000 mixed ops: %.2f ms (%.0f ops/sec)", ms, 200000.0 / (ms / 1000.0));

    YULDUZ_ReleaseEntityRegistry(&registry);
}


// Setup function for ECS tests (call in main before tests)
void setup_ecs_test_types(YULDUZ_ECSRegistry *registry) {
    YULDUZ_LOG_INFO("\n[SETUP] Registering ECS Test Types");

    YULDUZ_TypeDescription types[] = {
        {"Position", sizeof(Position), alignof(Position)},
        {"Velocity", sizeof(Velocity), alignof(Velocity)},
        {"Health", sizeof(Health), alignof(Health)},
        {"Damage", sizeof(Damage), alignof(Damage)},
        {"Name", sizeof(Name), alignof(Name)},
        {"TagPlayer", 0, 0},  // Zero-size tag
        {"TagEnemy", 0, 0},
        {"TagDead", 0, 0}
    };

    YULDUZ_Type type_ids[8];

    for (uint32_t i = 0; i < 8; i++) {
        YULDUZ_RegisterTypeInECSRegistry(registry, types[i], &type_ids[i]);
    }

    g_position_type = type_ids[0];
    g_velocity_type = type_ids[1];
    g_health_type = type_ids[2];
    g_damage_type = type_ids[3];
    g_name_type = type_ids[4];
    g_tag_player = type_ids[5];
    g_tag_enemy = type_ids[6];
    g_tag_dead = type_ids[7];

    YULDUZ_LOG_INFO("  ✓ Registered 8 types (5 components + 3 tags)");
}

// ============================================================================
// CORRECTNESS TESTS
// ============================================================================

void test_ecs_null_archetype(void) {
    YULDUZ_LOG_INFO("\n[TEST] Null Archetype");

    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialEntityCapacity = 16,
        .InitialComponentCapacity = 16,
        .InitialArchetypeCapacity = 16
    };

    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    // Create entity - should be in null archetype
    YULDUZ_Entity entity;
    SDL_assert(YULDUZ_CreateEntityInECSRegistry(&registry, &entity));

    // Verify entity is in archetype 0 (null archetype)
    YULDUZ_EntityRecord record;
    SDL_assert(YULDUZ_GetEntityRecordsInEntityRegistry(&registry.EntityRegistry, &entity, &record, 1));
    SDL_assert(record.ArchetypeType == registry.NullArchetypeType);

    // Null archetype should have 0 components and 0 tags
    YULDUZ_Archetype *null_arch = &registry.Dense[registry.NullArchetypeType];
    SDL_assert(null_arch->StoreCount == 0);
    SDL_assert(null_arch->TagCount == 0);
    SDL_assert(null_arch->DenseCount == 1);  // One entity

    YULDUZ_ReleaseECSRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Null Archetype PASSED");
}

void test_ecs_add_remove_tags(void) {
    YULDUZ_LOG_INFO("\n[TEST] Add/Remove Tags");

    YULDUZ_ECSRegistryInitializeInfo info = {16, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    // Add tag
    SDL_assert(YULDUZ_AddTagWithTypeInECSRegistry(&registry, entity, g_tag_player));
    SDL_assert(YULDUZ_HasTagWithTypeInECSRegistry(&registry, entity, g_tag_player));

    // Add another tag
    SDL_assert(YULDUZ_AddTagWithTypeInECSRegistry(&registry, entity, g_tag_enemy));
    SDL_assert(YULDUZ_HasTagWithTypeInECSRegistry(&registry, entity, g_tag_player));
    SDL_assert(YULDUZ_HasTagWithTypeInECSRegistry(&registry, entity, g_tag_enemy));

    // Remove tag
    SDL_assert(YULDUZ_RemoveTagWithTypeInECSRegistry(&registry, entity, g_tag_player));
    SDL_assert(!YULDUZ_HasTagWithTypeInECSRegistry(&registry, entity, g_tag_player));
    SDL_assert(YULDUZ_HasTagWithTypeInECSRegistry(&registry, entity, g_tag_enemy));

    // Add tag again (idempotent)
    SDL_assert(YULDUZ_AddTagWithTypeInECSRegistry(&registry, entity, g_tag_player));
    SDL_assert(YULDUZ_HasTagWithTypeInECSRegistry(&registry, entity, g_tag_player));

    // Remove non-existent tag (should succeed)
    SDL_assert(YULDUZ_RemoveTagWithTypeInECSRegistry(&registry, entity, g_tag_dead));

    YULDUZ_ReleaseECSRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Add/Remove Tags PASSED");
}

void test_ecs_add_remove_components(void) {
    YULDUZ_LOG_INFO("\n[TEST] Add/Remove Components");

    YULDUZ_ECSRegistryInitializeInfo info = {16, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    // Add Position component
    Position pos = {10.0f, 20.0f, 30.0f};
    SDL_assert(YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos));

    // Verify component data
    Position retrieved_pos;
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &retrieved_pos));
    SDL_assert(retrieved_pos.x == 10.0f && retrieved_pos.y == 20.0f && retrieved_pos.z == 30.0f);

    // Add Velocity component
    Velocity vel = {1.0f, 2.0f, 3.0f};
    SDL_assert(YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_velocity_type, &vel));

    // Verify both components exist
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &retrieved_pos));
    SDL_assert(retrieved_pos.x == 10.0f);  // Still intact after adding velocity

    // Remove Position
    SDL_assert(YULDUZ_RemoveComponentWithTypeInECSRegistry(&registry, entity, g_position_type));

    // Position should be gone, Velocity should remain
    SDL_assert(!YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &retrieved_pos));

    Velocity retrieved_vel;
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_velocity_type, &retrieved_vel));
    SDL_assert(retrieved_vel.x == 1.0f);

    YULDUZ_ReleaseECSRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Add/Remove Components PASSED");
}

void test_ecs_component_migration(void) {
    YULDUZ_LOG_INFO("\n[TEST] Component Migration");

    YULDUZ_ECSRegistryInitializeInfo info = {16, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    // Start: []
    // Add Position: [Position]
    Position pos = {100.0f, 200.0f, 300.0f};
    YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos);

    // Add Velocity: [Position, Velocity]
    Velocity vel = {5.0f, 10.0f, 15.0f};
    YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_velocity_type, &vel);

    // Add Health: [Position, Velocity, Health]
    Health health = {100.0f, 100.0f};
    YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_health_type, &health);

    // Verify all components preserved
    Position pos2;
    Velocity vel2;
    Health health2;

    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos2));
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_velocity_type, &vel2));
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_health_type, &health2));

    SDL_assert(pos2.x == 100.0f && pos2.y == 200.0f && pos2.z == 300.0f);
    SDL_assert(vel2.x == 5.0f && vel2.y == 10.0f && vel2.z == 15.0f);
    SDL_assert(health2.current == 100.0f && health2.max == 100.0f);

    // Remove middle component: [Position, Health]
    YULDUZ_RemoveComponentWithTypeInECSRegistry(&registry, entity, g_velocity_type);

    // Verify remaining components
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos2));
    SDL_assert(!YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_velocity_type, &vel2));
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_health_type, &health2));

    SDL_assert(pos2.x == 100.0f);
    SDL_assert(health2.current == 100.0f);

    YULDUZ_ReleaseECSRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Component Migration PASSED");
}

void test_ecs_tag_component_combinations(void) {
    YULDUZ_LOG_INFO("\n[TEST] Tag + Component Combinations");

    YULDUZ_ECSRegistryInitializeInfo info = {16, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    // Add component then tag
    Position pos = {1.0f, 2.0f, 3.0f};
    YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos);
    YULDUZ_AddTagWithTypeInECSRegistry(&registry, entity, g_tag_player);

    // Verify both exist
    Position pos2;
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos2));
    SDL_assert(YULDUZ_HasTagWithTypeInECSRegistry(&registry, entity, g_tag_player));
    SDL_assert(pos2.x == 1.0f);

    // Add another tag
    YULDUZ_AddTagWithTypeInECSRegistry(&registry, entity, g_tag_enemy);

    // Verify component still intact
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos2));
    SDL_assert(pos2.x == 1.0f);
    SDL_assert(YULDUZ_HasTagWithTypeInECSRegistry(&registry, entity, g_tag_player));
    SDL_assert(YULDUZ_HasTagWithTypeInECSRegistry(&registry, entity, g_tag_enemy));

    // Remove tag, component should remain
    YULDUZ_RemoveTagWithTypeInECSRegistry(&registry, entity, g_tag_player);
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos2));
    SDL_assert(!YULDUZ_HasTagWithTypeInECSRegistry(&registry, entity, g_tag_player));
    SDL_assert(YULDUZ_HasTagWithTypeInECSRegistry(&registry, entity, g_tag_enemy));

    YULDUZ_ReleaseECSRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Tag + Component Combinations PASSED");
}

void test_ecs_archetype_reuse(void) {
    YULDUZ_LOG_INFO("\n[TEST] Archetype Reuse");

    YULDUZ_ECSRegistryInitializeInfo info = {16, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    // Create two entities with same components
    YULDUZ_Entity e1, e2;
    YULDUZ_CreateEntityInECSRegistry(&registry, &e1);
    YULDUZ_CreateEntityInECSRegistry(&registry, &e2);

    Position pos1 = {1.0f, 2.0f, 3.0f};
    Position pos2 = {4.0f, 5.0f, 6.0f};

    YULDUZ_AddComponentWithTypeInECSRegistry(&registry, e1, g_position_type, &pos1);
    YULDUZ_AddComponentWithTypeInECSRegistry(&registry, e2, g_position_type, &pos2);

    // Both should be in the same archetype
    YULDUZ_EntityRecord r1, r2;
    YULDUZ_GetEntityRecordsInEntityRegistry(&registry.EntityRegistry, &e1, &r1, 1);
    YULDUZ_GetEntityRecordsInEntityRegistry(&registry.EntityRegistry, &e2, &r2, 1);

    SDL_assert(r1.ArchetypeType == r2.ArchetypeType);

    // Archetype should have 2 entities
    YULDUZ_Archetype *arch = &registry.Dense[r1.ArchetypeType];
    SDL_assert(arch->DenseCount == 2);

    // Verify both entities' data
    Position retrieved1, retrieved2;
    YULDUZ_GetComponentWithTypeInECSRegistry(&registry, e1, g_position_type, &retrieved1);
    YULDUZ_GetComponentWithTypeInECSRegistry(&registry, e2, g_position_type, &retrieved2);

    SDL_assert(retrieved1.x == 1.0f);
    SDL_assert(retrieved2.x == 4.0f);

    uint32_t initial_archetype_count = registry.DenseCount;

    // Create third entity with same components - should reuse archetype
    YULDUZ_Entity e3;
    YULDUZ_CreateEntityInECSRegistry(&registry, &e3);
    Position pos3 = {7.0f, 8.0f, 9.0f};
    YULDUZ_AddComponentWithTypeInECSRegistry(&registry, e3, g_position_type, &pos3);

    // No new archetype should be created
    SDL_assert(registry.DenseCount == initial_archetype_count);
    SDL_assert(arch->DenseCount == 3);

    YULDUZ_ReleaseECSRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Archetype Reuse PASSED");
}

void test_ecs_set_get_components(void) {
    YULDUZ_LOG_INFO("\n[TEST] Set/Get Components");

    YULDUZ_ECSRegistryInitializeInfo info = {16, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    Position pos = {10.0f, 20.0f, 30.0f};
    YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos);

    // Modify component
    pos.x = 100.0f;
    pos.y = 200.0f;
    pos.z = 300.0f;
    SDL_assert(YULDUZ_SetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos));

    // Verify modification
    Position retrieved;
    YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &retrieved);
    SDL_assert(retrieved.x == 100.0f && retrieved.y == 200.0f && retrieved.z == 300.0f);

    // Try to set non-existent component (should fail)
    Velocity vel = {1, 2, 3};
    SDL_assert(!YULDUZ_SetComponentWithTypeInECSRegistry(&registry, entity, g_velocity_type, &vel));

    YULDUZ_ReleaseECSRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Set/Get Components PASSED");
}

void test_ecs_invalid_operations(void) {
    YULDUZ_LOG_INFO("\n[TEST] Invalid Operations");

    YULDUZ_ECSRegistryInitializeInfo info = {16, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    YULDUZ_Entity invalid_entity = 99999;

    // Operations on invalid entity should fail gracefully
    Position pos = {1, 2, 3};
    SDL_assert(!YULDUZ_AddComponentWithTypeInECSRegistry(&registry, invalid_entity, g_position_type, &pos));
    SDL_assert(!YULDUZ_GetComponentWithTypeInECSRegistry(&registry, invalid_entity, g_position_type, &pos));
    SDL_assert(!YULDUZ_SetComponentWithTypeInECSRegistry(&registry, invalid_entity, g_position_type, &pos));
    SDL_assert(!YULDUZ_AddTagWithTypeInECSRegistry(&registry, invalid_entity, g_tag_player));
    SDL_assert(!YULDUZ_HasTagWithTypeInECSRegistry(&registry, invalid_entity, g_tag_player));

    // Create valid entity
    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    // Get component that doesn't exist
    SDL_assert(!YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos));

    // Set component that doesn't exist
    SDL_assert(!YULDUZ_SetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos));

    // Remove component that doesn't exist (should succeed - idempotent)
    SDL_assert(YULDUZ_RemoveComponentWithTypeInECSRegistry(&registry, entity, g_position_type));

    YULDUZ_ReleaseECSRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Invalid Operations PASSED");
}

void test_ecs_edge_cases(void) {
    YULDUZ_LOG_INFO("\n[TEST] Edge Cases");

    YULDUZ_ECSRegistryInitializeInfo info = {16, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    // Add component with nullptr data (should zero-initialize)
    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    SDL_assert(YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_position_type, nullptr));

    Position pos;
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos));
    SDL_assert(pos.x == 0.0f && pos.y == 0.0f && pos.z == 0.0f);

    // Add same component twice (should be idempotent)
    Position new_pos = {10, 20, 30};
    SDL_assert(YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &new_pos));

    // Should still have original data (first add wins)
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos));
    SDL_assert(pos.x == 0.0f);  // Still zero from first add

    // Add tag twice (should be idempotent)
    SDL_assert(YULDUZ_AddTagWithTypeInECSRegistry(&registry, entity, g_tag_player));
    SDL_assert(YULDUZ_AddTagWithTypeInECSRegistry(&registry, entity, g_tag_player));
    SDL_assert(YULDUZ_HasTagWithTypeInECSRegistry(&registry, entity, g_tag_player));

    YULDUZ_ReleaseECSRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Edge Cases PASSED");
}

void test_ecs_entity_destruction(void) {
    YULDUZ_LOG_INFO("\n[TEST] Entity Destruction");

    YULDUZ_ECSRegistryInitializeInfo info = {16, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    // Create entities with components
    YULDUZ_Entity entities[5];
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &entities[i]);
        Position pos = {(float)i, (float)i, (float)i};
        YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entities[i], g_position_type, &pos);
    }

    // Get archetype
    YULDUZ_EntityRecord record;
    YULDUZ_GetEntityRecordsInEntityRegistry(&registry.EntityRegistry, &entities[0], &record, 1);
    YULDUZ_Archetype *arch = &registry.Dense[record.ArchetypeType];

    SDL_assert(arch->DenseCount == 5);

    // Destroy middle entity
    SDL_assert(YULDUZ_DestroyEntityInECSRegistry(&registry, entities[2]));
    SDL_assert(arch->DenseCount == 4);

    // Verify destroyed entity is gone
    Position pos;
    SDL_assert(!YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entities[2], g_position_type, &pos));

    // Other entities should still exist
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entities[0], g_position_type, &pos));
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entities[1], g_position_type, &pos));
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entities[3], g_position_type, &pos));
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entities[4], g_position_type, &pos));

    YULDUZ_ReleaseECSRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Entity Destruction PASSED");
}

void test_ecs_archetype_growth(void) {
    YULDUZ_LOG_INFO("\n[TEST] Archetype Growth");

    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialEntityCapacity = 4,
        .InitialComponentCapacity = 4,
        .InitialArchetypeCapacity = 2  // Small initial capacity
    };

    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    uint32_t initial_capacity = registry.DenseCapacity;

    // Create many different archetypes to force growth
    YULDUZ_Entity entities[10];
    for (uint32_t i = 0; i < 10; i++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &entities[i]);

        // Each entity gets a unique combination of components
        if (i % 2 == 0) {
            Position pos = {(float)i, 0, 0};
            YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entities[i], g_position_type, &pos);
        }
        if (i % 3 == 0) {
            Velocity vel = {1, 1, 1};
            YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entities[i], g_velocity_type, &vel);
        }
        if (i % 5 == 0) {
            Health health = {100, 100};
            YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entities[i], g_health_type, &health);
        }
    }

    // Should have grown
    SDL_assert(registry.DenseCapacity > initial_capacity);
    SDL_assert(registry.DenseCount > 1);  // Multiple archetypes created

    // Verify all entities still work
    for (uint32_t i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            Position pos;
            SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entities[i], g_position_type, &pos));
            SDL_assert(pos.x == (float)i);
        }
    }

    YULDUZ_ReleaseECSRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Archetype Growth PASSED");
}

void test_ecs_multiple_migrations(void) {
    YULDUZ_LOG_INFO("\n[TEST] Multiple Migrations");

    YULDUZ_ECSRegistryInitializeInfo info = {16, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    // Migrate through multiple archetypes: [] -> [Pos] -> [Pos, Vel] -> [Pos, Vel, Health] -> [Vel, Health] -> [Health] -> []

    Position pos = {1, 2, 3};
    YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos);

    Velocity vel = {4, 5, 6};
    YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_velocity_type, &vel);

    Health health = {100, 100};
    YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_health_type, &health);

    // Verify all three
    Position p; Velocity v; Health h;
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &p));
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_velocity_type, &v));
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_health_type, &h));
    SDL_assert(p.x == 1.0f && v.x == 4.0f && h.current == 100.0f);

    // Remove Position
    YULDUZ_RemoveComponentWithTypeInECSRegistry(&registry, entity, g_position_type);
    SDL_assert(!YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &p));
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_velocity_type, &v));
    SDL_assert(v.x == 4.0f);  // Preserved

    // Remove Velocity
    YULDUZ_RemoveComponentWithTypeInECSRegistry(&registry, entity, g_velocity_type);
    SDL_assert(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_health_type, &h));
    SDL_assert(h.current == 100.0f);  // Preserved

    // Remove Health - back to null archetype
    YULDUZ_RemoveComponentWithTypeInECSRegistry(&registry, entity, g_health_type);

    YULDUZ_EntityRecord record;
    YULDUZ_GetEntityRecordsInEntityRegistry(&registry.EntityRegistry, &entity, &record, 1);
    SDL_assert(record.ArchetypeType == registry.NullArchetypeType);

    YULDUZ_ReleaseECSRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Multiple Migrations PASSED");
}

void test_ecs_zero_size_components(void) {
    YULDUZ_LOG_INFO("\n[TEST] Zero-Size Components (Tags)");

    YULDUZ_ECSRegistryInitializeInfo info = {16, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    // Add multiple tags
    YULDUZ_AddTagWithTypeInECSRegistry(&registry, entity, g_tag_player);
    YULDUZ_AddTagWithTypeInECSRegistry(&registry, entity, g_tag_enemy);
    YULDUZ_AddTagWithTypeInECSRegistry(&registry, entity, g_tag_dead);

    SDL_assert(YULDUZ_HasTagWithTypeInECSRegistry(&registry, entity, g_tag_player));
    SDL_assert(YULDUZ_HasTagWithTypeInECSRegistry(&registry, entity, g_tag_enemy));
    SDL_assert(YULDUZ_HasTagWithTypeInECSRegistry(&registry, entity, g_tag_dead));

    // Entity should be in archetype with 0 components, 3 tags
    YULDUZ_EntityRecord record;
    YULDUZ_GetEntityRecordsInEntityRegistry(&registry.EntityRegistry, &entity, &record, 1);
    YULDUZ_Archetype *arch = &registry.Dense[record.ArchetypeType];

    SDL_assert(arch->StoreCount == 0);
    SDL_assert(arch->TagCount == 3);

    YULDUZ_ReleaseECSRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Zero-Size Components PASSED");
}

// ============================================================================
// STRESS TESTS
// ============================================================================

void stress_ecs_create_destroy_cycles(void) {
    YULDUZ_LOG_INFO("\n[STRESS] Create/Destroy Cycles");

    YULDUZ_ECSRegistryInitializeInfo info = {1000, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    const uint32_t cycles = 100;
    const uint32_t entities_per_cycle = 100;

    for (uint32_t cycle = 0; cycle < cycles; cycle++) {
        YULDUZ_Entity entities[100];

        // Create
        for (uint32_t i = 0; i < entities_per_cycle; i++) {
            YULDUZ_CreateEntityInECSRegistry(&registry, &entities[i]);
            Position pos = {(float)i, (float)cycle, 0};
            YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entities[i], g_position_type, &pos);
        }

        // Destroy
        for (uint32_t i = 0; i < entities_per_cycle; i++) {
            YULDUZ_DestroyEntityInECSRegistry(&registry, entities[i]);
        }
    }

    // Free list should have all entities
    SDL_assert(registry.EntityRegistry.FreeListCount > 0);

    YULDUZ_ReleaseECSRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Create/Destroy Cycles PASSED (10,000 entities)");
}

void stress_ecs_component_churn(void) {
    YULDUZ_LOG_INFO("\n[STRESS] Component Churn");

    YULDUZ_ECSRegistryInitializeInfo info = {100, 16, 32};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    // Create 50 entities
    YULDUZ_Entity entities[50];
    for (uint32_t i = 0; i < 50; i++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &entities[i]);
    }

    // Randomly add/remove components 1000 times
    for (uint32_t i = 0; i < 1000; i++) {
        uint32_t entity_idx = i % 50;
        YULDUZ_Entity entity = entities[entity_idx];

        uint32_t operation = i % 6;

        switch (operation) {
            case 0: {
                Position pos = {(float)i, 0, 0};
                YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos);
                break;
            }
            case 1:
                YULDUZ_RemoveComponentWithTypeInECSRegistry(&registry, entity, g_position_type);
                break;
            case 2: {
                Velocity vel = {1, 1, 1};
                YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_velocity_type, &vel);
                break;
            }
            case 3:
                YULDUZ_RemoveComponentWithTypeInECSRegistry(&registry, entity, g_velocity_type);
                break;
            case 4: {
                Health health = {100, 100};
                YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_health_type, &health);
                break;
            }
            case 5:
                YULDUZ_RemoveComponentWithTypeInECSRegistry(&registry, entity, g_health_type);
                break;
        }
    }

    // All entities should still be valid
    for (uint32_t i = 0; i < 50; i++) {
        YULDUZ_EntityRecord record;
        SDL_assert(YULDUZ_GetEntityRecordsInEntityRegistry(&registry.EntityRegistry, &entities[i], &record, 1));
    }

    YULDUZ_ReleaseECSRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Component Churn PASSED (1,000 operations)");
}

void stress_ecs_archetype_explosion(void) {
    YULDUZ_LOG_INFO("\n[STRESS] Archetype Explosion");

    YULDUZ_ECSRegistryInitializeInfo info = {200, 16, 64};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    // Create entities with all possible combinations of 3 components
    // 2^3 = 8 combinations (including none)
    YULDUZ_Entity entities[64];
    uint32_t entity_idx = 0;

    for (uint32_t mask = 0; mask < 8; mask++) {
        for (uint32_t rep = 0; rep < 8; rep++) {
            YULDUZ_CreateEntityInECSRegistry(&registry, &entities[entity_idx]);

            if (mask & 1) {
                Position pos = {(float)entity_idx, 0, 0};
                YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entities[entity_idx], g_position_type, &pos);
            }
            if (mask & 2) {
                Velocity vel = {1, 1, 1};
                YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entities[entity_idx], g_velocity_type, &vel);
            }
            if (mask & 4) {
                Health health = {100, 100};
                YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entities[entity_idx], g_health_type, &health);
            }

            entity_idx++;
        }
    }

    // Should have created 8 archetypes (one for each combination)
    SDL_assert(registry.DenseCount == 8);

    // Verify all entities
    for (uint32_t i = 0; i < 64; i++) {
        YULDUZ_EntityRecord record;
        SDL_assert(YULDUZ_GetEntityRecordsInEntityRegistry(&registry.EntityRegistry, &entities[i], &record, 1));
    }

    YULDUZ_ReleaseECSRegistry(&registry);
    YULDUZ_LOG_INFO("  ✓ Archetype Explosion PASSED (8 archetypes, 64 entities)");
}

// ============================================================================
// BENCHMARKS
// ============================================================================

void benchmark_ecs_entity_spawn(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Entity Spawn");

    YULDUZ_ECSRegistryInitializeInfo info = {100000, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    const uint32_t count = 50000;
    YULDUZ_Entity *entities = SDL_malloc(count * sizeof(YULDUZ_Entity));

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &entities[i]);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  50,000 spawns: %.2f ms (%.0f ops/sec)", ms, 50000.0 / (ms / 1000.0));

    SDL_free(entities);
    YULDUZ_ReleaseECSRegistry(&registry);
}

void benchmark_ecs_add_component(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Add Component");

    YULDUZ_ECSRegistryInitializeInfo info = {50000, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    const uint32_t count = 50000;
    YULDUZ_Entity *entities = SDL_malloc(count * sizeof(YULDUZ_Entity));

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &entities[i]);
    }

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < count; i++) {
        Position pos = {(float)i, (float)i, (float)i};
        YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entities[i], g_position_type, &pos);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  50,000 component adds: %.2f ms (%.0f ops/sec)", ms, 50000.0 / (ms / 1000.0));

    SDL_free(entities);
    YULDUZ_ReleaseECSRegistry(&registry);
}

void benchmark_ecs_remove_component(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Remove Component");

    YULDUZ_ECSRegistryInitializeInfo info = {50000, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    const uint32_t count = 50000;
    YULDUZ_Entity *entities = SDL_malloc(count * sizeof(YULDUZ_Entity));

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &entities[i]);
        Position pos = {(float)i, 0, 0};
        YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entities[i], g_position_type, &pos);
    }

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_RemoveComponentWithTypeInECSRegistry(&registry, entities[i], g_position_type);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  50,000 component removes: %.2f ms (%.0f ops/sec)", ms, 50000.0 / (ms / 1000.0));

    SDL_free(entities);
    YULDUZ_ReleaseECSRegistry(&registry);
}

void benchmark_ecs_add_tag(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Add Tag");

    YULDUZ_ECSRegistryInitializeInfo info = {50000, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    const uint32_t count = 50000;
    YULDUZ_Entity *entities = SDL_malloc(count * sizeof(YULDUZ_Entity));

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &entities[i]);
    }

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_AddTagWithTypeInECSRegistry(&registry, entities[i], g_tag_player);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  50,000 tag adds: %.2f ms (%.0f ops/sec)", ms, 50000.0 / (ms / 1000.0));

    SDL_free(entities);
    YULDUZ_ReleaseECSRegistry(&registry);
}

void benchmark_ecs_get_component(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Get Component");

    YULDUZ_ECSRegistryInitializeInfo info = {10000, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    const uint32_t entity_count = 10000;
    YULDUZ_Entity *entities = SDL_malloc(entity_count * sizeof(YULDUZ_Entity));

    for (uint32_t i = 0; i < entity_count; i++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &entities[i]);
        Position pos = {(float)i, (float)i, (float)i};
        YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entities[i], g_position_type, &pos);
    }

    const uint32_t iterations = 10000;
    Position pos;

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t iter = 0; iter < iterations; iter++) {
        for (uint32_t i = 0; i < entity_count; i++) {
            YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entities[i], g_position_type, &pos);
        }
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  100,000,000 gets: %.2f ms (%.0f ops/sec)", ms, 100000000.0 / (ms / 1000.0));

    SDL_free(entities);
    YULDUZ_ReleaseECSRegistry(&registry);
}

void benchmark_ecs_set_component(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Set Component");

    YULDUZ_ECSRegistryInitializeInfo info = {10000, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    const uint32_t entity_count = 10000;
    YULDUZ_Entity *entities = SDL_malloc(entity_count * sizeof(YULDUZ_Entity));

    for (uint32_t i = 0; i < entity_count; i++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &entities[i]);
        Position pos = {0, 0, 0};
        YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entities[i], g_position_type, &pos);
    }

    const uint32_t iterations = 10000;

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t iter = 0; iter < iterations; iter++) {
        for (uint32_t i = 0; i < entity_count; i++) {
            Position pos = {(float)iter, (float)i, 0};
            YULDUZ_SetComponentWithTypeInECSRegistry(&registry, entities[i], g_position_type, &pos);
        }
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  100,000,000 sets: %.2f ms (%.0f ops/sec)", ms, 100000000.0 / (ms / 1000.0));

    SDL_free(entities);
    YULDUZ_ReleaseECSRegistry(&registry);
}

void benchmark_ecs_full_workflow(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Full Workflow");

    YULDUZ_ECSRegistryInitializeInfo info = {10000, 16, 16};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    const uint32_t count = 10000;

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < count; i++) {
        // Spawn
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

        // Add components
        Position pos = {(float)i, (float)i, (float)i};
        YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &pos);

        Velocity vel = {1, 2, 3};
        YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, g_velocity_type, &vel);

        // Add tag
        YULDUZ_AddTagWithTypeInECSRegistry(&registry, entity, g_tag_player);

        // Read
        Position p;
        YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &p);

        // Modify
        p.x += 10.0f;
        YULDUZ_SetComponentWithTypeInECSRegistry(&registry, entity, g_position_type, &p);

        // Remove component
        YULDUZ_RemoveComponentWithTypeInECSRegistry(&registry, entity, g_velocity_type);

        // Destroy
        if (i % 2 == 0) {
            YULDUZ_DestroyEntityInECSRegistry(&registry, entity);
        }
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  10,000 full workflows: %.2f ms (%.0f ops/sec)", ms, 10000.0 / (ms / 1000.0));

    YULDUZ_ReleaseECSRegistry(&registry);
}

void benchmark_ecs_archetype_lookup(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Archetype Lookup");

    YULDUZ_ECSRegistryInitializeInfo info = {1000, 16, 32};
    YULDUZ_ECSRegistry registry;
    YULDUZ_InitializeECSRegistry(&registry, &info);
    setup_ecs_test_types(&registry);

    // Create 16 different archetypes
    YULDUZ_Entity entities[16];
    for (uint32_t mask = 0; mask < 16; mask++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &entities[mask]);

        if (mask & 1) {
            Position pos = {0, 0, 0};
            YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entities[mask], g_position_type, &pos);
        }
        if (mask & 2) {
            Velocity vel = {0, 0, 0};
            YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entities[mask], g_velocity_type, &vel);
        }
        if (mask & 4) {
            Health health = {100, 100};
            YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entities[mask], g_health_type, &health);
        }
        if (mask & 8) {
            Damage damage = {10};
            YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entities[mask], g_damage_type, &damage);
        }
    }

    SDL_assert(registry.DenseCount == 16);  // 16 archetypes

    const uint32_t iterations = 1000000;

    uint64_t start = SDL_GetPerformanceCounter();

    // Benchmark finding existing archetypes
    for (uint32_t i = 0; i < iterations; i++) {
        YULDUZ_Entity new_entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &new_entity);

        uint32_t mask = i % 16;

        if (mask & 1) {
            Position pos = {0, 0, 0};
            YULDUZ_AddComponentWithTypeInECSRegistry(&registry, new_entity, g_position_type, &pos);
        }

        YULDUZ_DestroyEntityInECSRegistry(&registry, new_entity);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  1,000,000 archetype lookups: %.2f ms (%.0f ops/sec)", ms, 1000000.0 / (ms / 1000.0));

    YULDUZ_ReleaseECSRegistry(&registry);
}

void AssertFN(void *user_data, YULDUZ_AssertEntry *entry) {
    char message[2048];
    SDL_vsnprintf(message, sizeof(message), entry->Format, entry->FormatArguments);

    SDL_Log("[ASSERT][%s:%zu]: %s -> %s\n",
            entry->FunctionName,
            entry->LineNumber,
            entry->Condition,
            message);

    SDL_assert(false);
}

void LogFN(YULDUZ_Logger *logger, YULDUZ_LogEntry *entry) {
    char message[2048];
    SDL_vsnprintf(message, sizeof(message), entry->Format, entry->FormatArguments);

    SDL_Log("[%s][%s]: %s\n",
            logger->Name,
            YULDUZ_GetLogLevelName(entry->LogLevel),
            message);
}