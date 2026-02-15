#include <Tests/Tests.h>

// ============================================================================
// BENCHMARKS
// ============================================================================

void run_all_benchmarks(void) {
    YULDUZ_LOG_INFO("\n");
    YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
    YULDUZ_LOG_INFO("║           PERFORMANCE BENCHMARKS                               ║");
    YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");
    YULDUZ_LOG_INFO("\n");

    benchmark_archetype_add();
    benchmark_archetype_remove();
    benchmark_archetype_move();
    benchmark_archetype_iteration();
    benchmark_entity_create_destroy();
    benchmark_entity_reuse();
    benchmark_ecs_component_add();
    benchmark_ecs_component_remove();
    benchmark_ecs_component_set_get();
    benchmark_ecs_archetype_transition();
    benchmark_query_matching();
    benchmark_system_execution();
}

void benchmark_archetype_add(void) {
    const uint32_t iterations = 100000;

    YULDUZ_Archetype archetype = {0};
    YULDUZ_TypeInfo comps[] = {
        {.Type = g_position_type, .Size = sizeof(Position), .Alignment = _Alignof(Position)},
        {.Type = g_velocity_type, .Size = sizeof(Velocity), .Alignment = _Alignof(Velocity)}
    };
    YULDUZ_SDL_SortTypeInfos(comps, 2);
    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&archetype, comps, 2, nullptr, 0, 1024),
                  "Failed to init archetype");

    BENCHMARK_START("Archetype Add", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        Position pos = {(float)i, (float)i, (float)i};
        Velocity vel = {1.0f, 1.0f, 1.0f};

        YULDUZ_TypeDataInfo data[] = {
            {.Type = g_position_type, .Data = &pos},
            {.Type = g_velocity_type, .Data = &vel}
        };

        YULDUZ_ArchetypeIndex idx;
        YULDUZ_AddInArchetype(&archetype, i, data, &idx);
    }

    BENCHMARK_END();

    YULDUZ_LOG_INFO("Total entities added: %u", archetype.DenseCount);
    YULDUZ_LOG_INFO("Final capacity: %u", archetype.DenseCapacity);

    YULDUZ_ReleaseArchetype(&archetype);
}

void benchmark_archetype_remove(void) {
    const uint32_t iterations = 50000;

    YULDUZ_Archetype archetype = {0};
    YULDUZ_TypeInfo comps[] = {
        {.Type = g_position_type, .Size = sizeof(Position), .Alignment = _Alignof(Position)}
    };

    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&archetype, comps, 1, nullptr, 0, iterations),
                  "Failed to init archetype");

    // Pre-populate
    for (uint32_t i = 0; i < iterations; i++) {
        Position pos = {(float)i, (float)i, (float)i};
        YULDUZ_TypeDataInfo data[] = {{.Type = g_position_type, .Data = &pos}};

        YULDUZ_ArchetypeIndex idx;
        YULDUZ_AddInArchetype(&archetype, i, data, &idx);
    }

    BENCHMARK_START("Archetype Remove", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        YULDUZ_Entity moved;
        YULDUZ_RemoveInArchetype(&archetype, 0, &moved);
    }

    BENCHMARK_END();

    YULDUZ_ASSERT(archetype.DenseCount == 0, "All entities should be removed");

    YULDUZ_ReleaseArchetype(&archetype);
}

void benchmark_archetype_move(void) {
    const uint32_t iterations = 10000;

    YULDUZ_Archetype src = {0};
    YULDUZ_Archetype dst = {0};

    YULDUZ_TypeInfo src_comps[] = {
        {.Type = g_position_type, .Size = sizeof(Position), .Alignment = _Alignof(Position)}
    };
    YULDUZ_TypeInfo dst_comps[] = {
        {.Type = g_position_type, .Size = sizeof(Position), .Alignment = _Alignof(Position)},
        {.Type = g_health_type, .Size = sizeof(Health), .Alignment = _Alignof(Health)}
    };
    YULDUZ_SDL_SortTypeInfos(dst_comps, 2);
    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&src, src_comps, 1, nullptr, 0, iterations),
                  "Failed init src");
    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&dst, dst_comps, 2, nullptr, 0, iterations),
                  "Failed init dst");

    // Pre-populate source
    for (uint32_t i = 0; i < iterations; i++) {
        Position pos = {(float)i, (float)i, (float)i};
        YULDUZ_TypeDataInfo data[] = {{.Type = g_position_type, .Data = &pos}};

        YULDUZ_ArchetypeIndex idx;
        YULDUZ_AddInArchetype(&src, i, data, &idx);
    }

    BENCHMARK_START("Archetype Move", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        Health health = {100.0f, 100.0f};
        YULDUZ_TypeDataInfo move_data[] = {{.Type = g_health_type, .Data = &health}};

        YULDUZ_ArchetypeIndex dst_idx;
        YULDUZ_Entity moved;
        YULDUZ_MoveEntityInArchetype(&src, &dst, move_data, 1, 0, &dst_idx, &moved);
    }

    BENCHMARK_END();

    YULDUZ_ASSERT(src.DenseCount == 0, "Source should be empty");
    YULDUZ_ASSERT(dst.DenseCount == iterations, "All entities should be in destination");

    YULDUZ_ReleaseArchetype(&src);
    YULDUZ_ReleaseArchetype(&dst);
}

void benchmark_archetype_iteration(void) {
    const uint32_t entity_count = 100000;
    const uint32_t iterations = 100;

    YULDUZ_Archetype archetype = {0};
    YULDUZ_TypeInfo comps[] = {
        {.Type = g_position_type, .Size = sizeof(Position), .Alignment = _Alignof(Position)},
        {.Type = g_velocity_type, .Size = sizeof(Velocity), .Alignment = _Alignof(Velocity)}
    };
    YULDUZ_SDL_SortTypeInfos(comps, 2);
    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&archetype, comps, 2, nullptr, 0, entity_count),
                  "Failed to init archetype");

    // Populate with entities
    for (uint32_t i = 0; i < entity_count; i++) {
        Position pos = {0.0f, 0.0f, 0.0f};
        Velocity vel = {1.0f, 1.0f, 1.0f};

        YULDUZ_TypeDataInfo data[] = {
            {.Type = g_position_type, .Data = &pos},
            {.Type = g_velocity_type, .Data = &vel}
        };

        YULDUZ_ArchetypeIndex idx;
        YULDUZ_AddInArchetype(&archetype, i, data, &idx);
    }

    YULDUZ_ComponentStore *pos_store = YULDUZ_QueryStoreInArchetype(&archetype, g_position_type);
    YULDUZ_ComponentStore *vel_store = YULDUZ_QueryStoreInArchetype(&archetype, g_velocity_type);

    BENCHMARK_START("Archetype Iteration (Update)", iterations * entity_count);

    for (uint32_t iter = 0; iter < iterations; iter++) {
        for (uint32_t i = 0; i < archetype.DenseCount; i++) {
            Position *pos = (Position *)YULDUZ_GetComponentInComponentStore(pos_store, i);
            Velocity *vel = (Velocity *)YULDUZ_GetComponentInComponentStore(vel_store, i);

            pos->x += vel->x;
            pos->y += vel->y;
            pos->z += vel->z;
        }
    }

    BENCHMARK_END();

    YULDUZ_ReleaseArchetype(&archetype);
}

void benchmark_entity_create_destroy(void) {
    const uint32_t iterations = 100000;

    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeEntityRegistry(&registry, 1024),
                  "Failed to initialize registry");

    BENCHMARK_START("Entity Create/Destroy", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        YULDUZ_Entity e;
        YULDUZ_CreateEntityInEntityRegistry(&registry, i % 100, i, &e);

        if (i % 2 == 0 && i > 0) {
            YULDUZ_DestroyEntityInEntityRegistry(&registry, e);
        }
    }

    BENCHMARK_END();

    YULDUZ_ReleaseEntityRegistry(&registry);
}

void benchmark_entity_reuse(void) {
    const uint32_t iterations = 50000;

    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeEntityRegistry(&registry, 1024),
                  "Failed to initialize registry");

    BENCHMARK_START("Entity Reuse", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        YULDUZ_Entity e1, e2;
        YULDUZ_CreateEntityInEntityRegistry(&registry, 0, 0, &e1);
        YULDUZ_DestroyEntityInEntityRegistry(&registry, e1);
        YULDUZ_CreateEntityInEntityRegistry(&registry, 1, 1, &e2);
    }

    BENCHMARK_END();

    YULDUZ_ReleaseEntityRegistry(&registry);
}

void benchmark_ecs_component_add(void) {
    const uint32_t iterations = 10000;

    YULDUZ_ECSRegistry ecs = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity = 8,
        .InitialEntityCapacity = iterations,
        .InitialArchetypeCapacity = 32,
        .InitialArchetypeTypeCapacity = 32
    };

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    // Create entities
    YULDUZ_Entity *entities = SDL_malloc(sizeof(YULDUZ_Entity) * iterations);
    for (uint32_t i = 0; i < iterations; i++) {
        YULDUZ_CreateEntityInECSRegistry(&ecs, &entities[i]);
    }

    BENCHMARK_START("ECS Add Component", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        Position pos = {(float)i, (float)i, (float)i};
        YULDUZ_AddComponentInECSRegistry(&ecs, entities[i], "Position", &pos);
    }

    BENCHMARK_END();

    SDL_free(entities);
    YULDUZ_ReleaseECSRegistry(&ecs);
}

void benchmark_ecs_component_remove(void) {
    const uint32_t iterations = 10000;

    YULDUZ_ECSRegistry ecs = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity = 8,
        .InitialEntityCapacity = iterations,
        .InitialArchetypeCapacity = 32,
        .InitialArchetypeTypeCapacity = 32
    };

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    // Create entities with Position
    YULDUZ_Entity *entities = SDL_malloc(sizeof(YULDUZ_Entity) * iterations);
    for (uint32_t i = 0; i < iterations; i++) {
        YULDUZ_CreateEntityInECSRegistry(&ecs, &entities[i]);
        Position pos = {(float)i, (float)i, (float)i};
        YULDUZ_AddComponentInECSRegistry(&ecs, entities[i], "Position", &pos);
    }

    BENCHMARK_START("ECS Remove Component", iterations);

    for (uint32_t i = 0; i < iterations; i++) {
        YULDUZ_RemoveComponentInECSRegistry(&ecs, entities[i], "Position");
    }

    BENCHMARK_END();

    SDL_free(entities);
    YULDUZ_ReleaseECSRegistry(&ecs);
}

void benchmark_ecs_component_set_get(void) {
    const uint32_t entity_count = 10000;
    const uint32_t iterations = 100;

    YULDUZ_ECSRegistry ecs = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity = 8,
        .InitialEntityCapacity = entity_count,
        .InitialArchetypeCapacity = 32,
        .InitialArchetypeTypeCapacity = 32
    };

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    // Create entities with Position
    YULDUZ_Entity *entities = SDL_malloc(sizeof(YULDUZ_Entity) * entity_count);
    for (uint32_t i = 0; i < entity_count; i++) {
        YULDUZ_CreateEntityInECSRegistry(&ecs, &entities[i]);
        Position pos = {0.0f, 0.0f, 0.0f};
        YULDUZ_AddComponentInECSRegistry(&ecs, entities[i], "Position", &pos);
    }

    BENCHMARK_START("ECS Set/Get Component", iterations * entity_count * 2);

    for (uint32_t iter = 0; iter < iterations; iter++) {
        for (uint32_t i = 0; i < entity_count; i++) {
            Position pos = {(float)iter, (float)iter, (float)iter};
            YULDUZ_SetComponentInECSRegistry(&ecs, entities[i], "Position", &pos);

            Position check;
            YULDUZ_GetComponentInECSRegistry(&ecs, entities[i], "Position", &check);
        }
    }

    BENCHMARK_END();

    SDL_free(entities);
    YULDUZ_ReleaseECSRegistry(&ecs);
}

void benchmark_ecs_archetype_transition(void) {
    const uint32_t iterations = 5000;

    YULDUZ_ECSRegistry ecs = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity = 8,
        .InitialEntityCapacity = iterations,
        .InitialArchetypeCapacity = 64,
        .InitialArchetypeTypeCapacity = 64
    };

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    // Create entities
    YULDUZ_Entity *entities = SDL_malloc(sizeof(YULDUZ_Entity) * iterations);
    for (uint32_t i = 0; i < iterations; i++) {
        YULDUZ_CreateEntityInECSRegistry(&ecs, &entities[i]);
    }

    BENCHMARK_START("ECS Archetype Transitions", iterations * 4);

    for (uint32_t i = 0; i < iterations; i++) {
        Position pos = {(float)i, (float)i, (float)i};
        YULDUZ_AddComponentInECSRegistry(&ecs, entities[i], "Position", &pos);

        Velocity vel = {1.0f, 1.0f, 1.0f};
        YULDUZ_AddComponentInECSRegistry(&ecs, entities[i], "Velocity", &vel);

        Health health = {100.0f, 100.0f};
        YULDUZ_AddComponentInECSRegistry(&ecs, entities[i], "Health", &health);

        YULDUZ_RemoveComponentInECSRegistry(&ecs, entities[i], "Velocity");
    }

    BENCHMARK_END();

    YULDUZ_LOG_INFO("Final archetype count: %u", ecs.DenseCount);

    SDL_free(entities);
    YULDUZ_ReleaseECSRegistry(&ecs);
}

void benchmark_query_matching(void) {
    const uint32_t archetype_count = 1000;
    const uint32_t iterations = 10000;

    // Create many archetypes with varying components
    YULDUZ_Archetype *archetypes = SDL_malloc(sizeof(YULDUZ_Archetype) * archetype_count);

    for (uint32_t i = 0; i < archetype_count; i++) {
        uint32_t comp_count = (i % 4) + 1;
        YULDUZ_TypeInfo comps[4];

        uint32_t idx = 0;
        if (comp_count > 0) comps[idx++] = (YULDUZ_TypeInfo){g_position_type, sizeof(Position), _Alignof(Position)};
        if (comp_count > 1) comps[idx++] = (YULDUZ_TypeInfo){g_velocity_type, sizeof(Velocity), _Alignof(Velocity)};
        if (comp_count > 2) comps[idx++] = (YULDUZ_TypeInfo){g_health_type, sizeof(Health), _Alignof(Health)};
        if (comp_count > 3) comps[idx++] = (YULDUZ_TypeInfo){g_damage_type, sizeof(Damage), _Alignof(Damage)};
        YULDUZ_SDL_SortTypeInfos(comps, comp_count);
        YULDUZ_InitializeArchetype(&archetypes[i], comps, comp_count, nullptr, 0, 4);
    }

    // Create query for Position + Velocity
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 4, 2);
    YULDUZ_SetQueryWithComponentType(&query, g_position_type, YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&query, g_velocity_type, YULDUZ_QueryAccessType_Read);

    YULDUZ_QueryInfo info;
    YULDUZ_CreateQueryInfo(&query, &info);

    BENCHMARK_START("Query Archetype Matching", iterations * archetype_count);

    uint32_t match_count = 0;
    for (uint32_t iter = 0; iter < iterations; iter++) {
        for (uint32_t i = 0; i < archetype_count; i++) {
            if (YULDUZ_ArchetypeSupportsQueryInfo(&info, &archetypes[i])) {
                match_count++;
            }
        }
    }

    BENCHMARK_END();

    YULDUZ_LOG_INFO("Matches found per iteration: %u", match_count / iterations);

    YULDUZ_DestroyQueryInfo(&info);
    YULDUZ_ReleaseQuery(&query);

    for (uint32_t i = 0; i < archetype_count; i++) {
        YULDUZ_ReleaseArchetype(&archetypes[i]);
    }
    SDL_free(archetypes);
}

void benchmark_system_execution(void) {
    const uint32_t entity_count = 100000;
    const uint32_t iterations = 100;

    YULDUZ_ECSRegistry ecs = {0};
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialTypeCapacity = 8,
        .InitialEntityCapacity = entity_count,
        .InitialArchetypeCapacity = 32,
        .InitialArchetypeTypeCapacity = 32
    };

    YULDUZ_ASSERT(YULDUZ_InitializeECSRegistry(&ecs, info),
                  "Failed to initialize ECS");

    setup_test_types(&ecs.TypeRegistry);

    // Create entities with Position + Velocity
    for (uint32_t i = 0; i < entity_count; i++) {
        YULDUZ_Entity e;
        YULDUZ_CreateEntityInECSRegistry(&ecs, &e);

        Position pos = {0.0f, 0.0f, 0.0f};
        Velocity vel = {1.0f, 1.0f, 1.0f};

        YULDUZ_AddComponentInECSRegistry(&ecs, e, "Position", &pos);
        YULDUZ_AddComponentInECSRegistry(&ecs, e, "Velocity", &vel);
    }

    // Create movement system
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 4, 2);
    YULDUZ_SetQueryWithComponentType(&query, g_position_type, YULDUZ_QueryAccessType_Write);
    YULDUZ_SetQueryWithComponentType(&query, g_velocity_type, YULDUZ_QueryAccessType_Read);

    void MovementSystem(YULDUZ_Archetype *archetype, const YULDUZ_QueryInfo *q, void *user_data) {
        (void)q;
        (void)user_data;

        YULDUZ_ComponentStore *pos_store = YULDUZ_QueryStoreInArchetype(archetype, g_position_type);
        YULDUZ_ComponentStore *vel_store = YULDUZ_QueryStoreInArchetype(archetype, g_velocity_type);

        for (uint32_t i = 0; i < archetype->DenseCount; i++) {
            Position *pos = (Position *)YULDUZ_GetComponentInComponentStore(pos_store, i);
            Velocity *vel = (Velocity *)YULDUZ_GetComponentInComponentStore(vel_store, i);

            pos->x += vel->x;
            pos->y += vel->y;
            pos->z += vel->z;
        }
    }

    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "MovementSystem", &query, MovementSystem);

    BENCHMARK_START("System Execution", iterations * entity_count);

    for (uint32_t iter = 0; iter < iterations; iter++) {
        YULDUZ_RunSystem(&system, &ecs, nullptr);
    }

    BENCHMARK_END();

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&ecs);
}