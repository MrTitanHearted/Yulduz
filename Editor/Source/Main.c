#include <Yulduz/Engine.h>
#include <time.h>

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
YULDUZ_ComponentType g_position_type = YULDUZ_INVALID_COMPONENT_TYPE;
YULDUZ_ComponentType g_velocity_type = YULDUZ_INVALID_COMPONENT_TYPE;
YULDUZ_ComponentType g_health_type = YULDUZ_INVALID_COMPONENT_TYPE;
YULDUZ_ComponentType g_damage_type = YULDUZ_INVALID_COMPONENT_TYPE;
YULDUZ_ComponentType g_name_type = YULDUZ_INVALID_COMPONENT_TYPE;

// Test functions
void setup_component_registry(YULDUZ_ComponentTypeRegistry *registry);
void test_basic_add_remove(YULDUZ_ComponentTypeRegistry *registry);
void test_move_entities_between_archetypes(YULDUZ_ComponentTypeRegistry *registry);
void test_query_stores(YULDUZ_ComponentTypeRegistry *registry);
void test_capacity_growth(YULDUZ_ComponentTypeRegistry *registry);
void test_remove_with_swapback(YULDUZ_ComponentTypeRegistry *registry);
void test_mixed_operations(YULDUZ_ComponentTypeRegistry *registry);
void test_sorting_functions(void);

// New stress tests
void test_move_edge_cases(YULDUZ_ComponentTypeRegistry *registry);
void test_move_component_overlap(YULDUZ_ComponentTypeRegistry *registry);
void test_move_with_null_data(YULDUZ_ComponentTypeRegistry *registry);
void benchmark_add_performance(YULDUZ_ComponentTypeRegistry *registry);
void benchmark_remove_performance(YULDUZ_ComponentTypeRegistry *registry);
void benchmark_move_performance(YULDUZ_ComponentTypeRegistry *registry);
void benchmark_query_performance(YULDUZ_ComponentTypeRegistry *registry);

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
    YULDUZ_ComponentTypeRegistry registry;
    YULDUZ_InitializeComponentTypeRegistry(&registry, 16);
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

    YULDUZ_ReleaseComponentTypeRegistry(&registry);

    YULDUZ_LOG_INFO("\n=== ALL TESTS PASSED ===");

    YULDUZ_ReleaseLogs();
    YULDUZ_ReleaseAsserts();
    SDL_stack_free(loggers);
    SDL_Quit();

    return 0;
}

void setup_component_registry(YULDUZ_ComponentTypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[SETUP] Registering Component Types");

    YULDUZ_ComponentTypeInfo type_infos[] = {
        {"Position", sizeof(Position), alignof(Position)},
        {"Velocity", sizeof(Velocity), alignof(Velocity)},
        {"Health", sizeof(Health), alignof(Health)},
        {"Damage", sizeof(Damage), alignof(Damage)},
        {"Name", sizeof(Name), alignof(Name)}
    };

    YULDUZ_ComponentType types[5];

    bool result = YULDUZ_RegisterComponentTypesInComponentTypeRegistry(
        registry, type_infos, types, 5
    );

    SDL_assert(result);

    g_position_type = types[0];
    g_velocity_type = types[1];
    g_health_type = types[2];
    g_damage_type = types[3];
    g_name_type = types[4];

    YULDUZ_LOG_INFO("  ✓ Registered 5 component types");
}

void test_sorting_functions(void) {
    YULDUZ_LOG_INFO("\n[TEST] Sorting Functions");

    YULDUZ_ComponentType types[] = {5, 2, 8, 1, 3};
    YULDUZ_SDL_SortComponentTypes(types, 5);
    SDL_assert(types[0] == 1 && types[4] == 8);

    YULDUZ_LOG_INFO("  ✓ Sorting Functions PASSED");
}

void test_basic_add_remove(YULDUZ_ComponentTypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Basic Add/Remove");

    YULDUZ_ComponentType query_types[] = {g_position_type, g_velocity_type};
    YULDUZ_ComponentTypeInfo type_infos[2];
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, query_types, type_infos, 2);

    YULDUZ_ComponentTypeDescription types[] = {
        {g_position_type, type_infos[0].Size, type_infos[0].Alignment},
        {g_velocity_type, type_infos[1].Size, type_infos[1].Alignment}
    };
    YULDUZ_SDL_SortComponentTypeDescriptions(types, 2);

    YULDUZ_Archetype archetype;
    YULDUZ_InitializeArchetype(&archetype, types, 2, nullptr, 0, 4);

    for (uint32_t i = 0; i < 10; i++) {
        Position pos = {(float)i, (float)i * 2, (float)i * 3};
        Velocity vel = {1.0f, 2.0f, 3.0f};

        YULDUZ_ComponentDataInfo data[] = {
            {g_position_type, 0, &pos},
            {g_velocity_type, 0, &vel}
        };
        YULDUZ_SDL_SortComponentDataInfos(data, 2);

        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, (YULDUZ_Entity)i, data, &index);
    }

    YULDUZ_ComponentStore *pos_store = YULDUZ_QueryStoreInArchetype(&archetype, g_position_type);
    Position *positions = (Position *)pos_store->Dense;

    for (uint32_t i = 0; i < 10; i++) {
        SDL_assert(positions[i].x == (float)i);
    }

    YULDUZ_RemoveInArchetype(&archetype, 5);
    YULDUZ_RemoveInArchetype(&archetype, 0);
    SDL_assert(archetype.DenseCount == 8);

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_LOG_INFO("  ✓ Basic Add/Remove PASSED");
}

void test_move_entities_between_archetypes(YULDUZ_ComponentTypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Move Entities Between Archetypes");

    YULDUZ_ComponentType src_query[] = {g_position_type, g_velocity_type};
    YULDUZ_ComponentType dst_query[] = {g_position_type, g_velocity_type, g_health_type};

    YULDUZ_ComponentTypeInfo src_infos[2], dst_infos[3];
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, src_query, src_infos, 2);
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, dst_query, dst_infos, 3);

    YULDUZ_ComponentTypeDescription src_types[] = {
        {g_position_type, src_infos[0].Size, src_infos[0].Alignment},
        {g_velocity_type, src_infos[1].Size, src_infos[1].Alignment}
    };
    YULDUZ_SDL_SortComponentTypeDescriptions(src_types, 2);

    YULDUZ_ComponentTypeDescription dst_types[] = {
        {g_position_type, dst_infos[0].Size, dst_infos[0].Alignment},
        {g_velocity_type, dst_infos[1].Size, dst_infos[1].Alignment},
        {g_health_type, dst_infos[2].Size, dst_infos[2].Alignment}
    };
    YULDUZ_SDL_SortComponentTypeDescriptions(dst_types, 3);

    YULDUZ_Archetype src_arch, dst_arch;
    YULDUZ_InitializeArchetype(&src_arch, src_types, 2, nullptr, 0, 4);
    YULDUZ_InitializeArchetype(&dst_arch, dst_types, 3, nullptr, 0, 4);

    Position pos = {10.0f, 20.0f, 30.0f};
    Velocity vel = {1.0f, 2.0f, 3.0f};

    YULDUZ_ComponentDataInfo src_data[] = {
        {g_position_type, 0, &pos},
        {g_velocity_type, 0, &vel}
    };
    YULDUZ_SDL_SortComponentDataInfos(src_data, 2);

    YULDUZ_ArchetypeIndex src_index;
    YULDUZ_AddInArchetype(&src_arch, 100, src_data, &src_index);

    Health health = {100.0f, 100.0f};
    YULDUZ_ComponentDataInfo new_data[] = {{g_health_type, 0, &health}};

    YULDUZ_ArchetypeIndex dst_index;
    YULDUZ_MoveEntityInArchetype(&src_arch, &dst_arch, new_data, 1, src_index, &dst_index);

    SDL_assert(src_arch.DenseCount == 0);
    SDL_assert(dst_arch.DenseCount == 1);

    YULDUZ_ComponentStore *dst_pos = YULDUZ_QueryStoreInArchetype(&dst_arch, g_position_type);
    Position *p = (Position *)YULDUZ_GetComponentInComponentStore(dst_pos, 0);
    SDL_assert(p->x == 10.0f && p->y == 20.0f && p->z == 30.0f);

    YULDUZ_ReleaseArchetype(&src_arch);
    YULDUZ_ReleaseArchetype(&dst_arch);
    YULDUZ_LOG_INFO("  ✓ Move Between Archetypes PASSED");
}

void test_query_stores(YULDUZ_ComponentTypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Query Stores");

    YULDUZ_ComponentType query[] = {g_position_type, g_velocity_type, g_health_type};
    YULDUZ_ComponentTypeInfo type_infos[3];
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, query, type_infos, 3);

    YULDUZ_ComponentTypeDescription types[] = {
        {g_position_type, type_infos[0].Size, type_infos[0].Alignment},
        {g_velocity_type, type_infos[1].Size, type_infos[1].Alignment},
        {g_health_type, type_infos[2].Size, type_infos[2].Alignment}
    };
    YULDUZ_SDL_SortComponentTypeDescriptions(types, 3);

    YULDUZ_Archetype archetype;
    YULDUZ_InitializeArchetype(&archetype, types, 3, nullptr, 0, 4);

    YULDUZ_ComponentType query_types[] = {g_health_type, g_position_type};
    YULDUZ_ComponentStore *stores[2];

    bool result = YULDUZ_QueryStoresInArchetype(&archetype, query_types, 2, nullptr, 0, stores);
    SDL_assert(result);
    SDL_assert(stores[0]->Type == g_health_type);
    SDL_assert(stores[1]->Type == g_position_type);

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_LOG_INFO("  ✓ Query Stores PASSED");
}

void test_capacity_growth(YULDUZ_ComponentTypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Capacity Growth");

    YULDUZ_ComponentType query[] = {g_position_type};
    YULDUZ_ComponentTypeInfo type_info;
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, query, &type_info, 1);

    YULDUZ_ComponentTypeDescription types[] = {
        {g_position_type, type_info.Size, type_info.Alignment}
    };

    YULDUZ_Archetype archetype;
    YULDUZ_InitializeArchetype(&archetype, types, 1, nullptr, 0, 2);

    for (uint32_t i = 0; i < 100; i++) {
        Position pos = {(float)i, (float)i, (float)i};
        YULDUZ_ComponentDataInfo data[] = {{g_position_type, 0, &pos}};
        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, (YULDUZ_Entity)i, data, &index);
    }

    SDL_assert(archetype.DenseCount == 100);

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_LOG_INFO("  ✓ Capacity Growth PASSED");
}

void test_remove_with_swapback(YULDUZ_ComponentTypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Remove with Swap-Back");

    YULDUZ_ComponentType query[] = {g_position_type};
    YULDUZ_ComponentTypeInfo type_info;
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, query, &type_info, 1);

    YULDUZ_ComponentTypeDescription types[] = {
        {g_position_type, type_info.Size, type_info.Alignment}
    };

    YULDUZ_Archetype archetype;
    YULDUZ_InitializeArchetype(&archetype, types, 1, nullptr, 0, 10);

    for (uint32_t i = 0; i < 5; i++) {
        Position pos = {(float)i * 100, 0, 0};
        YULDUZ_ComponentDataInfo data[] = {{g_position_type, 0, &pos}};
        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, (YULDUZ_Entity)(1000 + i), data, &index);
    }

    YULDUZ_ComponentStore *store = YULDUZ_QueryStoreInArchetype(&archetype, g_position_type);
    Position *positions = (Position *)store->Dense;

    YULDUZ_RemoveInArchetype(&archetype, 2);
    SDL_assert(archetype.DenseCount == 4);
    SDL_assert(positions[2].x == 400.0f);

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_LOG_INFO("  ✓ Remove with Swap-Back PASSED");
}

void test_mixed_operations(YULDUZ_ComponentTypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Mixed Operations");

    YULDUZ_ComponentType query[] = {g_position_type, g_velocity_type, g_health_type};
    YULDUZ_ComponentTypeInfo type_infos[3];
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, query, type_infos, 3);

    YULDUZ_ComponentTypeDescription types[] = {
        {g_position_type, type_infos[0].Size, type_infos[0].Alignment},
        {g_velocity_type, type_infos[1].Size, type_infos[1].Alignment},
        {g_health_type, type_infos[2].Size, type_infos[2].Alignment}
    };
    YULDUZ_SDL_SortComponentTypeDescriptions(types, 3);

    YULDUZ_Archetype archetype;
    YULDUZ_InitializeArchetype(&archetype, types, 3, nullptr, 0, 8);

    uint32_t operations = 1000;
    uint32_t seed = (uint32_t)time(nullptr);

    for (uint32_t op = 0; op < operations; op++) {
        uint32_t choice = (seed * 1103515245 + 12345) % 100;
        seed = choice;

        if (choice < 60 && archetype.DenseCount < 100) {
            Position pos = {(float)op, (float)op, (float)op};
            Velocity vel = {1, 1, 1};
            Health health = {100, 100};

            YULDUZ_ComponentDataInfo data[] = {
                {g_position_type, 0, &pos},
                {g_velocity_type, 0, &vel},
                {g_health_type, 0, &health}
            };
            YULDUZ_SDL_SortComponentDataInfos(data, 3);

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

void test_move_edge_cases(YULDUZ_ComponentTypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Move Edge Cases");

    // Test 1: Move when destination has MORE components than source
    YULDUZ_ComponentType src_q[] = {g_position_type};
    YULDUZ_ComponentType dst_q[] = {g_position_type, g_velocity_type, g_health_type, g_damage_type};

    YULDUZ_ComponentTypeInfo src_infos[1], dst_infos[4];
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, src_q, src_infos, 1);
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, dst_q, dst_infos, 4);

    YULDUZ_ComponentTypeDescription src_types[] = {
        {g_position_type, src_infos[0].Size, src_infos[0].Alignment}
    };

    YULDUZ_ComponentTypeDescription dst_types[] = {
        {g_position_type, dst_infos[0].Size, dst_infos[0].Alignment},
        {g_velocity_type, dst_infos[1].Size, dst_infos[1].Alignment},
        {g_health_type, dst_infos[2].Size, dst_infos[2].Alignment},
        {g_damage_type, dst_infos[3].Size, dst_infos[3].Alignment}
    };
    YULDUZ_SDL_SortComponentTypeDescriptions(dst_types, 4);

    YULDUZ_Archetype src, dst;
    YULDUZ_InitializeArchetype(&src, src_types, 1, nullptr, 0, 4);
    YULDUZ_InitializeArchetype(&dst, dst_types, 4, nullptr, 0, 4);

    Position pos = {42.0f, 43.0f, 44.0f};
    YULDUZ_ComponentDataInfo src_data[] = {{g_position_type, 0, &pos}};

    YULDUZ_ArchetypeIndex src_idx;
    YULDUZ_AddInArchetype(&src, 999, src_data, &src_idx);

    // Provide only 2 of the 3 new components
    Velocity vel = {5.0f, 6.0f, 7.0f};
    Health health = {50.0f, 100.0f};
    YULDUZ_ComponentDataInfo new_data[] = {
        {g_velocity_type, 0, &vel},
        {g_health_type, 0, &health}
        // Damage will be zero-initialized
    };
    YULDUZ_SDL_SortComponentDataInfos(new_data, 2);

    YULDUZ_ArchetypeIndex dst_idx;
    YULDUZ_MoveEntityInArchetype(&src, &dst, new_data, 2, src_idx, &dst_idx);

    // Verify Position was preserved
    YULDUZ_ComponentStore *dst_pos = YULDUZ_QueryStoreInArchetype(&dst, g_position_type);
    Position *p = (Position *)YULDUZ_GetComponentInComponentStore(dst_pos, 0);
    SDL_assert(p->x == 42.0f && p->y == 43.0f && p->z == 44.0f);

    // Verify new components
    YULDUZ_ComponentStore *dst_vel = YULDUZ_QueryStoreInArchetype(&dst, g_velocity_type);
    Velocity *v = (Velocity *)YULDUZ_GetComponentInComponentStore(dst_vel, 0);
    SDL_assert(v->x == 5.0f);

    // Verify zero-initialized component
    YULDUZ_ComponentStore *dst_dmg = YULDUZ_QueryStoreInArchetype(&dst, g_damage_type);
    Damage *d = (Damage *)YULDUZ_GetComponentInComponentStore(dst_dmg, 0);
    SDL_assert(d->value == 0.0f);

    YULDUZ_ReleaseArchetype(&src);
    YULDUZ_ReleaseArchetype(&dst);

    YULDUZ_LOG_INFO("  ✓ Move with many new components works");
    YULDUZ_LOG_INFO("  ✓ Move Edge Cases PASSED");
}

void test_move_component_overlap(YULDUZ_ComponentTypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Move Component Overlap");

    // Test moving between archetypes with complete overlap
    YULDUZ_ComponentType query[] = {g_position_type, g_velocity_type};
    YULDUZ_ComponentTypeInfo type_infos[2];
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, query, type_infos, 2);

    YULDUZ_ComponentTypeDescription types[] = {
        {g_position_type, type_infos[0].Size, type_infos[0].Alignment},
        {g_velocity_type, type_infos[1].Size, type_infos[1].Alignment}
    };
    YULDUZ_SDL_SortComponentTypeDescriptions(types, 2);

    YULDUZ_Archetype src, dst;
    YULDUZ_InitializeArchetype(&src, types, 2, nullptr, 0, 4);
    YULDUZ_InitializeArchetype(&dst, types, 2, nullptr, 0, 4);

    Position pos = {100.0f, 200.0f, 300.0f};
    Velocity vel = {10.0f, 20.0f, 30.0f};

    YULDUZ_ComponentDataInfo data[] = {
        {g_position_type, 0, &pos},
        {g_velocity_type, 0, &vel}
    };
    YULDUZ_SDL_SortComponentDataInfos(data, 2);

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

void test_move_with_null_data(YULDUZ_ComponentTypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[TEST] Move with NULL Data");

    YULDUZ_ComponentType src_q[] = {g_position_type};
    YULDUZ_ComponentType dst_q[] = {g_position_type, g_health_type};

    YULDUZ_ComponentTypeInfo src_infos[1], dst_infos[2];
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, src_q, src_infos, 1);
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, dst_q, dst_infos, 2);

    YULDUZ_ComponentTypeDescription src_types[] = {
        {g_position_type, src_infos[0].Size, src_infos[0].Alignment}
    };

    YULDUZ_ComponentTypeDescription dst_types[] = {
        {g_position_type, dst_infos[0].Size, dst_infos[0].Alignment},
        {g_health_type, dst_infos[1].Size, dst_infos[1].Alignment}
    };
    YULDUZ_SDL_SortComponentTypeDescriptions(dst_types, 2);

    YULDUZ_Archetype src, dst;
    YULDUZ_InitializeArchetype(&src, src_types, 1, nullptr, 0, 4);
    YULDUZ_InitializeArchetype(&dst, dst_types, 2, nullptr, 0, 4);

    Position pos = {11.0f, 22.0f, 33.0f};
    YULDUZ_ComponentDataInfo src_data[] = {{g_position_type, 0, &pos}};

    YULDUZ_ArchetypeIndex src_idx;
    YULDUZ_AddInArchetype(&src, 555, src_data, &src_idx);

    // Pass NULL data for new component - should zero-initialize
    YULDUZ_ComponentDataInfo new_data[] = {{g_health_type, 0, nullptr}};

    YULDUZ_ArchetypeIndex dst_idx;
    YULDUZ_MoveEntityInArchetype(&src, &dst, new_data, 1, src_idx, &dst_idx);

    // Verify health was zero-initialized
    YULDUZ_ComponentStore *dst_health = YULDUZ_QueryStoreInArchetype(&dst, g_health_type);
    Health *h = (Health *)YULDUZ_GetComponentInComponentStore(dst_health, 0);
    SDL_assert(h->current == 0.0f && h->max == 0.0f);

    YULDUZ_ReleaseArchetype(&src);
    YULDUZ_ReleaseArchetype(&dst);

    YULDUZ_LOG_INFO("  ✓ NULL data zero-initializes correctly");
    YULDUZ_LOG_INFO("  ✓ Move with NULL Data PASSED");
}

void benchmark_add_performance(YULDUZ_ComponentTypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Add Performance");

    YULDUZ_ComponentType query[] = {g_position_type, g_velocity_type};
    YULDUZ_ComponentTypeInfo type_infos[2];
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, query, type_infos, 2);

    YULDUZ_ComponentTypeDescription types[] = {
        {g_position_type, type_infos[0].Size, type_infos[0].Alignment},
        {g_velocity_type, type_infos[1].Size, type_infos[1].Alignment}
    };
    YULDUZ_SDL_SortComponentTypeDescriptions(types, 2);

    YULDUZ_Archetype archetype;
    YULDUZ_InitializeArchetype(&archetype, types, 2, nullptr, 0, 100000);

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < 100000; i++) {
        Position pos = {(float)i, (float)i, (float)i};
        Velocity vel = {1, 1, 1};

        YULDUZ_ComponentDataInfo data[] = {
            {g_position_type, 0, &pos},
            {g_velocity_type, 0, &vel}
        };
        YULDUZ_SDL_SortComponentDataInfos(data, 2);

        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, (YULDUZ_Entity)i, data, &index);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  100,000 adds: %.2f ms (%.0f ops/sec)", ms, 100000.0 / (ms / 1000.0));

    YULDUZ_ReleaseArchetype(&archetype);
}

void benchmark_remove_performance(YULDUZ_ComponentTypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Remove Performance");

    YULDUZ_ComponentType query[] = {g_position_type};
    YULDUZ_ComponentTypeInfo type_info;
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, query, &type_info, 1);

    YULDUZ_ComponentTypeDescription types[] = {
        {g_position_type, type_info.Size, type_info.Alignment}
    };

    YULDUZ_Archetype archetype;
    YULDUZ_InitializeArchetype(&archetype, types, 1, nullptr, 0, 50000);

    // Add 50000 entities
    for (uint32_t i = 0; i < 50000; i++) {
        Position pos = {(float)i, 0, 0};
        YULDUZ_ComponentDataInfo data[] = {{g_position_type, 0, &pos}};
        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, (YULDUZ_Entity)i, data, &index);
    }

    uint64_t start = SDL_GetPerformanceCounter();

    // Remove all (from back for consistency)
    while (archetype.DenseCount > 0) {
        YULDUZ_RemoveInArchetype(&archetype, archetype.DenseCount - 1);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  50,000 removes: %.2f ms (%.0f ops/sec)", ms, 50000.0 / (ms / 1000.0));

    YULDUZ_ReleaseArchetype(&archetype);
}

void benchmark_move_performance(YULDUZ_ComponentTypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Move Performance");

    YULDUZ_ComponentType src_q[] = {g_position_type, g_velocity_type};
    YULDUZ_ComponentType dst_q[] = {g_position_type, g_velocity_type, g_health_type};

    YULDUZ_ComponentTypeInfo src_infos[2], dst_infos[3];
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, src_q, src_infos, 2);
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, dst_q, dst_infos, 3);

    YULDUZ_ComponentTypeDescription src_types[] = {
        {g_position_type, src_infos[0].Size, src_infos[0].Alignment},
        {g_velocity_type, src_infos[1].Size, src_infos[1].Alignment}
    };
    YULDUZ_SDL_SortComponentTypeDescriptions(src_types, 2);

    YULDUZ_ComponentTypeDescription dst_types[] = {
        {g_position_type, dst_infos[0].Size, dst_infos[0].Alignment},
        {g_velocity_type, dst_infos[1].Size, dst_infos[1].Alignment},
        {g_health_type, dst_infos[2].Size, dst_infos[2].Alignment}
    };
    YULDUZ_SDL_SortComponentTypeDescriptions(dst_types, 3);

    YULDUZ_Archetype src, dst;
    YULDUZ_InitializeArchetype(&src, src_types, 2, nullptr, 0, 10000);
    YULDUZ_InitializeArchetype(&dst, dst_types, 3, nullptr, 0, 10000);

    // Add 10000 entities to source
    for (uint32_t i = 0; i < 10000; i++) {
        Position pos = {(float)i, (float)i, (float)i};
        Velocity vel = {1, 1, 1};

        YULDUZ_ComponentDataInfo data[] = {
            {g_position_type, 0, &pos},
            {g_velocity_type, 0, &vel}
        };
        YULDUZ_SDL_SortComponentDataInfos(data, 2);

        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&src, (YULDUZ_Entity)i, data, &index);
    }

    Health health = {100, 100};
    YULDUZ_ComponentDataInfo new_data[] = {{g_health_type, 0, &health}};

    uint64_t start = SDL_GetPerformanceCounter();

    // Move all entities
    while (src.DenseCount > 0) {
        YULDUZ_ArchetypeIndex dst_idx;
        YULDUZ_MoveEntityInArchetype(&src, &dst, new_data, 1, 0, &dst_idx);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  10,000 moves: %.2f ms (%.0f ops/sec)", ms, 10000.0 / (ms / 1000.0));

    YULDUZ_ReleaseArchetype(&src);
    YULDUZ_ReleaseArchetype(&dst);
}

void benchmark_query_performance(YULDUZ_ComponentTypeRegistry *registry) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Query Performance");

    YULDUZ_ComponentType query[] = {g_position_type, g_velocity_type, g_health_type};
    YULDUZ_ComponentTypeInfo type_infos[3];
    YULDUZ_GetComponentTypeInfosInComponentTypeRegistry(registry, query, type_infos, 3);

    YULDUZ_ComponentTypeDescription types[] = {
        {g_position_type, type_infos[0].Size, type_infos[0].Alignment},
        {g_velocity_type, type_infos[1].Size, type_infos[1].Alignment},
        {g_health_type, type_infos[2].Size, type_infos[2].Alignment}
    };
    YULDUZ_SDL_SortComponentTypeDescriptions(types, 3);

    YULDUZ_Archetype archetype;
    YULDUZ_InitializeArchetype(&archetype, types, 3, nullptr, 0, 4);

    YULDUZ_ComponentType query_types[] = {g_velocity_type, g_position_type};
    YULDUZ_ComponentStore *stores[2];

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < 1000000; i++) {
        YULDUZ_QueryStoresInArchetype(&archetype, query_types, 2, nullptr, 0, stores);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

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
    YULDUZ_Entity check_entities[] = {entities[3], entities[5], entities[8]};
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
        records_to_set[i].ArchetypeType = i * 10;
        records_to_set[i].ArchetypeIndex = i * 100;
        records_to_set[i].Alive = true;
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
    bool result = YULDUZ_DestroyEntityInEntityRegistry(&registry, invalid);
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
    result = YULDUZ_SetEntityRecordsInEntityRegistry(&registry, &invalid, &set_record, 1);
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
    YULDUZ_Entity entities[count];

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
        records[i].ArchetypeType = i % 3;
        records[i].ArchetypeIndex = i;
        records[i].Alive = true;
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
        records[i].ArchetypeType = (i % 3) + 1;
        records[i].ArchetypeIndex = i + 100;
    }
    YULDUZ_SetEntityRecordsInEntityRegistry(&registry, entities, records, 10);

    YULDUZ_GetEntityRecordsInEntityRegistry(&registry, entities, retrieved, 10);
    for (uint32_t i = 0; i < 10; i++) {
        SDL_assert(retrieved[i].ArchetypeType == (i % 3) + 1);
        SDL_assert(retrieved[i].ArchetypeIndex == i + 100);
    }

    // Test invalid archetype values
    records[0].ArchetypeType = YULDUZ_INVALID_ARCHETYPE_TYPE;
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
    YULDUZ_Entity entities[batch_size];

    for (uint32_t i = 0; i < batch_size; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]);
    }

    // Batch set
    YULDUZ_EntityRecord records[batch_size];
    for (uint32_t i = 0; i < batch_size; i++) {
        records[i].ArchetypeType = i % 10;
        records[i].ArchetypeIndex = i;
        records[i].Alive = true;
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

    const uint32_t cycles = 100;
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
    YULDUZ_Entity entities[max_entities];
    bool entity_alive[max_entities] = {};
    SDL_memset(entities, 0x00, sizeof(bool) * SDL_arraysize(entities));
    uint32_t alive_count = 0;

    srand(12345);

    for (uint32_t op = 0; op < 5000; op++) {
        int operation = rand() % 3;

        if (operation == 0 && alive_count < max_entities) {
            YULDUZ_Entity new_entity;
            if (YULDUZ_CreateEntityInEntityRegistry(&registry, &new_entity)) {
                entities[alive_count] = new_entity;
                entity_alive[alive_count] = true;
                alive_count++;
            }
        }
        else if (operation == 1 && alive_count > 0) {
            uint32_t idx = rand() % alive_count;
            if (entity_alive[idx]) {
                YULDUZ_DestroyEntityInEntityRegistry(&registry, entities[idx]);
                entity_alive[idx] = false;

                entities[idx] = entities[alive_count - 1];
                entity_alive[idx] = entity_alive[alive_count - 1];
                alive_count--;
            }
        }
        else if (operation == 2 && alive_count > 0) {
            uint32_t idx = rand() % alive_count;
            if (entity_alive[idx]) {
                YULDUZ_EntityRecord record = {
                    .ArchetypeType = rand() % 20,
                    .ArchetypeIndex = rand() % 100,
                    .Alive = true
                };
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

    const uint32_t count = 100000;
    YULDUZ_Entity *entities = SDL_malloc(count * sizeof(YULDUZ_Entity));

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  100,000 creates: %.2f ms (%.0f ops/sec)", ms, 100000.0 / (ms / 1000.0));

    SDL_free(entities);
    YULDUZ_ReleaseEntityRegistry(&registry);
}

void benchmark_entity_destruction(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Entity Destruction");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 100000);

    const uint32_t count = 100000;
    YULDUZ_Entity *entities = SDL_malloc(count * sizeof(YULDUZ_Entity));

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]);
    }

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_DestroyEntityInEntityRegistry(&registry, entities[i]);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  100,000 destroys: %.2f ms (%.0f ops/sec)", ms, 100000.0 / (ms / 1000.0));

    SDL_free(entities);
    YULDUZ_ReleaseEntityRegistry(&registry);
}

void benchmark_entity_reuse(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Entity Reuse");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 100000);

    const uint32_t count = 50000;
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
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  50,000 creates (reused): %.2f ms (%.0f ops/sec)", ms, 50000.0 / (ms / 1000.0));

    SDL_free(entities);
    YULDUZ_ReleaseEntityRegistry(&registry);
}

void benchmark_entity_get_records(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Get Entity Records");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 10000);

    const uint32_t entity_count = 10000;
    YULDUZ_Entity *entities = SDL_malloc(entity_count * sizeof(YULDUZ_Entity));
    YULDUZ_EntityRecord *records = SDL_malloc(entity_count * sizeof(YULDUZ_EntityRecord));

    for (uint32_t i = 0; i < entity_count; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]);
    }

    const uint32_t iterations = 10000;

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t iter = 0; iter < iterations; iter++) {
        YULDUZ_GetEntityRecordsInEntityRegistry(&registry, entities, records, entity_count);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  100,000,000 gets: %.2f ms (%.0f ops/sec)", ms, 100000000.0 / (ms / 1000.0));

    SDL_free(entities);
    SDL_free(records);
    YULDUZ_ReleaseEntityRegistry(&registry);
}

void benchmark_entity_set_records(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Set Entity Records");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 10000);

    const uint32_t entity_count = 10000;
    YULDUZ_Entity *entities = SDL_malloc(entity_count * sizeof(YULDUZ_Entity));
    YULDUZ_EntityRecord *records = SDL_malloc(entity_count * sizeof(YULDUZ_EntityRecord));

    for (uint32_t i = 0; i < entity_count; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entities[i]);
        records[i].ArchetypeType = i % 100;
        records[i].ArchetypeIndex = i;
        records[i].Alive = true;
    }

    const uint32_t iterations = 10000;

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t iter = 0; iter < iterations; iter++) {
        YULDUZ_SetEntityRecordsInEntityRegistry(&registry, entities, records, entity_count);
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  100,000,000 sets: %.2f ms (%.0f ops/sec)", ms, 100000000.0 / (ms / 1000.0));

    SDL_free(entities);
    SDL_free(records);
    YULDUZ_ReleaseEntityRegistry(&registry);
}

void benchmark_entity_mixed_ops(void) {
    YULDUZ_LOG_INFO("\n[BENCHMARK] Mixed Entity Operations");

    YULDUZ_EntityRegistry registry;
    YULDUZ_InitializeEntityRegistry(&registry, 50000);

    const uint32_t iterations = 50000;
    YULDUZ_Entity entity;
    YULDUZ_EntityRecord record;

    uint64_t start = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < iterations; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, &entity);

        record.ArchetypeType = i % 20;
        record.ArchetypeIndex = i;
        record.Alive = true;
        YULDUZ_SetEntityRecordsInEntityRegistry(&registry, &entity, &record, 1);

        YULDUZ_GetEntityRecordsInEntityRegistry(&registry, &entity, &record, 1);

        if (i % 2 == 0) {
            YULDUZ_DestroyEntityInEntityRegistry(&registry, entity);
        }
    }

    uint64_t end = SDL_GetPerformanceCounter();
    double ms = (double)(end - start) * 1000.0 / SDL_GetPerformanceFrequency();

    YULDUZ_LOG_INFO("  200,000 mixed ops: %.2f ms (%.0f ops/sec)", ms, 200000.0 / (ms / 1000.0));

    YULDUZ_ReleaseEntityRegistry(&registry);
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