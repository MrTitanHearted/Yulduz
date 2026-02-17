#include <Yulduz/Engine.h>
#include <SDL3/SDL.h>

// Test helper macros
#define TEST_ASSERT(condition, message)                                 \
    do {                                                                \
        if (!(condition)) {                                             \
            YULDUZ_LOG_INFO("TEST FAILED: %s - %s", __func__, message); \
            return false;                                               \
        }                                                               \
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

typedef struct Name {
    char data[64];
} Name;

// ============================================================================
// ECS Registry Tests
// ============================================================================

bool test_ecs_registry_initialize_release(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {
            .InitialTypeCapacity          = 16,
            .InitialEntityCapacity        = 32,
            .InitialArchetypeCapacity     = 8,
            .InitialArchetypeTypeCapacity = 16};

    TEST_ASSERT(YULDUZ_InitializeECSRegistry(&registry, info),
                "Failed to initialize ECS registry");
    TEST_ASSERT(registry.DenseCapacity >= 8, "Archetype capacity not set");
    TEST_ASSERT(registry.DenseCount >= 1, "Should have null archetype");

    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_ecs_registry_register_types(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 32, 8, 16};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register component types
    YULDUZ_TypeDescription pos_desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Type pos_type = YULDUZ_INVALID_TYPE;
    TEST_ASSERT(YULDUZ_RegisterTypeInECSRegistry(&registry, pos_desc, &pos_type),
                "Failed to register Position type");
    TEST_ASSERT(pos_type != YULDUZ_INVALID_TYPE, "Type should be valid");

    // Register tag type
    YULDUZ_TypeDescription player_desc = {
        .Name      = "Player",
        .Size      = 0,
        .Alignment = 0};

    YULDUZ_Type player_type = YULDUZ_INVALID_TYPE;
    TEST_ASSERT(YULDUZ_RegisterTypeInECSRegistry(&registry, player_desc, &player_type),
                "Failed to register Player tag");

    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_ecs_registry_get_types(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 32, 8, 16};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register types
    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Type registered_type;
    YULDUZ_RegisterTypeInECSRegistry(&registry, desc, &registered_type);

    // Get type by name
    YULDUZ_Type retrieved_type = YULDUZ_INVALID_TYPE;
    TEST_ASSERT(YULDUZ_GetTypeInECSRegistry(&registry, "Position", &retrieved_type),
                "Failed to get type by name");
    TEST_ASSERT(retrieved_type == registered_type, "Type mismatch");

    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_ecs_registry_get_type_info(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 32, 8, 16};
    YULDUZ_InitializeECSRegistry(&registry, info);

    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_RegisterTypeInECSRegistry(&registry, desc, nullptr);

    // Get type info
    YULDUZ_TypeInfo type_info = {0};
    TEST_ASSERT(YULDUZ_GetTypeInfoInECSRegistry(&registry, "Position", &type_info),
                "Failed to get type info");
    TEST_ASSERT(type_info.Size == sizeof(Position), "Size mismatch");
    TEST_ASSERT(type_info.Alignment == alignof(Position), "Alignment mismatch");

    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_ecs_registry_create_destroy_entity(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 32, 8, 16};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Create entity
    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT(YULDUZ_CreateEntityInECSRegistry(&registry, &entity),
                "Failed to create entity");
    TEST_ASSERT(entity != YULDUZ_INVALID_ENTITY, "Entity should be valid");

    // Destroy entity
    TEST_ASSERT(YULDUZ_DestroyEntityInECSRegistry(&registry, entity),
                "Failed to destroy entity");

    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_ecs_registry_add_component(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 32, 8, 16};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register type
    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};
    YULDUZ_RegisterTypeInECSRegistry(&registry, desc, nullptr);

    // Create entity
    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    // Add component
    Position pos = {1.0f, 2.0f, 3.0f};
    TEST_ASSERT(YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos),
                "Failed to add component");

    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_ecs_registry_get_set_component(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 32, 8, 16};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Setup
    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};
    YULDUZ_RegisterTypeInECSRegistry(&registry, desc, nullptr);

    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    Position pos = {1.0f, 2.0f, 3.0f};
    YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);

    // Get component
    Position retrieved_pos = {0};
    TEST_ASSERT(YULDUZ_GetComponentInECSRegistry(&registry, entity, "Position", &retrieved_pos),
                "Failed to get component");
    TEST_ASSERT(retrieved_pos.x == 1.0f && retrieved_pos.y == 2.0f && retrieved_pos.z == 3.0f,
                "Component data mismatch");

    // Set component
    Position new_pos = {10.0f, 20.0f, 30.0f};
    TEST_ASSERT(YULDUZ_SetComponentInECSRegistry(&registry, entity, "Position", &new_pos),
                "Failed to set component");

    // Verify update
    YULDUZ_GetComponentInECSRegistry(&registry, entity, "Position", &retrieved_pos);
    TEST_ASSERT(retrieved_pos.x == 10.0f && retrieved_pos.y == 20.0f && retrieved_pos.z == 30.0f,
                "Component not updated");

    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_ecs_registry_remove_component(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 32, 8, 16};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Setup
    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};
    YULDUZ_RegisterTypeInECSRegistry(&registry, desc, nullptr);

    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    Position pos = {1.0f, 2.0f, 3.0f};
    YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);

    // Remove component
    TEST_ASSERT(YULDUZ_RemoveComponentInECSRegistry(&registry, entity, "Position"),
                "Failed to remove component");

    // Verify removal
    Position retrieved = {0};
    TEST_ASSERT(!YULDUZ_GetComponentInECSRegistry(&registry, entity, "Position", &retrieved),
                "Component should not exist after removal");

    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_ecs_registry_add_remove_tag(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 32, 8, 16};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register tag
    YULDUZ_TypeDescription desc = {
        .Name      = "Player",
        .Size      = 0,
        .Alignment = 0};
    YULDUZ_RegisterTypeInECSRegistry(&registry, desc, nullptr);

    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    // Add tag
    TEST_ASSERT(YULDUZ_AddTagInECSRegistry(&registry, entity, "Player"),
                "Failed to add tag");

    // Check tag
    TEST_ASSERT(YULDUZ_HasTagInECSRegistry(&registry, entity, "Player"),
                "Entity should have Player tag");

    // Remove tag
    TEST_ASSERT(YULDUZ_RemoveTagInECSRegistry(&registry, entity, "Player"),
                "Failed to remove tag");

    // Verify removal
    TEST_ASSERT(!YULDUZ_HasTagInECSRegistry(&registry, entity, "Player"),
                "Entity should not have Player tag after removal");

    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_ecs_registry_multiple_components(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 32, 8, 16};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register types
    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)},
        {.Name = "Health", .Size = sizeof(Health), .Alignment = alignof(Health)}};

    for (uint32_t i = 0; i < 3; i++) {
        YULDUZ_RegisterTypeInECSRegistry(&registry, descs[i], nullptr);
    }

    // Create entity
    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    // Add all components
    Position pos    = {1.0f, 2.0f, 3.0f};
    Velocity vel    = {10.0f, 20.0f, 30.0f};
    Health   health = {100.0f, 100.0f};

    YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
    YULDUZ_AddComponentInECSRegistry(&registry, entity, "Velocity", &vel);
    YULDUZ_AddComponentInECSRegistry(&registry, entity, "Health", &health);

    // Verify all components
    Position ret_pos    = {0};
    Velocity ret_vel    = {0};
    Health   ret_health = {0};

    TEST_ASSERT(YULDUZ_GetComponentInECSRegistry(&registry, entity, "Position", &ret_pos),
                "Failed to get Position");
    TEST_ASSERT(YULDUZ_GetComponentInECSRegistry(&registry, entity, "Velocity", &ret_vel),
                "Failed to get Velocity");
    TEST_ASSERT(YULDUZ_GetComponentInECSRegistry(&registry, entity, "Health", &ret_health),
                "Failed to get Health");

    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_ecs_registry_archetype_transitions(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 32, 8, 16};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register types
    YULDUZ_TypeDescription pos_desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};
    YULDUZ_RegisterTypeInECSRegistry(&registry, pos_desc, nullptr);

    // Create entity in null archetype
    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    uint32_t initial_archetype_count = registry.DenseCount;

    // Add component - should create new archetype
    Position pos = {1.0f, 2.0f, 3.0f};
    YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);

    TEST_ASSERT(registry.DenseCount > initial_archetype_count,
                "Should have created new archetype");

    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_ecs_registry_get_archetype_count(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 32, 8, 16};
    YULDUZ_InitializeECSRegistry(&registry, info);

    uint32_t count = 0;
    TEST_ASSERT(YULDUZ_GetArchetypeCountInECSRegistry(&registry, &count),
                "Failed to get archetype count");
    TEST_ASSERT(count >= 1, "Should have at least null archetype");

    // Register type and add component to create new archetype
    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};
    YULDUZ_RegisterTypeInECSRegistry(&registry, desc, nullptr);

    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
    Position pos = {0};
    YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);

    uint32_t new_count = 0;
    YULDUZ_GetArchetypeCountInECSRegistry(&registry, &new_count);
    TEST_ASSERT(new_count > count, "Archetype count should have increased");

    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_ecs_registry_component_with_type(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 32, 8, 16};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register type
    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};
    YULDUZ_Type type;
    YULDUZ_RegisterTypeInECSRegistry(&registry, desc, &type);

    // Create entity and add component using type
    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    Position pos = {5.0f, 10.0f, 15.0f};
    TEST_ASSERT(YULDUZ_AddComponentWithTypeInECSRegistry(&registry, entity, type, &pos),
                "Failed to add component with type");

    // Get component using type
    Position retrieved = {0};
    TEST_ASSERT(YULDUZ_GetComponentWithTypeInECSRegistry(&registry, entity, type, &retrieved),
                "Failed to get component with type");
    TEST_ASSERT(retrieved.x == 5.0f && retrieved.y == 10.0f && retrieved.z == 15.0f,
                "Component data mismatch");

    // Set component using type
    Position new_pos = {50.0f, 100.0f, 150.0f};
    TEST_ASSERT(YULDUZ_SetComponentWithTypeInECSRegistry(&registry, entity, type, &new_pos),
                "Failed to set component with type");

    // Remove component using type
    TEST_ASSERT(YULDUZ_RemoveComponentWithTypeInECSRegistry(&registry, entity, type),
                "Failed to remove component with type");

    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_ecs_registry_tag_with_type(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 32, 8, 16};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register tag
    YULDUZ_TypeDescription desc = {
        .Name      = "Player",
        .Size      = 0,
        .Alignment = 0};
    YULDUZ_Type type;
    YULDUZ_RegisterTypeInECSRegistry(&registry, desc, &type);

    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

    // Add tag with type
    TEST_ASSERT(YULDUZ_AddTagWithTypeInECSRegistry(&registry, entity, type),
                "Failed to add tag with type");

    // Check tag with type
    TEST_ASSERT(YULDUZ_HasTagWithTypeInECSRegistry(&registry, entity, type),
                "Entity should have tag");

    // Remove tag with type
    TEST_ASSERT(YULDUZ_RemoveTagWithTypeInECSRegistry(&registry, entity, type),
                "Failed to remove tag with type");

    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_ecs_registry_multiple_entities_same_archetype(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 32, 8, 16};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register type
    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};
    YULDUZ_RegisterTypeInECSRegistry(&registry, desc, nullptr);

    // Create multiple entities with same component
    YULDUZ_Entity entities[10];
    for (uint32_t i = 0; i < 10; i++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &entities[i]);
        Position pos = {(float)i, (float)i * 2, (float)i * 3};
        YULDUZ_AddComponentInECSRegistry(&registry, entities[i], "Position", &pos);
    }

    // Verify all entities
    for (uint32_t i = 0; i < 10; i++) {
        Position retrieved = {0};
        TEST_ASSERT(YULDUZ_GetComponentInECSRegistry(&registry, entities[i], "Position", &retrieved),
                    "Failed to get component");
        TEST_ASSERT(retrieved.x == (float)i, "Component data mismatch");
    }

    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

bool test_ecs_registry_complex_scenario(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 64, 16, 32};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register types
    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)},
        {.Name = "Health", .Size = sizeof(Health), .Alignment = alignof(Health)},
        {.Name = "Player", .Size = 0, .Alignment = 0},
        {.Name = "Enemy", .Size = 0, .Alignment = 0}};

    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_RegisterTypeInECSRegistry(&registry, descs[i], nullptr);
    }

    // Create player entity
    YULDUZ_Entity player;
    YULDUZ_CreateEntityInECSRegistry(&registry, &player);

    Position player_pos    = {0.0f, 0.0f, 0.0f};
    Velocity player_vel    = {1.0f, 0.0f, 0.0f};
    Health   player_health = {100.0f, 100.0f};

    YULDUZ_AddComponentInECSRegistry(&registry, player, "Position", &player_pos);
    YULDUZ_AddComponentInECSRegistry(&registry, player, "Velocity", &player_vel);
    YULDUZ_AddComponentInECSRegistry(&registry, player, "Health", &player_health);
    YULDUZ_AddTagInECSRegistry(&registry, player, "Player");

    // Create enemies
    YULDUZ_Entity enemies[5];
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &enemies[i]);

        Position pos    = {(float)(i * 10), 0.0f, 0.0f};
        Health   health = {50.0f, 50.0f};

        YULDUZ_AddComponentInECSRegistry(&registry, enemies[i], "Position", &pos);
        YULDUZ_AddComponentInECSRegistry(&registry, enemies[i], "Health", &health);
        YULDUZ_AddTagInECSRegistry(&registry, enemies[i], "Enemy");
    }

    // Verify player
    TEST_ASSERT(YULDUZ_HasTagInECSRegistry(&registry, player, "Player"),
                "Player should have Player tag");

    // Verify enemies
    for (uint32_t i = 0; i < 5; i++) {
        TEST_ASSERT(YULDUZ_HasTagInECSRegistry(&registry, enemies[i], "Enemy"),
                    "Enemy should have Enemy tag");
    }

    // Destroy an enemy
    YULDUZ_DestroyEntityInECSRegistry(&registry, enemies[2]);

    // Verify destroyed enemy no longer exists
    Health check_health = {0};
    TEST_ASSERT(!YULDUZ_GetComponentInECSRegistry(&registry, enemies[2], "Health", &check_health),
                "Destroyed entity should not have components");

    YULDUZ_ReleaseECSRegistry(&registry);
    TEST_PASS();
}

// ============================================================================
// Test Runner
// ============================================================================

void run_all_ecs_registry_tests(void) {
    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("Running ECS Registry Tests");
    YULDUZ_LOG_INFO("========================================\n");

    uint32_t passed = 0;
    uint32_t total  = 0;

#define RUN_TEST(test)        \
    do {                      \
        total++;              \
        if (test()) passed++; \
    } while (0)

    RUN_TEST(test_ecs_registry_initialize_release);
    RUN_TEST(test_ecs_registry_register_types);
    RUN_TEST(test_ecs_registry_get_types);
    RUN_TEST(test_ecs_registry_get_type_info);
    RUN_TEST(test_ecs_registry_create_destroy_entity);
    RUN_TEST(test_ecs_registry_add_component);
    RUN_TEST(test_ecs_registry_get_set_component);
    RUN_TEST(test_ecs_registry_remove_component);
    RUN_TEST(test_ecs_registry_add_remove_tag);
    RUN_TEST(test_ecs_registry_multiple_components);
    RUN_TEST(test_ecs_registry_archetype_transitions);
    RUN_TEST(test_ecs_registry_get_archetype_count);
    RUN_TEST(test_ecs_registry_component_with_type);
    RUN_TEST(test_ecs_registry_tag_with_type);
    RUN_TEST(test_ecs_registry_multiple_entities_same_archetype);
    RUN_TEST(test_ecs_registry_complex_scenario);

#undef RUN_TEST

    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("ECS Registry Tests: %u/%u passed", passed, total);
    YULDUZ_LOG_INFO("========================================\n");
}