#include <Yulduz/Engine.h>

// Test helper macros
#define TEST_ASSERT(condition, fmt, ...)                                        \
    do {                                                                        \
        if (!(condition)) {                                                     \
            YULDUZ_LOG_INFO("TEST FAILED: %s - " fmt, __func__, ##__VA_ARGS__); \
            return false;                                                       \
        }                                                                       \
    } while (0)

#define TEST_PASS()                                   \
    do {                                              \
        YULDUZ_LOG_INFO("TEST PASSED: %s", __func__); \
        return true;                                  \
    } while (0)

// Helper structures
typedef struct Position {
    float x, y, z;
} Position;

typedef struct Velocity {
    float x, y, z;
} Velocity;

typedef struct Health {
    float value;
    float max_value;
} Health;

typedef struct TestUserData {
    uint32_t call_count;
    uint32_t entity_count;
    float    total_x;
} TestUserData;

// ============================================================================
// Test System Functions
// ============================================================================

void TestSystem_Empty(const YULDUZ_Archetype *archetype, const YULDUZ_QueryInfo *query, void *user_data) {
    (void)archetype;
    (void)query;

    TestUserData *data = (TestUserData *)user_data;
    data->call_count++;
}

void TestSystem_CountEntities(const YULDUZ_Archetype *archetype, const YULDUZ_QueryInfo *query, void *user_data) {
    (void)query;

    TestUserData *data = (TestUserData *)user_data;
    data->call_count++;
    data->entity_count += archetype->DenseCount;
}

void TestSystem_ProcessPosition(const YULDUZ_Archetype *archetype, const YULDUZ_QueryInfo *query, void *user_data) {
    (void)query;

    TestUserData *data = (TestUserData *)user_data;
    data->call_count++;

    // Find Position component store
    for (uint32_t i = 0; i < archetype->StoreCount; i++) {
        if (archetype->Stores[i].Type == query->WithComponentTypes[0]) {
            Position *positions = (Position *)archetype->Stores[i].Dense;
            for (uint32_t j = 0; j < archetype->DenseCount; j++) {
                data->total_x += positions[j].x;
            }
            break;
        }
    }
}

void TestSystem_UpdateVelocity(const YULDUZ_Archetype *archetype, const YULDUZ_QueryInfo *query, void *user_data) {
    (void)query;
    (void)user_data;

    // Find Position and Velocity stores
    Position *positions  = NULL;
    Velocity *velocities = NULL;

    for (uint32_t i = 0; i < archetype->StoreCount; i++) {
        for (uint32_t q = 0; q < query->WithComponentCount; q++) {
            if (archetype->Stores[i].Type == query->WithComponentTypes[q]) {
                // Assuming first is Position, second is Velocity based on query order
                if (q == 0) {
                    positions = (Position *)archetype->Stores[i].Dense;
                } else if (q == 1) {
                    velocities = (Velocity *)archetype->Stores[i].Dense;
                }
            }
        }
    }

    if (positions && velocities) {
        for (uint32_t i = 0; i < archetype->DenseCount; i++) {
            positions[i].x += velocities[i].x;
            positions[i].y += velocities[i].y;
            positions[i].z += velocities[i].z;
        }
    }
}

// ============================================================================
// System Tests
// ============================================================================

bool test_system_initialize_release(void) {
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);

    YULDUZ_System system = {0};
    TEST_ASSERT(YULDUZ_InitializeSystem(&system, "TestSystem", &query, &TestSystem_Empty),
                "Failed to initialize system");
    TEST_ASSERT(system.Name != NULL, "Name should be set");
    TEST_ASSERT(system.SystemPFN == &TestSystem_Empty, "System function pointer mismatch");
    TEST_ASSERT(system.DenseCount == 0, "Initial archetype count should be 0");

    YULDUZ_ReleaseSystem(&system);
    TEST_ASSERT(system.Name == NULL, "Name should be null after release");

    YULDUZ_ReleaseQuery(&query);
    TEST_PASS();
}

bool test_system_run_empty(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 32, 8, 16};
    YULDUZ_InitializeECSRegistry(&registry, info);

    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);

    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "TestSystem", &query, &TestSystem_Empty);

    TestUserData user_data = {0};
    YULDUZ_RunSystem(&system, &registry, &user_data);

    // Should be called at least once for null archetype
    TEST_ASSERT(user_data.call_count >= 1, "System should be called at least once");

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_system_with_single_component_query(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 32, 8, 16};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register Position component
    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};
    YULDUZ_Type pos_type;
    YULDUZ_RegisterTypeInECSRegistry(&registry, desc, &pos_type);

    // Create query for Position
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);
    YULDUZ_SetQueryWithComponentType(&query, pos_type, YULDUZ_QueryAccessType_Read);

    // Create system
    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "PositionSystem", &query, &TestSystem_CountEntities);

    // Create entities with Position
    for (uint32_t i = 0; i < 10; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos = {(float)i, 0.0f, 0.0f};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
    }

    // Run system
    TestUserData user_data = {0};
    YULDUZ_RunSystem(&system, &registry, &user_data);

    TEST_ASSERT(user_data.entity_count == 10, "Should process 10 entities");

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_system_with_multiple_component_query(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 64, 16, 32};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register components
    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)}};

    YULDUZ_Type types[2];
    for (uint32_t i = 0; i < 2; i++) {
        YULDUZ_RegisterTypeInECSRegistry(&registry, descs[i], &types[i]);
    }

    // Create query for Position and Velocity
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);
    YULDUZ_SetQueryWithComponentType(&query, types[0], YULDUZ_QueryAccessType_Write);
    YULDUZ_SetQueryWithComponentType(&query, types[1], YULDUZ_QueryAccessType_Read);

    // Create system
    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "MovementSystem", &query, &TestSystem_CountEntities);

    // Create entities with both components
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos = {0.0f, 0.0f, 0.0f};
        Velocity vel = {1.0f, 0.0f, 0.0f};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Velocity", &vel);
    }

    // Create entities with only Position (should not match)
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos = {0.0f, 0.0f, 0.0f};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
    }

    // Run system
    TestUserData user_data = {0};
    YULDUZ_RunSystem(&system, &registry, &user_data);

    TEST_ASSERT(user_data.entity_count == 5, "Should only process entities with both components");

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_system_with_tag_query(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 64, 16, 32};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register types
    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Player", .Size = 0, .Alignment = 0}};

    YULDUZ_Type types[2];
    for (uint32_t i = 0; i < 2; i++) {
        YULDUZ_RegisterTypeInECSRegistry(&registry, descs[i], &types[i]);
    }

    // Create query for Position + Player tag
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);
    YULDUZ_SetQueryWithComponentType(&query, types[0], YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithTagType(&query, types[1]);

    // Create system
    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "PlayerSystem", &query, &TestSystem_CountEntities);

    // Create player entities
    for (uint32_t i = 0; i < 3; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos = {0.0f, 0.0f, 0.0f};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
        YULDUZ_AddTagInECSRegistry(&registry, entity, "Player");
    }

    // Create non-player entities
    for (uint32_t i = 0; i < 7; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos = {0.0f, 0.0f, 0.0f};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
    }

    // Run system
    TestUserData user_data = {0};
    YULDUZ_RunSystem(&system, &registry, &user_data);

    TEST_ASSERT(user_data.entity_count == 3, "Should only process player entities");

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_system_with_exclusion_query(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 64, 16, 32};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register types
    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Dead", .Size = 0, .Alignment = 0}};

    YULDUZ_Type types[2];
    for (uint32_t i = 0; i < 2; i++) {
        YULDUZ_RegisterTypeInECSRegistry(&registry, descs[i], &types[i]);
    }

    // Create query for Position WITHOUT Dead tag
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);
    YULDUZ_SetQueryWithComponentType(&query, types[0], YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithoutTagType(&query, types[1]);

    // Create system
    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "AliveSystem", &query, &TestSystem_CountEntities);

    // Create alive entities
    for (uint32_t i = 0; i < 8; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos = {0.0f, 0.0f, 0.0f};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
    }

    // Create dead entities
    for (uint32_t i = 0; i < 2; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos = {0.0f, 0.0f, 0.0f};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
        YULDUZ_AddTagInECSRegistry(&registry, entity, "Dead");
    }

    // Run system
    TestUserData user_data = {0};
    YULDUZ_RunSystem(&system, &registry, &user_data);

    TEST_ASSERT(user_data.entity_count == 8, "Should only process alive entities");

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_system_component_processing(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 64, 16, 32};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register Position
    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};
    YULDUZ_Type pos_type;
    YULDUZ_RegisterTypeInECSRegistry(&registry, desc, &pos_type);

    // Create query
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);
    YULDUZ_SetQueryWithComponentType(&query, pos_type, YULDUZ_QueryAccessType_Read);

    // Create system
    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "SumSystem", &query, &TestSystem_ProcessPosition);

    // Create entities with specific positions
    float expected_sum = 0.0f;
    for (uint32_t i = 0; i < 10; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos = {(float)i * 10.0f, 0.0f, 0.0f};
        expected_sum += pos.x;
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
    }

    // Run system
    TestUserData user_data = {0};
    YULDUZ_RunSystem(&system, &registry, &user_data);

    TEST_ASSERT(user_data.total_x == expected_sum,
                "Sum mismatch: expected %.2f, got %.2f", expected_sum, user_data.total_x);

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_system_multiple_runs(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 64, 16, 32};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register Position
    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};
    YULDUZ_Type pos_type;
    YULDUZ_RegisterTypeInECSRegistry(&registry, desc, &pos_type);

    // Create query
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);
    YULDUZ_SetQueryWithComponentType(&query, pos_type, YULDUZ_QueryAccessType_Read);

    // Create system
    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "TestSystem", &query, &TestSystem_CountEntities);

    // Create entities
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos = {0.0f, 0.0f, 0.0f};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
    }

    // Run system multiple times
    TestUserData user_data = {0};
    for (uint32_t i = 0; i < 10; i++) {
        YULDUZ_RunSystem(&system, &registry, &user_data);
    }

    TEST_ASSERT(user_data.call_count == 10, "System should be called 10 times");
    TEST_ASSERT(user_data.entity_count == 50, "Should process 5 entities x 10 runs");

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_system_dynamic_archetype_updates(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 64, 16, 32};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register components
    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)}};

    YULDUZ_Type types[2];
    for (uint32_t i = 0; i < 2; i++) {
        YULDUZ_RegisterTypeInECSRegistry(&registry, descs[i], &types[i]);
    }

    // Create query for Position + Velocity
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);
    YULDUZ_SetQueryWithComponentType(&query, types[0], YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&query, types[1], YULDUZ_QueryAccessType_Read);

    // Create system
    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "MovementSystem", &query, &TestSystem_CountEntities);

    // Create entities with only Position
    YULDUZ_Entity entities[5];
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &entities[i]);
        Position pos = {0.0f, 0.0f, 0.0f};
        YULDUZ_AddComponentInECSRegistry(&registry, entities[i], "Position", &pos);
    }

    // Run system - should find 0 entities
    TestUserData user_data1 = {0};
    YULDUZ_RunSystem(&system, &registry, &user_data1);
    TEST_ASSERT(user_data1.entity_count == 0, "Should find 0 entities initially");

    // Add Velocity to entities
    for (uint32_t i = 0; i < 5; i++) {
        Velocity vel = {1.0f, 0.0f, 0.0f};
        YULDUZ_AddComponentInECSRegistry(&registry, entities[i], "Velocity", &vel);
    }

    // Run system again - should find 5 entities
    TestUserData user_data2 = {0};
    YULDUZ_RunSystem(&system, &registry, &user_data2);
    TEST_ASSERT(user_data2.entity_count == 5, "Should find 5 entities after adding Velocity");

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_system_complex_scenario(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 128, 32, 64};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register types
    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)},
        {.Name = "Health", .Size = sizeof(Health), .Alignment = alignof(Health)},
        {.Name = "Player", .Size = 0, .Alignment = 0},
        {.Name = "Enemy", .Size = 0, .Alignment = 0}};

    YULDUZ_Type types[5];
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_RegisterTypeInECSRegistry(&registry, descs[i], &types[i]);
    }

    // Create multiple systems

    // System 1: All entities with Position
    YULDUZ_Query query1 = {0};
    YULDUZ_InitializeQuery(&query1, 16);
    YULDUZ_SetQueryWithComponentType(&query1, types[0], YULDUZ_QueryAccessType_Read);

    YULDUZ_System system1 = {0};
    YULDUZ_InitializeSystem(&system1, "AllPositions", &query1, &TestSystem_CountEntities);

    // System 2: Player entities with Position and Health
    YULDUZ_Query query2 = {0};
    YULDUZ_InitializeQuery(&query2, 16);
    YULDUZ_SetQueryWithComponentType(&query2, types[0], YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&query2, types[2], YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithTagType(&query2, types[3]);

    YULDUZ_System system2 = {0};
    YULDUZ_InitializeSystem(&system2, "PlayerHealth", &query2, &TestSystem_CountEntities);

    // Create varied entities
    // 5 players with Position, Velocity, Health
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos    = {0.0f, 0.0f, 0.0f};
        Velocity vel    = {1.0f, 0.0f, 0.0f};
        Health   health = {100.0f, 100.0f};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Velocity", &vel);
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Health", &health);
        YULDUZ_AddTagInECSRegistry(&registry, entity, "Player");
    }

    // 10 enemies with Position and Health
    for (uint32_t i = 0; i < 10; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos    = {0.0f, 0.0f, 0.0f};
        Health   health = {50.0f, 50.0f};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Health", &health);
        YULDUZ_AddTagInECSRegistry(&registry, entity, "Enemy");
    }

    // 5 static objects with just Position
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos = {0.0f, 0.0f, 0.0f};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
    }

    // Run systems
    TestUserData data1 = {0};
    YULDUZ_RunSystem(&system1, &registry, &data1);
    TEST_ASSERT(data1.entity_count == 20, "System1 should find all 20 entities with Position");

    TestUserData data2 = {0};
    YULDUZ_RunSystem(&system2, &registry, &data2);
    TEST_ASSERT(data2.entity_count == 5, "System2 should find only 5 player entities");

    YULDUZ_ReleaseSystem(&system1);
    YULDUZ_ReleaseSystem(&system2);
    YULDUZ_ReleaseQuery(&query1);
    YULDUZ_ReleaseQuery(&query2);
    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

// ============================================================================
// Test Runner
// ============================================================================

void run_all_system_tests(void) {
    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("Running System Tests");
    YULDUZ_LOG_INFO("========================================\n");

    uint32_t passed = 0;
    uint32_t total  = 0;

#define RUN_TEST(test)        \
    do {                      \
        total++;              \
        if (test()) passed++; \
    } while (0)

    RUN_TEST(test_system_initialize_release);
    RUN_TEST(test_system_run_empty);
    RUN_TEST(test_system_with_single_component_query);
    RUN_TEST(test_system_with_multiple_component_query);
    RUN_TEST(test_system_with_tag_query);
    RUN_TEST(test_system_with_exclusion_query);
    RUN_TEST(test_system_component_processing);
    RUN_TEST(test_system_multiple_runs);
    RUN_TEST(test_system_dynamic_archetype_updates);
    RUN_TEST(test_system_complex_scenario);

#undef RUN_TEST

    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("System Tests: %u/%u passed", passed, total);
    YULDUZ_LOG_INFO("========================================\n");
}