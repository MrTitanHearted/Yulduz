#include <Tests/Tests.h>

// ============================================================
// TEST TYPES
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

// ============================================================
// SYSTEM TEST DATA
// ============================================================

typedef struct SystemTestData {
    uint32_t archetype_call_count;
    uint32_t total_entity_count;
    uint32_t custom_value;
    float    delta_time;
    bool     was_called;
} SystemTestData;

// ============================================================
// TEST SYSTEM CALLBACKS
// ============================================================

static void empty_system(
    const YULDUZ_Archetype *archetype,
    const YULDUZ_QueryInfo *query,
    void                   *user_data) {
    SystemTestData *data = (SystemTestData *)user_data;
    data->archetype_call_count++;
    data->total_entity_count += archetype->DenseCount;
    data->was_called = true;
}

static void position_update_system(
    const YULDUZ_Archetype *archetype,
    const YULDUZ_QueryInfo *query,
    void                   *user_data) {
    SystemTestData *data = (SystemTestData *)user_data;

    // Find Position store
    YULDUZ_ComponentStore *pos_store = NULL;
    for (uint32_t i = 0; i < archetype->StoreCount; i++) {
        // Compare with first component in query
        if (archetype->Stores[i].Type == query->WithComponentTypes[0]) {
            pos_store = &archetype->Stores[i];
            break;
        }
    }

    if (pos_store != NULL) {
        // Update all positions
        for (uint32_t i = 0; i < archetype->DenseCount; i++) {
            TestPosition *pos = (TestPosition *)YULDUZ_GetComponentInComponentStore(pos_store, i);
            pos->x += 1.0f * data->delta_time;
            pos->y += 1.0f * data->delta_time;
            pos->z += 1.0f * data->delta_time;
        }
    }

    data->archetype_call_count++;
    data->total_entity_count += archetype->DenseCount;
}

// ============================================================
// HELPER FUNCTIONS
// ============================================================

static bool setup_test_ecs(YULDUZ_ECSRegistry *ecs) {
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialEntityCapacity        = 64,
        .InitialTagTypeCapacity       = 16,
        .InitialComponentTypeCapacity = 16,
        .InitialArchetypeCapacity     = 16,
        .InitialArchetypeTypeCapacity = 16,
    };
    return YULDUZ_InitializeECSRegistry(ecs, &info);
}

// ============================================================
// SYSTEM INITIALIZATION TESTS
// ============================================================

static void test_system_initialization(void) {
    TEST_START("System: Initialization and Release");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Register component
    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type));

    // Create query
    YULDUZ_Query query = {0};
    TEST_ASSERT_TRUE(YULDUZ_InitializeQuery(&query, 4));
    TEST_ASSERT_TRUE(YULDUZ_SetQueryWithComponentType(&query, pos_type, YULDUZ_QueryAccessType_Read));

    // Create system with UserData
    SystemTestData data   = {.custom_value = 42};
    YULDUZ_System  system = {0};
    TEST_ASSERT_TRUE(YULDUZ_InitializeSystem(&system, "TestSystem", &data, &query, empty_system));

    // Verify initialization
    TEST_ASSERT_NOT_NULL(system.Name);
    TEST_ASSERT_EQUAL(SDL_strcmp(system.Name, "TestSystem"), 0);
    TEST_ASSERT_EQUAL(system.UserData, &data);
    TEST_ASSERT_EQUAL(system.SystemPFN, empty_system);
    TEST_ASSERT_EQUAL(system.DenseCount, 0);
    TEST_ASSERT_EQUAL(system.LastArchetypeCount, 0);

    // Release
    YULDUZ_ReleaseSystem(&system);
    TEST_ASSERT_NULL(system.Name);
    TEST_ASSERT_NULL(system.Dense);

    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

// ============================================================
// SYSTEM EXECUTION TESTS
// ============================================================

static void test_system_execution_basic(void) {
    TEST_START("System: Basic Execution");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Register component
    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type));

    // Create entities
    TestPosition pos = {1.0f, 2.0f, 3.0f};
    for (uint32_t i = 0; i < 10; i++) {
        YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
        TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &entity));
        TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &pos));
    }

    // Create system
    YULDUZ_Query query = {0};
    TEST_ASSERT_TRUE(YULDUZ_InitializeQuery(&query, 4));
    TEST_ASSERT_TRUE(YULDUZ_SetQueryWithComponentType(&query, pos_type, YULDUZ_QueryAccessType_Read));

    SystemTestData data   = {0};
    YULDUZ_System  system = {0};
    TEST_ASSERT_TRUE(YULDUZ_InitializeSystem(&system, "CountSystem", &data, &query, empty_system));

    // Run system
    YULDUZ_RunSystem(&system, &ecs);

    // Verify system was called
    TEST_ASSERT_TRUE(data.was_called);
    TEST_ASSERT_EQUAL(data.archetype_call_count, 1);  // One archetype: [Position]
    TEST_ASSERT_EQUAL(data.total_entity_count, 10);   // 10 entities

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

static void test_system_user_data(void) {
    TEST_START("System: UserData Passed Correctly");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Register component
    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type));

    // Create entity
    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &entity));

    TestPosition pos = {5.0f, 10.0f, 15.0f};
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &pos));

    // Create system with custom UserData
    YULDUZ_Query query = {0};
    TEST_ASSERT_TRUE(YULDUZ_InitializeQuery(&query, 4));
    TEST_ASSERT_TRUE(YULDUZ_SetQueryWithComponentType(&query, pos_type, YULDUZ_QueryAccessType_Write));

    SystemTestData data   = {.delta_time = 0.016f, .custom_value = 999};
    YULDUZ_System  system = {0};
    TEST_ASSERT_TRUE(YULDUZ_InitializeSystem(&system, "UpdateSystem", &data, &query, position_update_system));

    // Run system
    YULDUZ_RunSystem(&system, &ecs);

    // Verify UserData was used
    TEST_ASSERT_EQUAL(data.archetype_call_count, 1);
    TEST_ASSERT_EQUAL(data.custom_value, 999);  // Unchanged

    // Verify position was updated using delta_time
    TestPosition updated_pos = {0};
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &updated_pos));
    TEST_ASSERT_TRUE(updated_pos.x > 5.0f);  // Should be incremented
    TEST_ASSERT_TRUE(updated_pos.y > 10.0f);
    TEST_ASSERT_TRUE(updated_pos.z > 15.0f);

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

static void test_system_multiple_archetypes(void) {
    TEST_START("System: Runs on Multiple Matching Archetypes");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Register components
    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType vel_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType hp_type  = YULDUZ_INVALID_COMPONENT_TYPE;

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type));

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Velocity", .Size = sizeof(TestVelocity), .Alignment = _Alignof(TestVelocity), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &vel_type));

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Health", .Size = sizeof(TestHealth), .Alignment = _Alignof(TestHealth), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &hp_type));

    // Create entities in different archetypes:
    // Archetype 1: [Position] - 5 entities
    TestPosition pos = {0};
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_Entity e = YULDUZ_INVALID_ENTITY;
        TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &e));
        TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, pos_type, &pos));
    }

    // Archetype 2: [Position, Velocity] - 3 entities
    TestVelocity vel = {0};
    for (uint32_t i = 0; i < 3; i++) {
        YULDUZ_Entity e = YULDUZ_INVALID_ENTITY;
        TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &e));
        TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, pos_type, &pos));
        TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, vel_type, &vel));
    }

    // Archetype 3: [Position, Health] - 2 entities
    TestHealth hp = {0};
    for (uint32_t i = 0; i < 2; i++) {
        YULDUZ_Entity e = YULDUZ_INVALID_ENTITY;
        TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &e));
        TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, pos_type, &pos));
        TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e, hp_type, &hp));
    }

    // System queries for Position (all 3 archetypes match)
    YULDUZ_Query query = {0};
    TEST_ASSERT_TRUE(YULDUZ_InitializeQuery(&query, 4));
    TEST_ASSERT_TRUE(YULDUZ_SetQueryWithComponentType(&query, pos_type, YULDUZ_QueryAccessType_Read));

    SystemTestData data   = {0};
    YULDUZ_System  system = {0};
    TEST_ASSERT_TRUE(YULDUZ_InitializeSystem(&system, "PositionSystem", &data, &query, empty_system));

    // Run system
    YULDUZ_RunSystem(&system, &ecs);

    // Verify system ran on all 3 archetypes
    TEST_ASSERT_EQUAL(data.archetype_call_count, 3);
    TEST_ASSERT_EQUAL(data.total_entity_count, 10);  // 5 + 3 + 2

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

static void test_system_query_filtering(void) {
    TEST_START("System: Query Filtering (WITH/WITHOUT)");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Register components and tags
    YULDUZ_ComponentType pos_type   = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType vel_type   = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_TagType       active_tag = YULDUZ_INVALID_TAG_TYPE;
    YULDUZ_TagType       dead_tag   = YULDUZ_INVALID_TAG_TYPE;

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type));

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Velocity", .Size = sizeof(TestVelocity), .Alignment = _Alignof(TestVelocity), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &vel_type));

    TEST_ASSERT_TRUE(YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "Active", &active_tag));
    TEST_ASSERT_TRUE(YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "Dead", &dead_tag));

    TestPosition pos = {0};
    TestVelocity vel = {0};

    // Entity 1: [Position, Velocity] + [Active] → MATCHES
    YULDUZ_Entity e1 = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &e1));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e1, pos_type, &pos));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e1, vel_type, &vel));
    TEST_ASSERT_TRUE(YULDUZ_AddTagWithTypeInECSRegistry(&ecs, e1, active_tag));

    // Entity 2: [Position, Velocity] + [Active, Dead] → EXCLUDED (has Dead)
    YULDUZ_Entity e2 = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &e2));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e2, pos_type, &pos));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e2, vel_type, &vel));
    TEST_ASSERT_TRUE(YULDUZ_AddTagWithTypeInECSRegistry(&ecs, e2, active_tag));
    TEST_ASSERT_TRUE(YULDUZ_AddTagWithTypeInECSRegistry(&ecs, e2, dead_tag));

    // Entity 3: [Position, Velocity] (no Active tag) → EXCLUDED (missing Active)
    YULDUZ_Entity e3 = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &e3));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e3, pos_type, &pos));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e3, vel_type, &vel));

    // System: WITH Position, Velocity, Active; WITHOUT Dead
    YULDUZ_Query query = {0};
    TEST_ASSERT_TRUE(YULDUZ_InitializeQuery(&query, 4));
    TEST_ASSERT_TRUE(YULDUZ_SetQueryWithComponentType(&query, pos_type, YULDUZ_QueryAccessType_Read));
    TEST_ASSERT_TRUE(YULDUZ_SetQueryWithComponentType(&query, vel_type, YULDUZ_QueryAccessType_Read));
    TEST_ASSERT_TRUE(YULDUZ_SetQueryWithTagType(&query, active_tag));
    TEST_ASSERT_TRUE(YULDUZ_SetQueryWithoutTagType(&query, dead_tag));

    SystemTestData data   = {0};
    YULDUZ_System  system = {0};
    TEST_ASSERT_TRUE(YULDUZ_InitializeSystem(&system, "ActiveMovementSystem", &data, &query, empty_system));

    // Run system
    YULDUZ_RunSystem(&system, &ecs);

    // Only e1's archetype should match
    TEST_ASSERT_EQUAL(data.archetype_call_count, 1);
    TEST_ASSERT_EQUAL(data.total_entity_count, 1);

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

static void test_system_incremental_cache(void) {
    TEST_START("System: Incremental Archetype Cache");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Register component
    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type));

    // Create system
    YULDUZ_Query query = {0};
    TEST_ASSERT_TRUE(YULDUZ_InitializeQuery(&query, 4));
    TEST_ASSERT_TRUE(YULDUZ_SetQueryWithComponentType(&query, pos_type, YULDUZ_QueryAccessType_Read));

    SystemTestData data   = {0};
    YULDUZ_System  system = {0};
    TEST_ASSERT_TRUE(YULDUZ_InitializeSystem(&system, "TestSystem", &data, &query, empty_system));

    // Run 1: No entities
    YULDUZ_RunSystem(&system, &ecs);
    TEST_ASSERT_EQUAL(data.archetype_call_count, 0);
    TEST_ASSERT_EQUAL(system.DenseCount, 0);

    // Create entity (new archetype created)
    TestPosition  pos = {0};
    YULDUZ_Entity e1  = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &e1));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e1, pos_type, &pos));

    // Run 2: System discovers new archetype
    data = (SystemTestData){0};
    YULDUZ_RunSystem(&system, &ecs);
    TEST_ASSERT_EQUAL(data.archetype_call_count, 1);
    TEST_ASSERT_EQUAL(data.total_entity_count, 1);
    TEST_ASSERT_EQUAL(system.DenseCount, 1);  // 1 archetype cached

    // Add more entities to same archetype (no new archetype)
    YULDUZ_Entity e2 = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &e2));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e2, pos_type, &pos));

    // Run 3: Same archetype, more entities
    data = (SystemTestData){0};
    YULDUZ_RunSystem(&system, &ecs);
    TEST_ASSERT_EQUAL(data.archetype_call_count, 1);  // Still 1 archetype
    TEST_ASSERT_EQUAL(data.total_entity_count, 2);    // But 2 entities now
    TEST_ASSERT_EQUAL(system.DenseCount, 1);          // Cache unchanged

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

// ============================================================
// ENTRY POINT
// ============================================================

void run_all_system_tests(void) {
    YULDUZ_LOG_INFO("");
    YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
    YULDUZ_LOG_INFO("║           SYSTEM TESTS                                         ║");
    YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");
    YULDUZ_LOG_INFO("");

    test_system_initialization();
    test_system_execution_basic();
    test_system_user_data();
    test_system_multiple_archetypes();
    test_system_query_filtering();
    test_system_incremental_cache();
}