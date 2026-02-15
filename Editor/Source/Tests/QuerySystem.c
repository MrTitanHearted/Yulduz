#include <Tests/Tests.h>

// ============================================================================
// QUERY TESTS
// ============================================================================

void run_all_query_tests(void) {
    YULDUZ_LOG_INFO("\n");
    YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
    YULDUZ_LOG_INFO("║           QUERY TESTS                                          ║");
    YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");
    YULDUZ_LOG_INFO("\n");

    test_query_initialization();
    test_query_component_requirements();
    test_query_tag_requirements();
    test_query_mixed_requirements();
    test_query_access_types();
    test_query_deep_copy();
    test_query_archetype_matching();
}

void test_query_initialization(void) {
    TEST_START("Query Initialization");

    YULDUZ_Query query = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeQuery(&query, 4, 4),
                  "Failed to initialize query");

    YULDUZ_ASSERT(query.RequiredComponentCapacity == 4, "Component capacity should be 4");
    YULDUZ_ASSERT(query.RequiredTagCapacity == 4, "Tag capacity should be 4");
    YULDUZ_ASSERT(query.RequiredComponentCount == 0, "Component count should start at 0");
    YULDUZ_ASSERT(query.RequiredTagCount == 0, "Tag count should start at 0");

    YULDUZ_ReleaseQuery(&query);

    TEST_END("Query Initialization");
}

void test_query_component_requirements(void) {
    TEST_START("Query Component Requirements");

    YULDUZ_Query query = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeQuery(&query, 4, 2),
                  "Failed to initialize query");

    // Add Position component with Read access
    YULDUZ_ASSERT(YULDUZ_SetQueryWithComponentType(&query, g_position_type,
                                                   YULDUZ_QueryAccessType_Read),
                  "Failed to add Position to query");

    YULDUZ_ASSERT(query.RequiredComponentCount == 1, "Should have 1 component requirement");

    // Add Velocity component with Write access
    YULDUZ_ASSERT(YULDUZ_SetQueryWithComponentType(&query, g_velocity_type,
                                                   YULDUZ_QueryAccessType_Write),
                  "Failed to add Velocity to query");

    YULDUZ_ASSERT(query.RequiredComponentCount == 2, "Should have 2 component requirements");

    // Verify components are stored
    bool found_position = false, found_velocity = false;
    for (uint32_t i = 0; i < query.RequiredComponentCount; i++) {
        if (query.RequiredComponentTypes[i] == g_position_type) {
            found_position = true;
            YULDUZ_ASSERT(query.RequiredComponentAccessTypes[i] == YULDUZ_QueryAccessType_Read,
                          "Position should have Read access");
        }
        if (query.RequiredComponentTypes[i] == g_velocity_type) {
            found_velocity = true;
            YULDUZ_ASSERT(query.RequiredComponentAccessTypes[i] == YULDUZ_QueryAccessType_Write,
                          "Velocity should have Write access");
        }
    }

    YULDUZ_ASSERT(found_position && found_velocity, "Both components should be in query");

    YULDUZ_ReleaseQuery(&query);

    TEST_END("Query Component Requirements");
}

void test_query_tag_requirements(void) {
    TEST_START("Query Tag Requirements");

    YULDUZ_Query query = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeQuery(&query, 2, 4),
                  "Failed to initialize query");

    // Add Player tag
    YULDUZ_ASSERT(YULDUZ_SetQueryWithTagType(&query, g_tag_player),
                  "Failed to add Player tag to query");

    YULDUZ_ASSERT(query.RequiredTagCount == 1, "Should have 1 tag requirement");

    // Add Active tag
    YULDUZ_ASSERT(YULDUZ_SetQueryWithTagType(&query, g_tag_active),
                  "Failed to add Active tag to query");

    YULDUZ_ASSERT(query.RequiredTagCount == 2, "Should have 2 tag requirements");

    // Verify tags are stored
    bool found_player = false, found_active = false;
    for (uint32_t i = 0; i < query.RequiredTagCount; i++) {
        if (query.RequiredTagTypes[i] == g_tag_player) found_player = true;
        if (query.RequiredTagTypes[i] == g_tag_active) found_active = true;
    }

    YULDUZ_ASSERT(found_player && found_active, "Both tags should be in query");

    YULDUZ_ReleaseQuery(&query);

    TEST_END("Query Tag Requirements");
}

void test_query_mixed_requirements(void) {
    TEST_START("Query Mixed Requirements (Components + Tags)");

    YULDUZ_Query query = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeQuery(&query, 4, 4),
                  "Failed to initialize query");

    // Add components
    YULDUZ_SetQueryWithComponentType(&query, g_position_type, YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&query, g_velocity_type, YULDUZ_QueryAccessType_Write);

    // Add tags
    YULDUZ_SetQueryWithTagType(&query, g_tag_player);
    YULDUZ_SetQueryWithTagType(&query, g_tag_active);

    YULDUZ_ASSERT(query.RequiredComponentCount == 2, "Should have 2 component requirements");
    YULDUZ_ASSERT(query.RequiredTagCount == 2, "Should have 2 tag requirements");

    // Create QueryInfo and verify
    YULDUZ_QueryInfo info;
    YULDUZ_ASSERT(YULDUZ_CreateQueryInfo(&query, &info),
                  "Failed to create query info");

    YULDUZ_ASSERT(info.WithComponentCount == 2, "QueryInfo should have 2 components");
    YULDUZ_ASSERT(info.WithTagCount == 2, "QueryInfo should have 2 tags");

    YULDUZ_DestroyQueryInfo(&info);
    YULDUZ_ReleaseQuery(&query);

    TEST_END("Query Mixed Requirements (Components + Tags)");
}

void test_query_access_types(void) {
    TEST_START("Query Access Types");

    YULDUZ_Query query = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeQuery(&query, 4, 2),
                  "Failed to initialize query");

    // Add components with different access types
    YULDUZ_SetQueryWithComponentType(&query, g_position_type, YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&query, g_velocity_type, YULDUZ_QueryAccessType_Write);
    YULDUZ_SetQueryWithComponentType(&query, g_health_type, YULDUZ_QueryAccessType_Read);

    // Verify access types
    for (uint32_t i = 0; i < query.RequiredComponentCount; i++) {
        if (query.RequiredComponentTypes[i] == g_position_type) {
            YULDUZ_ASSERT(query.RequiredComponentAccessTypes[i] == YULDUZ_QueryAccessType_Read,
                          "Position should have Read access");
        } else if (query.RequiredComponentTypes[i] == g_velocity_type) {
            YULDUZ_ASSERT(query.RequiredComponentAccessTypes[i] == YULDUZ_QueryAccessType_Write,
                          "Velocity should have Write access");
        } else if (query.RequiredComponentTypes[i] == g_health_type) {
            YULDUZ_ASSERT(query.RequiredComponentAccessTypes[i] == YULDUZ_QueryAccessType_Read,
                          "Health should have Read access");
        }
    }

    YULDUZ_ReleaseQuery(&query);

    TEST_END("Query Access Types");
}

void test_query_deep_copy(void) {
    TEST_START("Query Deep Copy");

    YULDUZ_Query src = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeQuery(&src, 4, 4),
                  "Failed to initialize source query");

    // Setup source query
    YULDUZ_SetQueryWithComponentType(&src, g_position_type, YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&src, g_velocity_type, YULDUZ_QueryAccessType_Write);
    YULDUZ_SetQueryWithTagType(&src, g_tag_player);

    // Deep copy
    YULDUZ_Query dst = {0};
    YULDUZ_ASSERT(YULDUZ_DeepCopyQuery(&src, &dst),
                  "Failed to deep copy query");

    // Verify copy
    YULDUZ_ASSERT(dst.RequiredComponentCount == src.RequiredComponentCount,
                  "Component count should match");
    YULDUZ_ASSERT(dst.RequiredTagCount == src.RequiredTagCount,
                  "Tag count should match");

    // Verify component types and access types copied
    for (uint32_t i = 0; i < src.RequiredComponentCount; i++) {
        bool found = false;
        for (uint32_t j = 0; j < dst.RequiredComponentCount; j++) {
            if (dst.RequiredComponentTypes[j] == src.RequiredComponentTypes[i]) {
                found = true;
                YULDUZ_ASSERT(dst.RequiredComponentAccessTypes[j] == src.RequiredComponentAccessTypes[i],
                              "Access type should match for component");
                break;
            }
        }
        YULDUZ_ASSERT(found, "Component should be in copy");
    }

    // Verify tags copied
    for (uint32_t i = 0; i < src.RequiredTagCount; i++) {
        bool found = false;
        for (uint32_t j = 0; j < dst.RequiredTagCount; j++) {
            if (dst.RequiredTagTypes[j] == src.RequiredTagTypes[i]) {
                found = true;
                break;
            }
        }
        YULDUZ_ASSERT(found, "Tag should be in copy");
    }

    YULDUZ_ReleaseQuery(&src);
    YULDUZ_ReleaseQuery(&dst);

    TEST_END("Query Deep Copy");
}

void test_query_archetype_matching(void) {
    TEST_START("Query Archetype Matching");

    // Create archetype with Position + Velocity + Health
    YULDUZ_Archetype archetype = {0};
    YULDUZ_TypeInfo  comps[]   = {
        {.Type = g_position_type, .Size = sizeof(Position), .Alignment = _Alignof(Position)},
        {.Type = g_velocity_type, .Size = sizeof(Velocity), .Alignment = _Alignof(Velocity)},
        {.Type = g_health_type, .Size = sizeof(Health), .Alignment = _Alignof(Health)}};
    YULDUZ_Type tags[] = {g_tag_player, g_tag_active};

    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&archetype, comps, 3, tags, 2, 4),
                  "Failed to initialize archetype");

    // Query 1: Position only (should match)
    YULDUZ_Query query1 = {0};
    YULDUZ_InitializeQuery(&query1, 4, 2);
    YULDUZ_SetQueryWithComponentType(&query1, g_position_type, YULDUZ_QueryAccessType_Read);

    YULDUZ_QueryInfo info1;
    YULDUZ_CreateQueryInfo(&query1, &info1);

    YULDUZ_ASSERT(YULDUZ_ArchetypeSupportsQueryInfo(&info1, &archetype),
                  "Archetype should match query for Position");

    // Query 2: Position + Velocity (should match)
    YULDUZ_Query query2 = {0};
    YULDUZ_InitializeQuery(&query2, 4, 2);
    YULDUZ_SetQueryWithComponentType(&query2, g_position_type, YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&query2, g_velocity_type, YULDUZ_QueryAccessType_Write);

    YULDUZ_QueryInfo info2;
    YULDUZ_CreateQueryInfo(&query2, &info2);

    YULDUZ_ASSERT(YULDUZ_ArchetypeSupportsQueryInfo(&info2, &archetype),
                  "Archetype should match query for Position + Velocity");

    // Query 3: Position + Damage (should NOT match - archetype doesn't have Damage)
    YULDUZ_Query query3 = {0};
    YULDUZ_InitializeQuery(&query3, 4, 2);
    YULDUZ_SetQueryWithComponentType(&query3, g_position_type, YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&query3, g_damage_type, YULDUZ_QueryAccessType_Read);

    YULDUZ_QueryInfo info3;
    YULDUZ_CreateQueryInfo(&query3, &info3);

    YULDUZ_ASSERT(!YULDUZ_ArchetypeSupportsQueryInfo(&info3, &archetype),
                  "Archetype should NOT match query requiring Damage");

    // Query 4: Position + Player tag (should match)
    YULDUZ_Query query4 = {0};
    YULDUZ_InitializeQuery(&query4, 4, 2);
    YULDUZ_SetQueryWithComponentType(&query4, g_position_type, YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithTagType(&query4, g_tag_player);

    YULDUZ_QueryInfo info4;
    YULDUZ_CreateQueryInfo(&query4, &info4);

    YULDUZ_ASSERT(YULDUZ_ArchetypeSupportsQueryInfo(&info4, &archetype),
                  "Archetype should match query for Position + Player tag");

    // Query 5: Enemy tag (should NOT match)
    YULDUZ_Query query5 = {0};
    YULDUZ_InitializeQuery(&query5, 4, 2);
    YULDUZ_SetQueryWithTagType(&query5, g_tag_enemy);

    YULDUZ_QueryInfo info5;
    YULDUZ_CreateQueryInfo(&query5, &info5);

    YULDUZ_ASSERT(!YULDUZ_ArchetypeSupportsQueryInfo(&info5, &archetype),
                  "Archetype should NOT match query requiring Enemy tag");

    // Cleanup
    YULDUZ_DestroyQueryInfo(&info1);
    YULDUZ_DestroyQueryInfo(&info2);
    YULDUZ_DestroyQueryInfo(&info3);
    YULDUZ_DestroyQueryInfo(&info4);
    YULDUZ_DestroyQueryInfo(&info5);

    YULDUZ_ReleaseQuery(&query1);
    YULDUZ_ReleaseQuery(&query2);
    YULDUZ_ReleaseQuery(&query3);
    YULDUZ_ReleaseQuery(&query4);
    YULDUZ_ReleaseQuery(&query5);

    YULDUZ_ReleaseArchetype(&archetype);

    TEST_END("Query Archetype Matching");
}

// ============================================================================
// SYSTEM TESTS
// ============================================================================

void run_all_system_tests(void) {
    YULDUZ_LOG_INFO("\n");
    YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
    YULDUZ_LOG_INFO("║           SYSTEM TESTS                                         ║");
    YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");
    YULDUZ_LOG_INFO("\n");

    test_system_initialization();
    test_system_execution();
    test_system_query_filtering();
    test_system_read_write_access();
    test_system_multiple_archetypes();
    test_system_with_user_data();
}

// System function for testing
static void TestMovementSystem(YULDUZ_Archetype *archetype, const YULDUZ_QueryInfo *query, void *user_data) {
    (void)query;
    (void)user_data;

    // Get component stores
    YULDUZ_ComponentStore *pos_store = YULDUZ_QueryStoreInArchetype(archetype, g_position_type);
    YULDUZ_ComponentStore *vel_store = YULDUZ_QueryStoreInArchetype(archetype, g_velocity_type);

    if (!pos_store || !vel_store) return;

    // Update positions based on velocities
    for (uint32_t i = 0; i < archetype->DenseCount; i++) {
        Position *pos = (Position *)YULDUZ_GetComponentInComponentStore(pos_store, i);
        Velocity *vel = (Velocity *)YULDUZ_GetComponentInComponentStore(vel_store, i);

        pos->x += vel->x;
        pos->y += vel->y;
        pos->z += vel->z;
    }
}

static uint32_t g_system_execution_count = 0;

static void CountingSystem(YULDUZ_Archetype *archetype, const YULDUZ_QueryInfo *query, void *user_data) {
    (void)archetype;
    (void)query;
    (void)user_data;
    g_system_execution_count++;
}

void test_system_initialization(void) {
    TEST_START("System Initialization");

    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 4, 2);
    YULDUZ_SetQueryWithComponentType(&query, g_position_type, YULDUZ_QueryAccessType_Write);
    YULDUZ_SetQueryWithComponentType(&query, g_velocity_type, YULDUZ_QueryAccessType_Read);

    YULDUZ_System system = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeSystem(&system, "MovementSystem", &query, &TestMovementSystem),
                  "Failed to initialize system");

    YULDUZ_ASSERT(SDL_strcmp(system.Name, "MovementSystem") == 0, "System name should match");
    YULDUZ_ASSERT(system.SystemPFN == &TestMovementSystem, "System function should match");
    YULDUZ_ASSERT(system.Query.WithComponentCount == 2, "System should have 2 component requirements");

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);

    TEST_END("System Initialization");
}

void test_system_execution(void) {
    TEST_START("System Execution");

    // Create ECS
    YULDUZ_ECSRegistry               ecs      = {0};
    YULDUZ_ECSRegistryInitializeInfo ecs_info = {
        .InitialTypeCapacity          = 8,
        .InitialEntityCapacity        = 16,
        .InitialArchetypeCapacity     = 8,
        .InitialArchetypeTypeCapacity = 8};
    YULDUZ_InitializeECSRegistry(&ecs, ecs_info);
    setup_test_types(&ecs.TypeRegistry);

    // Create entities with Position + Velocity
    YULDUZ_Entity e1, e2;
    YULDUZ_CreateEntityInECSRegistry(&ecs, &e1);
    YULDUZ_CreateEntityInECSRegistry(&ecs, &e2);

    Position pos1 = {0.0f, 0.0f, 0.0f};
    Velocity vel1 = {1.0f, 2.0f, 3.0f};
    YULDUZ_AddComponentInECSRegistry(&ecs, e1, "Position", &pos1);
    YULDUZ_AddComponentInECSRegistry(&ecs, e1, "Velocity", &vel1);

    Position pos2 = {10.0f, 20.0f, 30.0f};
    Velocity vel2 = {0.5f, 1.0f, 1.5f};
    YULDUZ_AddComponentInECSRegistry(&ecs, e2, "Position", &pos2);
    YULDUZ_AddComponentInECSRegistry(&ecs, e2, "Velocity", &vel2);

    // Create movement system
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 4, 2);
    YULDUZ_SetQueryWithComponentType(&query, g_position_type, YULDUZ_QueryAccessType_Write);
    YULDUZ_SetQueryWithComponentType(&query, g_velocity_type, YULDUZ_QueryAccessType_Read);

    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "MovementSystem", &query, &TestMovementSystem);

    // Run system
    YULDUZ_RunSystem(&system, &ecs, nullptr);

    // Verify positions updated
    Position check_pos1, check_pos2;
    YULDUZ_GetComponentInECSRegistry(&ecs, e1, "Position", &check_pos1);
    YULDUZ_GetComponentInECSRegistry(&ecs, e2, "Position", &check_pos2);

    YULDUZ_ASSERT(check_pos1.x == 1.0f && check_pos1.y == 2.0f && check_pos1.z == 3.0f,
                  "Entity 1 position should be updated");
    YULDUZ_ASSERT(check_pos2.x == 10.5f && check_pos2.y == 21.0f && check_pos2.z == 31.5f,
                  "Entity 2 position should be updated");

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("System Execution");
}

void test_system_query_filtering(void) {
    TEST_START("System Query Filtering");

    // Create ECS
    YULDUZ_ECSRegistry               ecs      = {0};
    YULDUZ_ECSRegistryInitializeInfo ecs_info = {
        .InitialTypeCapacity          = 8,
        .InitialEntityCapacity        = 16,
        .InitialArchetypeCapacity     = 8,
        .InitialArchetypeTypeCapacity = 8};
    YULDUZ_InitializeECSRegistry(&ecs, ecs_info);
    setup_test_types(&ecs.TypeRegistry);

    // Create entities:
    // e1: Position + Velocity
    // e2: Position only
    // e3: Position + Velocity + Health
    YULDUZ_Entity e1, e2, e3;
    YULDUZ_CreateEntityInECSRegistry(&ecs, &e1);
    YULDUZ_CreateEntityInECSRegistry(&ecs, &e2);
    YULDUZ_CreateEntityInECSRegistry(&ecs, &e3);

    Position pos    = {0.0f, 0.0f, 0.0f};
    Velocity vel    = {1.0f, 1.0f, 1.0f};
    Health   health = {100.0f, 100.0f};

    YULDUZ_AddComponentInECSRegistry(&ecs, e1, "Position", &pos);
    YULDUZ_AddComponentInECSRegistry(&ecs, e1, "Velocity", &vel);

    YULDUZ_AddComponentInECSRegistry(&ecs, e2, "Position", &pos);

    YULDUZ_AddComponentInECSRegistry(&ecs, e3, "Position", &pos);
    YULDUZ_AddComponentInECSRegistry(&ecs, e3, "Velocity", &vel);
    YULDUZ_AddComponentInECSRegistry(&ecs, e3, "Health", &health);

    // Create system that requires Position + Velocity
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 4, 2);
    YULDUZ_SetQueryWithComponentType(&query, g_position_type, YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&query, g_velocity_type, YULDUZ_QueryAccessType_Read);

    g_system_execution_count = 0;

    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "CountingSystem", &query, &CountingSystem);

    YULDUZ_RunSystem(&system, &ecs, nullptr);

    // System should run on 2 archetypes: [Position, Velocity] and [Position, Velocity, Health]
    // But NOT on [Position] only
    YULDUZ_ASSERT(g_system_execution_count == 2,
                  "System should run on 2 matching archetypes");

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("System Query Filtering");
}

void test_system_read_write_access(void) {
    TEST_START("System Read/Write Access");

    // This test verifies that access types are properly stored
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 4, 2);

    YULDUZ_SetQueryWithComponentType(&query, g_position_type, YULDUZ_QueryAccessType_Write);
    YULDUZ_SetQueryWithComponentType(&query, g_velocity_type, YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&query, g_health_type, YULDUZ_QueryAccessType_Read);

    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "TestSystem", &query, &CountingSystem);

    // Verify access types through query info
    YULDUZ_ASSERT(system.Query.WithComponentCount == 3,
                  "System should have 3 component requirements");

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);

    TEST_END("System Read/Write Access");
}

void test_system_multiple_archetypes(void) {
    TEST_START("System Multiple Archetypes");

    YULDUZ_ECSRegistry               ecs      = {0};
    YULDUZ_ECSRegistryInitializeInfo ecs_info = {
        .InitialTypeCapacity          = 8,
        .InitialEntityCapacity        = 32,
        .InitialArchetypeCapacity     = 16,
        .InitialArchetypeTypeCapacity = 16};
    YULDUZ_InitializeECSRegistry(&ecs, ecs_info);
    setup_test_types(&ecs.TypeRegistry);

    // Create 10 entities with varying component combinations, all having Position
    for (uint32_t i = 0; i < 10; i++) {
        YULDUZ_Entity e;
        YULDUZ_CreateEntityInECSRegistry(&ecs, &e);

        Position pos = {(float)i, 0.0f, 0.0f};
        YULDUZ_AddComponentInECSRegistry(&ecs, e, "Position", &pos);

        if (i % 2 == 0) {
            Velocity vel = {1.0f, 1.0f, 1.0f};
            YULDUZ_AddComponentInECSRegistry(&ecs, e, "Velocity", &vel);
        }

        if (i % 3 == 0) {
            Health health = {100.0f, 100.0f};
            YULDUZ_AddComponentInECSRegistry(&ecs, e, "Health", &health);
        }
    }

    // Create system that only requires Position
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 4, 2);
    YULDUZ_SetQueryWithComponentType(&query, g_position_type, YULDUZ_QueryAccessType_Read);

    g_system_execution_count = 0;

    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "CountingSystem", &query, &CountingSystem);

    YULDUZ_RunSystem(&system, &ecs, nullptr);

    YULDUZ_LOG_INFO("System ran on %u archetypes", g_system_execution_count);
    YULDUZ_ASSERT(g_system_execution_count > 0, "System should run on multiple archetypes");

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("System Multiple Archetypes");
}

typedef struct UserData {
    uint32_t processed_count;
} UserData;

static void UserDataSystem(YULDUZ_Archetype *archetype, const YULDUZ_QueryInfo *query, void *user_data) {
    (void)query;

    UserData *data = (UserData *)user_data;
    data->processed_count += archetype->DenseCount;
}

void test_system_with_user_data(void) {
    TEST_START("System With User Data");

    YULDUZ_ECSRegistry               ecs      = {0};
    YULDUZ_ECSRegistryInitializeInfo ecs_info = {
        .InitialTypeCapacity          = 8,
        .InitialEntityCapacity        = 16,
        .InitialArchetypeCapacity     = 8,
        .InitialArchetypeTypeCapacity = 8};
    YULDUZ_InitializeECSRegistry(&ecs, ecs_info);
    setup_test_types(&ecs.TypeRegistry);

    // Create 5 entities with Position
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_Entity e;
        YULDUZ_CreateEntityInECSRegistry(&ecs, &e);
        Position pos = {(float)i, 0.0f, 0.0f};
        YULDUZ_AddComponentInECSRegistry(&ecs, e, "Position", &pos);
    }

    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 4, 2);
    YULDUZ_SetQueryWithComponentType(&query, g_position_type, YULDUZ_QueryAccessType_Read);

    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "UserDataSystem", &query, &UserDataSystem);

    UserData data = {.processed_count = 0};
    YULDUZ_RunSystem(&system, &ecs, &data);

    YULDUZ_ASSERT(data.processed_count == 5, "Should have processed 5 entities");

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("System With User Data");
}