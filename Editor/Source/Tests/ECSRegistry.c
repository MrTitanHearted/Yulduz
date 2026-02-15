#include <Tests/Tests.h>

// ============================================================================
// ECS REGISTRY TESTS
// ============================================================================

void run_all_ecs_registry_tests(void) {
    YULDUZ_LOG_INFO("\n");
    YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
    YULDUZ_LOG_INFO("║           ECS REGISTRY TESTS                                   ║");
    YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");
    YULDUZ_LOG_INFO("\n");

    test_ecs_null_archetype();
    test_ecs_entity_lifecycle();
    test_ecs_add_remove_components();
    test_ecs_add_remove_tags();
    test_ecs_set_get_components();
    test_ecs_archetype_transitions();
    test_ecs_archetype_creation();
    // test_ecs_query_archetypes();
    test_ecs_complex_scenarios();
}

void test_ecs_null_archetype(void) {
    TEST_START("ECS Null Archetype");

    YULDUZ_ECSRegistry ecs = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity = 8,
        .InitialEntityCapacity = 16,
        .InitialArchetypeCapacity = 8,
        .InitialArchetypeTypeCapacity = 8
    };

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS registry");

    // Register types
    setup_test_types(&ecs.TypeRegistry);

    // Null archetype should exist for entities with no components
    YULDUZ_ASSERT(ecs.NullArchetypeType != YULDUZ_INVALID_ARCHETYPE_TYPE,
                  "Null archetype type should be valid");

    // Create entity - should go into null archetype
    YULDUZ_Entity entity;
    YULDUZ_ASSERT(YULDUZ_CreateEntityInECSRegistry(&ecs, &entity),
                  "Failed to create entity");

    YULDUZ_ASSERT(entity != YULDUZ_INVALID_ENTITY, "Entity should be valid");

    // Entity should not have any components or tags
    YULDUZ_ASSERT(!YULDUZ_HasTagInECSRegistry(&ecs, entity, "Player"),
                  "New entity should not have Player tag");

    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("ECS Null Archetype");
}

void test_ecs_entity_lifecycle(void) {
    TEST_START("ECS Entity Lifecycle");

    YULDUZ_ECSRegistry ecs = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity = 8,
        .InitialEntityCapacity = 16,
        .InitialArchetypeCapacity = 8,
        .InitialArchetypeTypeCapacity = 8
    };

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    // Create entity
    YULDUZ_Entity entity;
    YULDUZ_ASSERT(YULDUZ_CreateEntityInECSRegistry(&ecs, &entity),
                  "Failed to create entity");

    YULDUZ_LOG_INFO("Created entity: %u", entity);

    // Destroy entity
    YULDUZ_ASSERT(YULDUZ_DestroyEntityInECSRegistry(&ecs, entity),
                  "Failed to destroy entity");

    // Try to operate on destroyed entity - should fail
    Position pos = {1.0f, 2.0f, 3.0f};
    bool result = YULDUZ_AddComponentInECSRegistry(&ecs, entity, "Position", &pos);
    YULDUZ_ASSERT(!result, "Should not be able to add component to destroyed entity");

    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("ECS Entity Lifecycle");
}

void test_ecs_add_remove_components(void) {
    TEST_START("ECS Add/Remove Components");

    YULDUZ_ECSRegistry ecs = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity = 8,
        .InitialEntityCapacity = 16,
        .InitialArchetypeCapacity = 8,
        .InitialArchetypeTypeCapacity = 8
    };

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    YULDUZ_Entity entity;
    YULDUZ_ASSERT(YULDUZ_CreateEntityInECSRegistry(&ecs, &entity),
                  "Failed to create entity");

    // Add Position component
    Position pos = {10.0f, 20.0f, 30.0f};
    YULDUZ_ASSERT(YULDUZ_AddComponentInECSRegistry(&ecs, entity, "Position", &pos),
                  "Failed to add Position component");

    // Verify component was added
    Position retrieved_pos;
    YULDUZ_ASSERT(YULDUZ_GetComponentInECSRegistry(&ecs, entity, "Position", &retrieved_pos),
                  "Failed to get Position component");

    YULDUZ_ASSERT(retrieved_pos.x == 10.0f && retrieved_pos.y == 20.0f && retrieved_pos.z == 30.0f,
                  "Position data should match");

    // Add Velocity component
    Velocity vel = {1.0f, 2.0f, 3.0f};
    YULDUZ_ASSERT(YULDUZ_AddComponentInECSRegistry(&ecs, entity, "Velocity", &vel),
                  "Failed to add Velocity component");

    // Verify both components exist
    Position check_pos;
    Velocity check_vel;
    YULDUZ_ASSERT(YULDUZ_GetComponentInECSRegistry(&ecs, entity, "Position", &check_pos),
                  "Position should still exist");
    YULDUZ_ASSERT(YULDUZ_GetComponentInECSRegistry(&ecs, entity, "Velocity", &check_vel),
                  "Velocity should exist");

    YULDUZ_ASSERT(check_pos.x == 10.0f, "Position should be preserved");
    YULDUZ_ASSERT(check_vel.x == 1.0f, "Velocity should match");

    // Remove Position
    YULDUZ_ASSERT(YULDUZ_RemoveComponentInECSRegistry(&ecs, entity, "Position"),
                  "Failed to remove Position");

    // Velocity should still exist, Position should not
    YULDUZ_ASSERT(YULDUZ_GetComponentInECSRegistry(&ecs, entity, "Velocity", &check_vel),
                  "Velocity should still exist after removing Position");
    YULDUZ_ASSERT(!YULDUZ_GetComponentInECSRegistry(&ecs, entity, "Position", &check_pos),
                  "Position should not exist after removal");

    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("ECS Add/Remove Components");
}

void test_ecs_add_remove_tags(void) {
    TEST_START("ECS Add/Remove Tags");

    YULDUZ_ECSRegistry ecs = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity = 8,
        .InitialEntityCapacity = 16,
        .InitialArchetypeCapacity = 8,
        .InitialArchetypeTypeCapacity = 8
    };

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    YULDUZ_Entity entity;
    YULDUZ_ASSERT(YULDUZ_CreateEntityInECSRegistry(&ecs, &entity),
                  "Failed to create entity");

    // Add Player tag
    YULDUZ_ASSERT(YULDUZ_AddTagInECSRegistry(&ecs, entity, "Player"),
                  "Failed to add Player tag");

    YULDUZ_ASSERT(YULDUZ_HasTagInECSRegistry(&ecs, entity, "Player"),
                  "Entity should have Player tag");

    // Add Enemy tag
    YULDUZ_ASSERT(YULDUZ_AddTagInECSRegistry(&ecs, entity, "Enemy"),
                  "Failed to add Enemy tag");

    YULDUZ_ASSERT(YULDUZ_HasTagInECSRegistry(&ecs, entity, "Player"),
                  "Player tag should still exist");
    YULDUZ_ASSERT(YULDUZ_HasTagInECSRegistry(&ecs, entity, "Enemy"),
                  "Enemy tag should exist");

    // Remove Player tag
    YULDUZ_ASSERT(YULDUZ_RemoveTagInECSRegistry(&ecs, entity, "Player"),
                  "Failed to remove Player tag");

    YULDUZ_ASSERT(!YULDUZ_HasTagInECSRegistry(&ecs, entity, "Player"),
                  "Player tag should not exist after removal");
    YULDUZ_ASSERT(YULDUZ_HasTagInECSRegistry(&ecs, entity, "Enemy"),
                  "Enemy tag should still exist");

    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("ECS Add/Remove Tags");
}

void test_ecs_set_get_components(void) {
    TEST_START("ECS Set/Get Components");

    YULDUZ_ECSRegistry ecs = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity = 8,
        .InitialEntityCapacity = 16,
        .InitialArchetypeCapacity = 8,
        .InitialArchetypeTypeCapacity = 8
    };

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    YULDUZ_Entity entity;
    YULDUZ_ASSERT(YULDUZ_CreateEntityInECSRegistry(&ecs, &entity),
                  "Failed to create entity");

    // Add Position component
    Position pos = {1.0f, 2.0f, 3.0f};
    YULDUZ_ASSERT(YULDUZ_AddComponentInECSRegistry(&ecs, entity, "Position", &pos),
                  "Failed to add Position");

    // Update Position using Set (doesn't change archetype)
    Position new_pos = {10.0f, 20.0f, 30.0f};
    YULDUZ_ASSERT(YULDUZ_SetComponentInECSRegistry(&ecs, entity, "Position", &new_pos),
                  "Failed to set Position");

    // Get updated Position
    Position retrieved_pos;
    YULDUZ_ASSERT(YULDUZ_GetComponentInECSRegistry(&ecs, entity, "Position", &retrieved_pos),
                  "Failed to get Position");

    YULDUZ_ASSERT(retrieved_pos.x == 10.0f && retrieved_pos.y == 20.0f && retrieved_pos.z == 30.0f,
                  "Position should be updated");

    // Try to set component that doesn't exist - should fail
    Health health = {100.0f, 100.0f};
    bool result = YULDUZ_SetComponentInECSRegistry(&ecs, entity, "Health", &health);
    YULDUZ_ASSERT(!result, "Setting non-existent component should fail");

    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("ECS Set/Get Components");
}

void test_ecs_archetype_transitions(void) {
    TEST_START("ECS Archetype Transitions");

    YULDUZ_ECSRegistry ecs = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity = 8,
        .InitialEntityCapacity = 16,
        .InitialArchetypeCapacity = 8,
        .InitialArchetypeTypeCapacity = 8
    };

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    YULDUZ_Entity entity;
    YULDUZ_ASSERT(YULDUZ_CreateEntityInECSRegistry(&ecs, &entity),
                  "Failed to create entity");

    // Entity starts in null archetype (no components)
    // Add Position -> moves to archetype [Position]
    Position pos = {5.0f, 10.0f, 15.0f};
    YULDUZ_ASSERT(YULDUZ_AddComponentInECSRegistry(&ecs, entity, "Position", &pos),
                  "Failed to add Position");

    // Add Velocity -> moves to archetype [Position, Velocity]
    Velocity vel = {1.0f, 1.0f, 1.0f};
    YULDUZ_ASSERT(YULDUZ_AddComponentInECSRegistry(&ecs, entity, "Velocity", &vel),
                  "Failed to add Velocity");

    // Add Health -> moves to archetype [Position, Velocity, Health]
    Health health = {100.0f, 100.0f};
    YULDUZ_ASSERT(YULDUZ_AddComponentInECSRegistry(&ecs, entity, "Health", &health),
                  "Failed to add Health");

    // Verify all components exist
    Position check_pos;
    Velocity check_vel;
    Health check_health;

    YULDUZ_ASSERT(YULDUZ_GetComponentInECSRegistry(&ecs, entity, "Position", &check_pos),
                  "Position should exist");
    YULDUZ_ASSERT(YULDUZ_GetComponentInECSRegistry(&ecs, entity, "Velocity", &check_vel),
                  "Velocity should exist");
    YULDUZ_ASSERT(YULDUZ_GetComponentInECSRegistry(&ecs, entity, "Health", &check_health),
                  "Health should exist");

    YULDUZ_ASSERT(check_pos.x == 5.0f, "Position data should be preserved");
    YULDUZ_ASSERT(check_vel.x == 1.0f, "Velocity data should be preserved");
    YULDUZ_ASSERT(check_health.current == 100.0f, "Health data should match");

    // Remove Velocity -> moves to archetype [Position, Health]
    YULDUZ_ASSERT(YULDUZ_RemoveComponentInECSRegistry(&ecs, entity, "Velocity"),
                  "Failed to remove Velocity");

    // Verify Position and Health still exist, Velocity is gone
    YULDUZ_ASSERT(YULDUZ_GetComponentInECSRegistry(&ecs, entity, "Position", &check_pos),
                  "Position should still exist");
    YULDUZ_ASSERT(YULDUZ_GetComponentInECSRegistry(&ecs, entity, "Health", &check_health),
                  "Health should still exist");
    YULDUZ_ASSERT(!YULDUZ_GetComponentInECSRegistry(&ecs, entity, "Velocity", &check_vel),
                  "Velocity should not exist");

    YULDUZ_ASSERT(check_pos.x == 5.0f, "Position data should be preserved across transition");
    YULDUZ_ASSERT(check_health.current == 100.0f, "Health data should be preserved");

    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("ECS Archetype Transitions");
}

void test_ecs_archetype_creation(void) {
    TEST_START("ECS Archetype Creation and Reuse");

    YULDUZ_ECSRegistry ecs = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity = 8,
        .InitialEntityCapacity = 16,
        .InitialArchetypeCapacity = 4,
        .InitialArchetypeTypeCapacity = 4
    };

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    uint32_t initial_archetype_count = ecs.DenseCount;

    // Create entity with Position + Velocity
    YULDUZ_Entity e1;
    YULDUZ_ASSERT(YULDUZ_CreateEntityInECSRegistry(&ecs, &e1),
                  "Failed to create entity 1");

    Position pos1 = {1.0f, 2.0f, 3.0f};
    Velocity vel1 = {0.1f, 0.2f, 0.3f};

    YULDUZ_AddComponentInECSRegistry(&ecs, e1, "Position", &pos1);
    YULDUZ_AddComponentInECSRegistry(&ecs, e1, "Velocity", &vel1);

    uint32_t archetype_count_after_e1 = ecs.DenseCount;
    YULDUZ_LOG_INFO("Archetypes after e1: %u", archetype_count_after_e1);

    // Create another entity with same components - should reuse archetype
    YULDUZ_Entity e2;
    YULDUZ_ASSERT(YULDUZ_CreateEntityInECSRegistry(&ecs, &e2),
                  "Failed to create entity 2");

    Position pos2 = {10.0f, 20.0f, 30.0f};
    Velocity vel2 = {1.0f, 2.0f, 3.0f};

    YULDUZ_AddComponentInECSRegistry(&ecs, e2, "Position", &pos2);
    YULDUZ_AddComponentInECSRegistry(&ecs, e2, "Velocity", &vel2);

    uint32_t archetype_count_after_e2 = ecs.DenseCount;
    YULDUZ_LOG_INFO("Archetypes after e2: %u", archetype_count_after_e2);

    YULDUZ_ASSERT(archetype_count_after_e2 == archetype_count_after_e1,
                  "Second entity with same components should not create new archetype");

    // Create entity with different components - should create new archetype
    YULDUZ_Entity e3;
    YULDUZ_ASSERT(YULDUZ_CreateEntityInECSRegistry(&ecs, &e3),
                  "Failed to create entity 3");

    Health health = {50.0f, 100.0f};
    YULDUZ_AddComponentInECSRegistry(&ecs, e3, "Health", &health);

    uint32_t archetype_count_after_e3 = ecs.DenseCount;
    YULDUZ_LOG_INFO("Archetypes after e3: %u", archetype_count_after_e3);

    YULDUZ_ASSERT(archetype_count_after_e3 > archetype_count_after_e2,
                  "Entity with different components should create new archetype");

    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("ECS Archetype Creation and Reuse");
}

/*
void test_ecs_query_archetypes(void) {
    TEST_START("ECS Query Archetypes");

    YULDUZ_ECSRegistry ecs = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity = 8,
        .InitialEntityCapacity = 16,
        .InitialArchetypeCapacity = 8,
        .InitialArchetypeTypeCapacity = 8
    };

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    // Create entities with different component combinations
    YULDUZ_Entity e1, e2, e3;

    // Entity 1: Position
    YULDUZ_CreateEntityInECSRegistry(&ecs, &e1);
    Position pos1 = {1.0f, 2.0f, 3.0f};
    YULDUZ_AddComponentInECSRegistry(&ecs, e1, "Position", &pos1);

    // Entity 2: Position + Velocity
    YULDUZ_CreateEntityInECSRegistry(&ecs, &e2);
    Position pos2 = {10.0f, 20.0f, 30.0f};
    Velocity vel2 = {1.0f, 2.0f, 3.0f};
    YULDUZ_AddComponentInECSRegistry(&ecs, e2, "Position", &pos2);
    YULDUZ_AddComponentInECSRegistry(&ecs, e2, "Velocity", &vel2);

    // Entity 3: Position + Velocity + Health
    YULDUZ_CreateEntityInECSRegistry(&ecs, &e3);
    Position pos3 = {100.0f, 200.0f, 300.0f};
    Velocity vel3 = {10.0f, 20.0f, 30.0f};
    Health health3 = {100.0f, 100.0f};
    YULDUZ_AddComponentInECSRegistry(&ecs, e3, "Position", &pos3);
    YULDUZ_AddComponentInECSRegistry(&ecs, e3, "Velocity", &vel3);
    YULDUZ_AddComponentInECSRegistry(&ecs, e3, "Health", &health3);

    // Query for archetypes with Position
    YULDUZ_Type position_types[] = {g_position_type};
    YULDUZ_QueryInfo query1 = {
        .WithComponentTypes = position_types,
        .WithTagTypes = nullptr,
        .WithComponentCount = 1,
        .WithTagCount = 0
    };

    uint32_t count1 = 0;
    YULDUZ_ArchetypeType *results1 = YULDUZ_QueryArchetypesInECSRegistry(&ecs, &query1, &count1);
    YULDUZ_ASSERT(results1 != nullptr, "Query should succeed");
    YULDUZ_ASSERT(count1 >= 3, "Should find at least 3 archetypes with Position");
    YULDUZ_LOG_INFO("Found %u archetypes with Position", count1);
    SDL_free(results1);

    // Query for archetypes with Position + Velocity
    YULDUZ_Type pos_vel_types[] = {g_position_type, g_velocity_type};
    YULDUZ_QueryInfo query2 = {
        .WithComponentTypes = pos_vel_types,
        .WithTagTypes = nullptr,
        .WithComponentCount = 2,
        .WithTagCount = 0
    };

    uint32_t count2 = 0;
    YULDUZ_ArchetypeType *results2 = YULDUZ_QueryArchetypesInECSRegistry(&ecs, &query2, &count2);
    YULDUZ_ASSERT(results2 != nullptr, "Query should succeed");
    YULDUZ_ASSERT(count2 >= 2, "Should find at least 2 archetypes with Position + Velocity");
    YULDUZ_LOG_INFO("Found %u archetypes with Position + Velocity", count2);
    SDL_free(results2);

    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("ECS Query Archetypes");
}*/

void test_ecs_complex_scenarios(void) {
    TEST_START("ECS Complex Scenarios");

    YULDUZ_ECSRegistry ecs = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity = 16,
        .InitialEntityCapacity = 32,
        .InitialArchetypeCapacity = 16,
        .InitialArchetypeTypeCapacity = 16
    };

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    // Scenario: Create multiple entities, add/remove components, verify data integrity
    const uint32_t entity_count = 10;
    YULDUZ_Entity entities[entity_count];

    // Create entities
    for (uint32_t i = 0; i < entity_count; i++) {
        YULDUZ_ASSERT(YULDUZ_CreateEntityInECSRegistry(&ecs, &entities[i]),
                      "Failed to create entity %u", i);
    }

    // Add Position to all
    for (uint32_t i = 0; i < entity_count; i++) {
        Position pos = {(float)i, (float)(i * 10), (float)(i * 100)};
        YULDUZ_AddComponentInECSRegistry(&ecs, entities[i], "Position", &pos);
    }

    // Add Velocity to even entities, Health to odd entities
    for (uint32_t i = 0; i < entity_count; i++) {
        if (i % 2 == 0) {
            Velocity vel = {(float)i * 0.1f, (float)i * 0.2f, (float)i * 0.3f};
            YULDUZ_AddComponentInECSRegistry(&ecs, entities[i], "Velocity", &vel);
        } else {
            Health health = {(float)(100 - i), 100.0f};
            YULDUZ_AddComponentInECSRegistry(&ecs, entities[i], "Health", &health);
        }
    }

    // Add tags to entities 0-4
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_AddTagInECSRegistry(&ecs, entities[i], "Player");
    }

    // Verify all data
    for (uint32_t i = 0; i < entity_count; i++) {
        Position pos;
        YULDUZ_ASSERT(YULDUZ_GetComponentInECSRegistry(&ecs, entities[i], "Position", &pos),
                      "Entity %u should have Position", i);
        YULDUZ_ASSERT(pos.x == (float)i, "Position.x should match for entity %u", i);

        if (i % 2 == 0) {
            Velocity vel;
            YULDUZ_ASSERT(YULDUZ_GetComponentInECSRegistry(&ecs, entities[i], "Velocity", &vel),
                          "Even entity %u should have Velocity", i);
        } else {
            Health health;
            YULDUZ_ASSERT(YULDUZ_GetComponentInECSRegistry(&ecs, entities[i], "Health", &health),
                          "Odd entity %u should have Health", i);
        }

        if (i < 5) {
            YULDUZ_ASSERT(YULDUZ_HasTagInECSRegistry(&ecs, entities[i], "Player"),
                          "Entity %u should have Player tag", i);
        }
    }

    // Remove some components and verify
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_RemoveComponentInECSRegistry(&ecs, entities[i], "Position");
    }

    for (uint32_t i = 0; i < 5; i++) {
        Position pos;
        YULDUZ_ASSERT(!YULDUZ_GetComponentInECSRegistry(&ecs, entities[i], "Position", &pos),
                      "Entity %u should not have Position after removal", i);
    }

    YULDUZ_LOG_INFO("Complex scenario completed successfully");
    YULDUZ_LOG_INFO("Total archetypes created: %u", ecs.DenseCount);

    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("ECS Complex Scenarios");
}