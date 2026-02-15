#include <Tests/Tests.h>

// ============================================================================
// GLOBAL TYPE VARIABLES - Implementation
// ============================================================================

YULDUZ_Type g_position_type  = YULDUZ_INVALID_COMPONENT_TYPE;
YULDUZ_Type g_velocity_type  = YULDUZ_INVALID_COMPONENT_TYPE;
YULDUZ_Type g_health_type    = YULDUZ_INVALID_COMPONENT_TYPE;
YULDUZ_Type g_damage_type    = YULDUZ_INVALID_COMPONENT_TYPE;
YULDUZ_Type g_name_type      = YULDUZ_INVALID_COMPONENT_TYPE;
YULDUZ_Type g_sprite_type    = YULDUZ_INVALID_COMPONENT_TYPE;
YULDUZ_Type g_transform_type = YULDUZ_INVALID_COMPONENT_TYPE;
YULDUZ_Type g_tag_player     = YULDUZ_INVALID_COMPONENT_TYPE;
YULDUZ_Type g_tag_enemy      = YULDUZ_INVALID_COMPONENT_TYPE;
YULDUZ_Type g_tag_dead       = YULDUZ_INVALID_COMPONENT_TYPE;
YULDUZ_Type g_tag_active     = YULDUZ_INVALID_COMPONENT_TYPE;
YULDUZ_Type g_tag_inactive   = YULDUZ_INVALID_COMPONENT_TYPE;

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void setup_test_types(YULDUZ_TypeRegistry *registry) {
    // Register component types
    YULDUZ_TypeDescription descriptions[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = _Alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = _Alignof(Velocity)},
        {.Name = "Health", .Size = sizeof(Health), .Alignment = _Alignof(Health)},
        {.Name = "Damage", .Size = sizeof(Damage), .Alignment = _Alignof(Damage)},
        {.Name = "Name", .Size = sizeof(Name), .Alignment = _Alignof(Name)},
        {.Name = "Sprite", .Size = sizeof(Sprite), .Alignment = _Alignof(Sprite)},
        {.Name = "Transform", .Size = sizeof(Transform), .Alignment = _Alignof(Transform)}};

    YULDUZ_Type types[7];
    YULDUZ_ASSERT(YULDUZ_RegisterTypesInTypeRegistry(registry, descriptions, types, 7),
                  "Failed to register component types");

    g_position_type  = types[0];
    g_velocity_type  = types[1];
    g_health_type    = types[2];
    g_damage_type    = types[3];
    g_name_type      = types[4];
    g_sprite_type    = types[5];
    g_transform_type = types[6];

    // Register tag types (zero-size)
    YULDUZ_TypeDescription tag_descriptions[] = {
        {.Name = "Player", .Size = 0, .Alignment = 1},
        {.Name = "Enemy", .Size = 0, .Alignment = 1},
        {.Name = "Dead", .Size = 0, .Alignment = 1},
        {.Name = "Active", .Size = 0, .Alignment = 1},
        {.Name = "Inactive", .Size = 0, .Alignment = 1}};

    YULDUZ_Type tag_types[5];
    YULDUZ_ASSERT(YULDUZ_RegisterTypesInTypeRegistry(registry, tag_descriptions, tag_types, 5),
                  "Failed to register tag types");

    g_tag_player   = tag_types[0];
    g_tag_enemy    = tag_types[1];
    g_tag_dead     = tag_types[2];
    g_tag_active   = tag_types[3];
    g_tag_inactive = tag_types[4];
}

void cleanup_test_types(void) {
    // Reset to invalid
    g_position_type  = YULDUZ_INVALID_COMPONENT_TYPE;
    g_velocity_type  = YULDUZ_INVALID_COMPONENT_TYPE;
    g_health_type    = YULDUZ_INVALID_COMPONENT_TYPE;
    g_damage_type    = YULDUZ_INVALID_COMPONENT_TYPE;
    g_name_type      = YULDUZ_INVALID_COMPONENT_TYPE;
    g_sprite_type    = YULDUZ_INVALID_COMPONENT_TYPE;
    g_transform_type = YULDUZ_INVALID_COMPONENT_TYPE;
    g_tag_player     = YULDUZ_INVALID_COMPONENT_TYPE;
    g_tag_enemy      = YULDUZ_INVALID_COMPONENT_TYPE;
    g_tag_dead       = YULDUZ_INVALID_COMPONENT_TYPE;
    g_tag_active     = YULDUZ_INVALID_COMPONENT_TYPE;
    g_tag_inactive   = YULDUZ_INVALID_COMPONENT_TYPE;
}

// ============================================================================
// STRESS TESTS
// ============================================================================

void run_all_stress_tests(void) {
    YULDUZ_LOG_INFO("\n");
    YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
    YULDUZ_LOG_INFO("║           STRESS TESTS                                         ║");
    YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");
    YULDUZ_LOG_INFO("\n");

    stress_test_many_entities();
    stress_test_many_archetypes();
    stress_test_component_churn();
    stress_test_archetype_transitions();
    stress_test_random_operations();
}

void stress_test_many_entities(void) {
    TEST_START("Stress: Many Entities");

    const uint32_t entity_count = 100000;

    YULDUZ_ECSRegistry               ecs  = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity          = 8,
        .InitialEntityCapacity        = 1024,
        .InitialArchetypeCapacity     = 32,
        .InitialArchetypeTypeCapacity = 32};

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    YULDUZ_LOG_INFO("Creating %u entities...", entity_count);

    clock_t start = clock();

    for (uint32_t i = 0; i < entity_count; i++) {
        YULDUZ_Entity e;
        YULDUZ_ASSERT(YULDUZ_CreateEntityInECSRegistry(&ecs, &e),
                      "Failed to create entity %u", i);

        Position pos = {(float)(i % 1000), (float)(i / 1000), 0.0f};
        YULDUZ_AddComponentInECSRegistry(&ecs, e, "Position", &pos);

        if (i % 2 == 0) {
            Velocity vel = {1.0f, 1.0f, 1.0f};
            YULDUZ_AddComponentInECSRegistry(&ecs, e, "Velocity", &vel);
        }

        if (i % 1000 == 0) {
            YULDUZ_LOG_INFO("Progress: %u%%", (i * 100) / entity_count);
        }
    }

    clock_t end        = clock();
    double  time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

    YULDUZ_LOG_INFO("Created %u entities in %.3f seconds", entity_count, time_taken);
    YULDUZ_LOG_INFO("Rate: %.2f entities/sec", entity_count / time_taken);
    YULDUZ_LOG_INFO("Archetypes created: %u", ecs.DenseCount);

    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("Stress: Many Entities");
}

void stress_test_many_archetypes(void) {
    TEST_START("Stress: Many Archetypes");

    const uint32_t archetype_count = 500;

    YULDUZ_ECSRegistry               ecs  = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity          = 16,
        .InitialEntityCapacity        = archetype_count * 2,
        .InitialArchetypeCapacity     = 128,
        .InitialArchetypeTypeCapacity = 128};

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    YULDUZ_LOG_INFO("Creating %u different archetypes...", archetype_count);

    // Create entities with unique component combinations
    for (uint32_t i = 0; i < archetype_count; i++) {
        YULDUZ_Entity e;
        YULDUZ_ASSERT(YULDUZ_CreateEntityInECSRegistry(&ecs, &e),
                      "Failed to create entity %u", i);

        // Create unique component combinations based on i
        if (i & 1) {
            Position pos = {(float)i, 0.0f, 0.0f};
            YULDUZ_AddComponentInECSRegistry(&ecs, e, "Position", &pos);
        }

        if (i & 2) {
            Velocity vel = {1.0f, 1.0f, 1.0f};
            YULDUZ_AddComponentInECSRegistry(&ecs, e, "Velocity", &vel);
        }

        if (i & 4) {
            Health health = {100.0f, 100.0f};
            YULDUZ_AddComponentInECSRegistry(&ecs, e, "Health", &health);
        }

        if (i & 8) {
            Damage damage = {25.0f};
            YULDUZ_AddComponentInECSRegistry(&ecs, e, "Damage", &damage);
        }

        if (i & 16) {
            YULDUZ_AddTagInECSRegistry(&ecs, e, "Player");
        }

        if (i & 32) {
            YULDUZ_AddTagInECSRegistry(&ecs, e, "Enemy");
        }
    }

    YULDUZ_LOG_INFO("Created %u archetypes", ecs.DenseCount);
    YULDUZ_ASSERT(ecs.DenseCount > 50, "Should have created many different archetypes");

    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("Stress: Many Archetypes");
}

void stress_test_component_churn(void) {
    TEST_START("Stress: Component Churn");

    const uint32_t entity_count = 1000;
    const uint32_t operations   = 10000;

    YULDUZ_ECSRegistry               ecs  = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity          = 8,
        .InitialEntityCapacity        = entity_count,
        .InitialArchetypeCapacity     = 64,
        .InitialArchetypeTypeCapacity = 64};

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    // Create entities
    YULDUZ_Entity *entities = SDL_malloc(sizeof(YULDUZ_Entity) * entity_count);
    for (uint32_t i = 0; i < entity_count; i++) {
        YULDUZ_ASSERT(YULDUZ_CreateEntityInECSRegistry(&ecs, &entities[i]),
                      "Failed to create entity %u", i);
    }

    YULDUZ_LOG_INFO("Performing %u component add/remove operations...", operations);

    srand((unsigned)time(nullptr));

    for (uint32_t op = 0; op < operations; op++) {
        YULDUZ_Entity e      = entities[rand() % entity_count];
        int           choice = rand() % 6;

        switch (choice) {
            case 0: {
                Position pos = {(float)op, 0.0f, 0.0f};
                YULDUZ_AddComponentInECSRegistry(&ecs, e, "Position", &pos);
                break;
            }
            case 1: {
                Velocity vel = {1.0f, 1.0f, 1.0f};
                YULDUZ_AddComponentInECSRegistry(&ecs, e, "Velocity", &vel);
                break;
            }
            case 2: {
                Health health = {100.0f, 100.0f};
                YULDUZ_AddComponentInECSRegistry(&ecs, e, "Health", &health);
                break;
            }
            case 3:
                YULDUZ_RemoveComponentInECSRegistry(&ecs, e, "Position");
                break;
            case 4:
                YULDUZ_RemoveComponentInECSRegistry(&ecs, e, "Velocity");
                break;
            case 5:
                YULDUZ_RemoveComponentInECSRegistry(&ecs, e, "Health");
                break;
        }

        if (op % 1000 == 0) {
            YULDUZ_LOG_INFO("Progress: %u%%", (op * 100) / operations);
        }
    }

    YULDUZ_LOG_INFO("Final archetype count: %u", ecs.DenseCount);

    SDL_free(entities);
    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("Stress: Component Churn");
}

void stress_test_archetype_transitions(void) {
    TEST_START("Stress: Archetype Transitions");

    const uint32_t entity_count = 5000;

    YULDUZ_ECSRegistry               ecs  = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity          = 8,
        .InitialEntityCapacity        = entity_count,
        .InitialArchetypeCapacity     = 128,
        .InitialArchetypeTypeCapacity = 128};

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    YULDUZ_LOG_INFO("Creating and transitioning %u entities...", entity_count);

    YULDUZ_Entity *entities = SDL_malloc(sizeof(YULDUZ_Entity) * entity_count);

    // Create entities and transition through multiple archetypes
    for (uint32_t i = 0; i < entity_count; i++) {
        YULDUZ_ASSERT(YULDUZ_CreateEntityInECSRegistry(&ecs, &entities[i]),
                      "Failed to create entity %u", i);

        // Start: null archetype
        // Add Position: [Position]
        Position pos = {(float)i, 0.0f, 0.0f};
        YULDUZ_AddComponentInECSRegistry(&ecs, entities[i], "Position", &pos);

        // Add Velocity: [Position, Velocity]
        Velocity vel = {1.0f, 1.0f, 1.0f};
        YULDUZ_AddComponentInECSRegistry(&ecs, entities[i], "Velocity", &vel);

        // Add Health: [Position, Velocity, Health]
        Health health = {100.0f, 100.0f};
        YULDUZ_AddComponentInECSRegistry(&ecs, entities[i], "Health", &health);

        // Remove Position: [Velocity, Health]
        YULDUZ_RemoveComponentInECSRegistry(&ecs, entities[i], "Position");

        // Add Damage: [Velocity, Health, Damage]
        Damage damage = {25.0f};
        YULDUZ_AddComponentInECSRegistry(&ecs, entities[i], "Damage", &damage);

        // Remove Velocity: [Health, Damage]
        YULDUZ_RemoveComponentInECSRegistry(&ecs, entities[i], "Velocity");

        if (i % 500 == 0) {
            YULDUZ_LOG_INFO("Progress: %u%%", (i * 100) / entity_count);
        }
    }

    YULDUZ_LOG_INFO("Completed. Final archetype count: %u", ecs.DenseCount);

    // Verify final state
    for (uint32_t i = 0; i < entity_count; i++) {
        Health health;
        Damage damage;

        YULDUZ_ASSERT(YULDUZ_GetComponentInECSRegistry(&ecs, entities[i], "Health", &health),
                      "Entity should have Health");
        YULDUZ_ASSERT(YULDUZ_GetComponentInECSRegistry(&ecs, entities[i], "Damage", &damage),
                      "Entity should have Damage");
    }

    SDL_free(entities);
    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("Stress: Archetype Transitions");
}

void stress_test_random_operations(void) {
    TEST_START("Stress: Random Operations");

    const uint32_t max_entities = 5000;
    const uint32_t operations   = 50000;

    YULDUZ_ECSRegistry               ecs  = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity          = 8,
        .InitialEntityCapacity        = max_entities,
        .InitialArchetypeCapacity     = 128,
        .InitialArchetypeTypeCapacity = 128};

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    YULDUZ_Entity *active_entities = SDL_malloc(sizeof(YULDUZ_Entity) * max_entities);
    uint32_t       active_count    = 0;

    srand((unsigned)time(nullptr));

    YULDUZ_LOG_INFO("Performing %u random operations...", operations);

    for (uint32_t op = 0; op < operations; op++) {
        int choice = rand() % 10;

        if (choice < 3 && active_count < max_entities) {
            // Create entity
            YULDUZ_Entity e;
            YULDUZ_CreateEntityInECSRegistry(&ecs, &e);
            active_entities[active_count++] = e;
        } else if (choice < 4 && active_count > 0) {
            // Destroy entity
            uint32_t idx = rand() % active_count;
            YULDUZ_DestroyEntityInECSRegistry(&ecs, active_entities[idx]);
            active_entities[idx] = active_entities[--active_count];
        } else if (active_count > 0) {
            // Add/Remove component or tag
            YULDUZ_Entity e           = active_entities[rand() % active_count];
            int           comp_choice = rand() % 8;

            switch (comp_choice) {
                case 0: {
                    Position pos = {(float)op, 0.0f, 0.0f};
                    YULDUZ_AddComponentInECSRegistry(&ecs, e, "Position", &pos);
                    break;
                }
                case 1: {
                    Velocity vel = {1.0f, 1.0f, 1.0f};
                    YULDUZ_AddComponentInECSRegistry(&ecs, e, "Velocity", &vel);
                    break;
                }
                case 2:
                    YULDUZ_RemoveComponentInECSRegistry(&ecs, e, "Position");
                    break;
                case 3:
                    YULDUZ_RemoveComponentInECSRegistry(&ecs, e, "Velocity");
                    break;
                case 4:
                    YULDUZ_AddTagInECSRegistry(&ecs, e, "Player");
                    break;
                case 5:
                    YULDUZ_AddTagInECSRegistry(&ecs, e, "Enemy");
                    break;
                case 6:
                    YULDUZ_RemoveTagInECSRegistry(&ecs, e, "Player");
                    break;
                case 7:
                    YULDUZ_RemoveTagInECSRegistry(&ecs, e, "Enemy");
                    break;
            }
        }

        if (op % 5000 == 0) {
            YULDUZ_LOG_INFO("Progress: %u%% (Active entities: %u, Archetypes: %u)",
                            (op * 100) / operations, active_count, ecs.DenseCount);
        }
    }

    YULDUZ_LOG_INFO("Completed. Final state:");
    YULDUZ_LOG_INFO("  Active entities: %u", active_count);
    YULDUZ_LOG_INFO("  Archetypes: %u", ecs.DenseCount);

    SDL_free(active_entities);
    YULDUZ_ReleaseECSRegistry(&ecs);

    TEST_END("Stress: Random Operations");
}

// ============================================================================
// COMMAND BUFFER TESTS (Stub - implement if Command Buffer is ready)
// ============================================================================

void run_all_command_buffer_tests(void) {
    YULDUZ_LOG_INFO("\n");
    YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
    YULDUZ_LOG_INFO("║           COMMAND BUFFER TESTS                                 ║");
    YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");
    YULDUZ_LOG_INFO("\n");

    YULDUZ_LOG_INFO("Command Buffer tests not yet implemented (feature not ready)");

    // TODO: Implement when Command Buffer is ready
    // test_command_buffer_deferred_operations();
    // test_command_buffer_entity_creation();
    // test_command_buffer_add_remove_components();
    // test_command_buffer_flush();
}

void test_command_buffer_deferred_operations(void) {}

void test_command_buffer_entity_creation(void) {}

void test_command_buffer_add_remove_components(void) {}

void test_command_buffer_flush(void) {}