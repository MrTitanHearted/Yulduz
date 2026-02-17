#include <Yulduz/Engine.h>
#include <SDL3/SDL.h>

// Benchmark helper macros
#define BENCHMARK_START(name)                              \
    do {                                                   \
        YULDUZ_LOG_INFO("BENCHMARK: %s", name);            \
        uint64_t start_time = SDL_GetPerformanceCounter(); \
        uint64_t freq       = SDL_GetPerformanceFrequency();

#define BENCHMARK_END()                                              \
    uint64_t end_time   = SDL_GetPerformanceCounter();               \
    double   elapsed_ms = ((end_time - start_time) * 1000.0) / freq; \
    YULDUZ_LOG_INFO("  Time: %.3f ms", elapsed_ms);                  \
    }                                                                \
    while (0)

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

typedef struct Transform {
    float matrix[16];
} Transform;

// ============================================================================
// Type Registry Benchmarks
// ============================================================================

void benchmark_type_registry_register_types(void) {
    BENCHMARK_START("Type Registry - Register 1000 types");

    YULDUZ_TypeRegistry registry = {0};
    YULDUZ_InitializeTypeRegistry(&registry, 16);

    char name_buffer[64];
    for (uint32_t i = 0; i < 1000; i++) {
        SDL_snprintf(name_buffer, sizeof(name_buffer), "Type_%u", i);
        YULDUZ_TypeDescription desc = {
            .Name      = name_buffer,
            .Size      = 4 + (i % 64),
            .Alignment = 4};
        YULDUZ_RegisterTypesInTypeRegistry(&registry, &desc, nullptr, 1);
    }

    YULDUZ_ReleaseTypeRegistry(&registry);

    BENCHMARK_END();
}

void benchmark_type_registry_lookup_types(void) {
    YULDUZ_TypeRegistry registry = {0};
    YULDUZ_InitializeTypeRegistry(&registry, 16);

    // Prepare types
    char name_buffer[64];
    for (uint32_t i = 0; i < 100; i++) {
        SDL_snprintf(name_buffer, sizeof(name_buffer), "Type_%u", i);
        YULDUZ_TypeDescription desc = {
            .Name      = name_buffer,
            .Size      = 4,
            .Alignment = 4};
        YULDUZ_RegisterTypesInTypeRegistry(&registry, &desc, nullptr, 1);
    }

    BENCHMARK_START("Type Registry - Lookup 10000 types");

    const char *names[100];
    YULDUZ_Type types[100];

    for (uint32_t iter = 0; iter < 100; iter++) {
        for (uint32_t i = 0; i < 100; i++) {
            SDL_snprintf(name_buffer, sizeof(name_buffer), "Type_%u", i);
            names[i] = name_buffer;
        }
        YULDUZ_GetTypesInTypeRegistry(&registry, names, types, 100);
    }

    YULDUZ_ReleaseTypeRegistry(&registry);

    BENCHMARK_END();
}

void benchmark_type_registry_sort_types(void) {
    YULDUZ_TypeRegistry registry = {0};
    YULDUZ_InitializeTypeRegistry(&registry, 1024);

    // Register types
    YULDUZ_Type types[1000];
    char        name_buffer[64];
    for (uint32_t i = 0; i < 1000; i++) {
        SDL_snprintf(name_buffer, sizeof(name_buffer), "Type_%u", i);
        YULDUZ_TypeDescription desc = {
            .Name      = name_buffer,
            .Size      = 4,
            .Alignment = 4};
        YULDUZ_RegisterTypesInTypeRegistry(&registry, &desc, &types[i], 1);
    }

    BENCHMARK_START("Type Registry - Sort 1000 types x 1000 times");

    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_SDL_SortTypes(types, 1000);
    }

    YULDUZ_ReleaseTypeRegistry(&registry);

    BENCHMARK_END();
}

// ============================================================================
// Entity Registry Benchmarks
// ============================================================================

void benchmark_entity_registry_create_entities(void) {
    BENCHMARK_START("Entity Registry - Create 100,000 entities");

    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 1024);

    for (uint32_t i = 0; i < 100000; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInEntityRegistry(&registry, i % 10, i % 100, &entity);
    }

    YULDUZ_ReleaseEntityRegistry(&registry);

    BENCHMARK_END();
}

void benchmark_entity_registry_create_destroy_cycle(void) {
    BENCHMARK_START("Entity Registry - Create/Destroy cycle 50,000 times");

    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 1024);

    for (uint32_t i = 0; i < 50000; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInEntityRegistry(&registry, 0, 0, &entity);
        YULDUZ_DestroyEntityInEntityRegistry(&registry, entity);
    }

    YULDUZ_ReleaseEntityRegistry(&registry);

    BENCHMARK_END();
}

void benchmark_entity_registry_get_records(void) {
    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 100000);

    // Create entities
    YULDUZ_Entity entities[10000];
    for (uint32_t i = 0; i < 10000; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, i % 10, i % 100, &entities[i]);
    }

    BENCHMARK_START("Entity Registry - Get 10,000 entity records x 1000 times");

    YULDUZ_EntityRecord records[10000];
    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_GetEntityRecordsInEntityRegistry(&registry, entities, records, 10000);
    }

    YULDUZ_ReleaseEntityRegistry(&registry);

    BENCHMARK_END();
}

void benchmark_entity_registry_set_records(void) {
    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 100000);

    // Create entities
    YULDUZ_Entity entities[10000];
    for (uint32_t i = 0; i < 10000; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, 0, 0, &entities[i]);
    }

    BENCHMARK_START("Entity Registry - Set 10,000 entity records x 1000 times");

    YULDUZ_EntityRecord records[10000];
    for (uint32_t i = 0; i < 10000; i++) {
        records[i].ArchetypeType  = i % 10;
        records[i].ArchetypeIndex = i % 100;
    }

    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_SetEntityRecordsInEntityRegistry(&registry, entities, records, 10000);
    }

    YULDUZ_ReleaseEntityRegistry(&registry);

    BENCHMARK_END();
}

void benchmark_entity_registry_fragmentation(void) {
    BENCHMARK_START("Entity Registry - Fragmentation test (create 10000, destroy 5000, create 5000)");

    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 1024);

    YULDUZ_Entity entities[10000];

    // Create 10000 entities
    for (uint32_t i = 0; i < 10000; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, 0, 0, &entities[i]);
    }

    // Destroy every other entity
    for (uint32_t i = 0; i < 10000; i += 2) {
        YULDUZ_DestroyEntityInEntityRegistry(&registry, entities[i]);
    }

    // Create 5000 new entities (should reuse)
    for (uint32_t i = 0; i < 5000; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInEntityRegistry(&registry, 1, 1, &entity);
    }

    YULDUZ_ReleaseEntityRegistry(&registry);

    BENCHMARK_END();
}

// ============================================================================
// Archetype Benchmarks
// ============================================================================

void benchmark_archetype_add_entities(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Type type;
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &desc, &type, 1);

    YULDUZ_TypeInfo type_info = {
        .Type      = type,
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Archetype archetype = {0};
    YULDUZ_InitializeArchetype(&archetype, &type_info, 1, nullptr, 0, 1024);

    BENCHMARK_START("Archetype - Add 50,000 entities");

    Position            pos       = {0};
    YULDUZ_TypeDataInfo data_info = {.Type = type, .Data = &pos};

    for (uint32_t i = 0; i < 50000; i++) {
        pos.x = (float)i;
        pos.y = (float)i * 2;
        pos.z = (float)i * 3;
        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, i, &data_info, &index);
    }

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);

    BENCHMARK_END();
}

void benchmark_archetype_remove_entities(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Type type;
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &desc, &type, 1);

    YULDUZ_TypeInfo type_info = {
        .Type      = type,
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Archetype archetype = {0};
    YULDUZ_InitializeArchetype(&archetype, &type_info, 1, nullptr, 0, 100000);

    // Add entities
    Position            pos       = {0};
    YULDUZ_TypeDataInfo data_info = {.Type = type, .Data = &pos};
    for (uint32_t i = 0; i < 10000; i++) {
        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, i, &data_info, &index);
    }

    BENCHMARK_START("Archetype - Remove 10,000 entities");

    for (uint32_t i = 0; i < 10000; i++) {
        YULDUZ_Entity moved;
        YULDUZ_RemoveInArchetype(&archetype, 0, &moved);
    }

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);

    BENCHMARK_END();
}

void benchmark_archetype_component_access(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Type type;
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &desc, &type, 1);

    YULDUZ_TypeInfo type_info = {
        .Type      = type,
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Archetype archetype = {0};
    YULDUZ_InitializeArchetype(&archetype, &type_info, 1, nullptr, 0, 100000);

    // Add entities
    Position            pos       = {0};
    YULDUZ_TypeDataInfo data_info = {.Type = type, .Data = &pos};
    for (uint32_t i = 0; i < 10000; i++) {
        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, i, &data_info, &index);
    }

    BENCHMARK_START("Archetype - Access 10,000 components x 1000 iterations");

    YULDUZ_ComponentStore *store = YULDUZ_QueryStoreInArchetype(&archetype, type);
    float                  sum   = 0.0f;

    for (uint32_t iter = 0; iter < 1000; iter++) {
        for (uint32_t i = 0; i < 10000; i++) {
            Position *p = (Position *)YULDUZ_GetComponentInComponentStore(store, i);
            sum += p->x + p->y + p->z;
        }
    }

    YULDUZ_LOG_INFO("  Sum (prevent optimization): %.2f", sum);

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);

    BENCHMARK_END();
}

void benchmark_archetype_multi_component(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)},
        {.Name = "Health", .Size = sizeof(Health), .Alignment = alignof(Health)}};

    YULDUZ_Type types[3];
    for (uint32_t i = 0; i < 3; i++) {
        YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &descs[i], &types[i], 1);
    }
    YULDUZ_SDL_SortTypes(types, 3);

    YULDUZ_TypeInfo type_infos[3];
    for (uint32_t i = 0; i < 3; i++) {
        type_infos[i].Type      = types[i];
        type_infos[i].Size      = descs[i].Size;
        type_infos[i].Alignment = descs[i].Alignment;
    }
    YULDUZ_SDL_SortTypeInfos(type_infos, 3);

    YULDUZ_Archetype archetype = {0};
    YULDUZ_InitializeArchetype(&archetype, type_infos, 3, nullptr, 0, 100000);

    BENCHMARK_START("Archetype - Add 20,000 entities with 3 components");

    Position pos    = {0};
    Velocity vel    = {0};
    Health   health = {100.0f, 100.0f};

    YULDUZ_TypeDataInfo data_infos[3] = {
        {.Type = types[0], .Data = &pos},
        {.Type = types[1], .Data = &vel},
        {.Type = types[2], .Data = &health}};
    YULDUZ_SDL_SortTypeDataInfos(data_infos, 3);

    for (uint32_t i = 0; i < 20000; i++) {
        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, i, data_infos, &index);
    }

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);

    BENCHMARK_END();
}

// ============================================================================
// ECS Registry Benchmarks
// ============================================================================

void benchmark_ecs_registry_entity_creation(void) {
    BENCHMARK_START("ECS Registry - Create 50,000 entities");

    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {64, 100000, 32, 64};
    YULDUZ_InitializeECSRegistry(&registry, info);

    for (uint32_t i = 0; i < 50000; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
    }

    YULDUZ_ReleaseECSRegistry(&registry);

    BENCHMARK_END();
}

void benchmark_ecs_registry_add_components(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {64, 100000, 32, 64};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register type
    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};
    YULDUZ_RegisterTypeInECSRegistry(&registry, desc, nullptr);

    // Create entities
    YULDUZ_Entity entities[10000];
    for (uint32_t i = 0; i < 10000; i++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &entities[i]);
    }

    BENCHMARK_START("ECS Registry - Add component to 10,000 entities");

    Position pos = {0};
    for (uint32_t i = 0; i < 10000; i++) {
        pos.x = (float)i;
        YULDUZ_AddComponentInECSRegistry(&registry, entities[i], "Position", &pos);
    }

    YULDUZ_ReleaseECSRegistry(&registry);

    BENCHMARK_END();
}

void benchmark_ecs_registry_get_set_components(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {64, 100000, 32, 64};
    YULDUZ_InitializeECSRegistry(&registry, info);

    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};
    YULDUZ_RegisterTypeInECSRegistry(&registry, desc, nullptr);

    // Setup entities with components
    YULDUZ_Entity entities[5000];
    Position      pos = {0};
    for (uint32_t i = 0; i < 5000; i++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &entities[i]);
        YULDUZ_AddComponentInECSRegistry(&registry, entities[i], "Position", &pos);
    }

    BENCHMARK_START("ECS Registry - Get/Set components 5000 entities x 200 iterations");

    for (uint32_t iter = 0; iter < 200; iter++) {
        for (uint32_t i = 0; i < 5000; i++) {
            Position retrieved;
            YULDUZ_GetComponentInECSRegistry(&registry, entities[i], "Position", &retrieved);
            retrieved.x += 1.0f;
            YULDUZ_SetComponentInECSRegistry(&registry, entities[i], "Position", &retrieved);
        }
    }

    YULDUZ_ReleaseECSRegistry(&registry);

    BENCHMARK_END();
}

void benchmark_ecs_registry_archetype_transitions(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {64, 100000, 32, 64};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register types
    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)},
        {.Name = "Health", .Size = sizeof(Health), .Alignment = alignof(Health)}};

    for (uint32_t i = 0; i < 3; i++) {
        YULDUZ_RegisterTypeInECSRegistry(&registry, descs[i], nullptr);
    }

    // Create entities
    YULDUZ_Entity entities[1000];
    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &entities[i]);
    }

    BENCHMARK_START("ECS Registry - Archetype transitions (1000 entities, 3 components)");

    Position pos    = {0};
    Velocity vel    = {0};
    Health   health = {100.0f, 100.0f};

    // Add Position
    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_AddComponentInECSRegistry(&registry, entities[i], "Position", &pos);
    }

    // Add Velocity
    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_AddComponentInECSRegistry(&registry, entities[i], "Velocity", &vel);
    }

    // Add Health
    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_AddComponentInECSRegistry(&registry, entities[i], "Health", &health);
    }

    // Remove components
    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_RemoveComponentInECSRegistry(&registry, entities[i], "Velocity");
    }

    YULDUZ_ReleaseECSRegistry(&registry);

    BENCHMARK_END();
}

void benchmark_ecs_registry_complex_scenario(void) {
    BENCHMARK_START("ECS Registry - Complex scenario (10,000 entities, mixed operations)");

    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {64, 20000, 32, 64};
    YULDUZ_InitializeECSRegistry(&registry, info);

    // Register types
    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)},
        {.Name = "Health", .Size = sizeof(Health), .Alignment = alignof(Health)},
        {.Name = "Transform", .Size = sizeof(Transform), .Alignment = alignof(Transform)},
        {.Name = "Player", .Size = 0, .Alignment = 0},
        {.Name = "Enemy", .Size = 0, .Alignment = 0}};

    for (uint32_t i = 0; i < 6; i++) {
        YULDUZ_RegisterTypeInECSRegistry(&registry, descs[i], nullptr);
    }

    // Create entities with various component combinations
    YULDUZ_Entity entities[10000];
    Position      pos    = {0};
    Velocity      vel    = {0};
    Health        health = {100.0f, 100.0f};

    for (uint32_t i = 0; i < 10000; i++) {
        YULDUZ_CreateEntityInECSRegistry(&registry, &entities[i]);

        // All have position
        YULDUZ_AddComponentInECSRegistry(&registry, entities[i], "Position", &pos);

        // 50% have velocity
        if (i % 2 == 0) {
            YULDUZ_AddComponentInECSRegistry(&registry, entities[i], "Velocity", &vel);
        }

        // 75% have health
        if (i % 4 != 0) {
            YULDUZ_AddComponentInECSRegistry(&registry, entities[i], "Health", &health);
        }

        // Tags
        if (i % 10 == 0) {
            YULDUZ_AddTagInECSRegistry(&registry, entities[i], "Player");
        } else if (i % 3 == 0) {
            YULDUZ_AddTagInECSRegistry(&registry, entities[i], "Enemy");
        }
    }

    // Simulate game loop updates
    for (uint32_t frame = 0; frame < 10; frame++) {
        for (uint32_t i = 0; i < 10000; i++) {
            Position p;
            if (YULDUZ_GetComponentInECSRegistry(&registry, entities[i], "Position", &p)) {
                p.x += 1.0f;
                YULDUZ_SetComponentInECSRegistry(&registry, entities[i], "Position", &p);
            }
        }
    }

    YULDUZ_ReleaseECSRegistry(&registry);

    BENCHMARK_END();
}

// ============================================================================
// Benchmark Runner
// ============================================================================

void run_all_type_registry_benchmarks(void) {
    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("Type Registry Benchmarks");
    YULDUZ_LOG_INFO("========================================\n");

    benchmark_type_registry_register_types();
    benchmark_type_registry_lookup_types();
    benchmark_type_registry_sort_types();
}

void run_all_entity_registry_benchmarks(void) {
    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("Entity Registry Benchmarks");
    YULDUZ_LOG_INFO("========================================\n");

    benchmark_entity_registry_create_entities();
    benchmark_entity_registry_create_destroy_cycle();
    benchmark_entity_registry_get_records();
    benchmark_entity_registry_set_records();
    benchmark_entity_registry_fragmentation();
}

void run_all_archetype_benchmarks(void) {
    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("Archetype Benchmarks");
    YULDUZ_LOG_INFO("========================================\n");

    benchmark_archetype_add_entities();
    benchmark_archetype_remove_entities();
    benchmark_archetype_component_access();
    benchmark_archetype_multi_component();
}

void run_all_ecs_registry_benchmarks(void) {
    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("ECS Registry Benchmarks");
    YULDUZ_LOG_INFO("========================================\n");

    benchmark_ecs_registry_entity_creation();
    benchmark_ecs_registry_add_components();
    benchmark_ecs_registry_get_set_components();
    benchmark_ecs_registry_archetype_transitions();
    benchmark_ecs_registry_complex_scenario();
}

// ============================================================================
// Query Benchmarks
// ============================================================================

void benchmark_query_creation(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)},
        {.Name = "Health", .Size = sizeof(Health), .Alignment = alignof(Health)}};

    YULDUZ_Type types[3];
    for (uint32_t i = 0; i < 3; i++) {
        YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &descs[i], &types[i], 1);
    }

    BENCHMARK_START("Query - Create 10,000 complex queries");

    for (uint32_t i = 0; i < 10000; i++) {
        YULDUZ_Query query = {0};
        YULDUZ_InitializeQuery(&query, 16);
        YULDUZ_SetQueryWithComponentType(&query, types[0], YULDUZ_QueryAccessType_Read);
        YULDUZ_SetQueryWithComponentType(&query, types[1], YULDUZ_QueryAccessType_Write);
        YULDUZ_SetQueryWithComponentType(&query, types[2], YULDUZ_QueryAccessType_Read);
        YULDUZ_ReleaseQuery(&query);
    }

    YULDUZ_ReleaseTypeRegistry(&type_registry);

    BENCHMARK_END();
}

void benchmark_query_archetype_matching(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)},
        {.Name = "Health", .Size = sizeof(Health), .Alignment = alignof(Health)}};

    YULDUZ_Type types[3];
    for (uint32_t i = 0; i < 3; i++) {
        YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &descs[i], &types[i], 1);
    }
    YULDUZ_SDL_SortTypes(types, 3);

    // Create archetype
    YULDUZ_TypeInfo type_infos[3];
    for (uint32_t i = 0; i < 3; i++) {
        type_infos[i].Type      = types[i];
        type_infos[i].Size      = descs[i].Size;
        type_infos[i].Alignment = descs[i].Alignment;
    }
    YULDUZ_SDL_SortTypeInfos(type_infos, 3);

    YULDUZ_Archetype archetype = {0};
    YULDUZ_InitializeArchetype(&archetype, type_infos, 3, NULL, 0, 16);

    // Create query
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);
    YULDUZ_SetQueryWithComponentType(&query, types[0], YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&query, types[1], YULDUZ_QueryAccessType_Write);

    YULDUZ_QueryInfo info = {0};
    YULDUZ_CreateQueryInfo(&info, &query);

    BENCHMARK_START("Query - Archetype matching 1,000,000 times");

    uint32_t match_count = 0;
    for (uint32_t i = 0; i < 1000000; i++) {
        if (YULDUZ_ArchetypeSupportsQueryInfo(&info, &archetype)) {
            match_count++;
        }
    }

    YULDUZ_LOG_INFO("  Matches: %u", match_count);

    YULDUZ_DestroyQueryInfo(&info);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);

    BENCHMARK_END();
}

void benchmark_query_deep_copy(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)}};

    YULDUZ_Type types[2];
    for (uint32_t i = 0; i < 2; i++) {
        YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &descs[i], &types[i], 1);
    }

    YULDUZ_Query src_query = {0};
    YULDUZ_InitializeQuery(&src_query, 16);
    YULDUZ_SetQueryWithComponentType(&src_query, types[0], YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&src_query, types[1], YULDUZ_QueryAccessType_Write);

    BENCHMARK_START("Query - Deep copy 50,000 times");

    for (uint32_t i = 0; i < 50000; i++) {
        YULDUZ_Query dst_query = {0};
        YULDUZ_DeepCopyQuery(&src_query, &dst_query);
        YULDUZ_ReleaseQuery(&dst_query);
    }

    YULDUZ_ReleaseQuery(&src_query);
    YULDUZ_ReleaseTypeRegistry(&type_registry);

    BENCHMARK_END();
}

// ============================================================================
// System Benchmarks
// ============================================================================

typedef struct BenchUserData {
    uint32_t call_count;
    uint32_t entity_count;
} BenchUserData;

void BenchSystem_Empty(const YULDUZ_Archetype *archetype, const YULDUZ_QueryInfo *query, void *user_data) {
    (void)archetype;
    (void)query;
    (void)user_data;
}

void BenchSystem_Count(const YULDUZ_Archetype *archetype, const YULDUZ_QueryInfo *query, void *user_data) {
    (void)query;
    BenchUserData *data = (BenchUserData *)user_data;
    data->call_count++;
    data->entity_count += archetype->DenseCount;
}

void BenchSystem_ProcessComponents(const YULDUZ_Archetype *archetype, const YULDUZ_QueryInfo *query, void *user_data) {
    (void)user_data;

    Position *positions  = NULL;
    Velocity *velocities = NULL;

    for (uint32_t i = 0; i < archetype->StoreCount; i++) {
        for (uint32_t q = 0; q < query->WithComponentCount; q++) {
            if (archetype->Stores[i].Type == query->WithComponentTypes[q]) {
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
            positions[i].x += velocities[i].x * 0.016f;
            positions[i].y += velocities[i].y * 0.016f;
            positions[i].z += velocities[i].z * 0.016f;
        }
    }
}

void benchmark_system_initialization(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};
    YULDUZ_Type type;
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &desc, &type, 1);

    BENCHMARK_START("System - Initialize/Release 10,000 systems");

    for (uint32_t i = 0; i < 10000; i++) {
        YULDUZ_Query query = {0};
        YULDUZ_InitializeQuery(&query, 16);
        YULDUZ_SetQueryWithComponentType(&query, type, YULDUZ_QueryAccessType_Read);

        YULDUZ_System system = {0};
        YULDUZ_InitializeSystem(&system, "BenchSystem", &query, &BenchSystem_Empty);

        YULDUZ_ReleaseSystem(&system);
        YULDUZ_ReleaseQuery(&query);
    }

    YULDUZ_ReleaseTypeRegistry(&type_registry);

    BENCHMARK_END();
}

void benchmark_system_iteration_empty_entities(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 100000, 32, 64};
    YULDUZ_InitializeECSRegistry(&registry, info);

    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};
    YULDUZ_Type type;
    YULDUZ_RegisterTypeInECSRegistry(&registry, desc, &type);

    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);
    YULDUZ_SetQueryWithComponentType(&query, type, YULDUZ_QueryAccessType_Read);

    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "BenchSystem", &query, &BenchSystem_Count);

    // Create 10,000 entities
    for (uint32_t i = 0; i < 10000; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos = {0};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
    }

    BENCHMARK_START("System - Iterate 10,000 entities x 1000 times");

    BenchUserData user_data = {0};
    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_RunSystem(&system, &registry, &user_data);
    }

    YULDUZ_LOG_INFO("  Total entities processed: %u", user_data.entity_count);

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&registry);

    BENCHMARK_END();
}

void benchmark_system_component_processing(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 100000, 32, 64};
    YULDUZ_InitializeECSRegistry(&registry, info);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)}};

    YULDUZ_Type types[2];
    for (uint32_t i = 0; i < 2; i++) {
        YULDUZ_RegisterTypeInECSRegistry(&registry, descs[i], &types[i]);
    }

    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);
    YULDUZ_SetQueryWithComponentType(&query, types[0], YULDUZ_QueryAccessType_Write);
    YULDUZ_SetQueryWithComponentType(&query, types[1], YULDUZ_QueryAccessType_Read);

    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "MovementSystem", &query, &BenchSystem_ProcessComponents);

    // Create 10,000 entities with Position and Velocity
    for (uint32_t i = 0; i < 10000; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos = {0.0f, 0.0f, 0.0f};
        Velocity vel = {1.0f, 1.0f, 1.0f};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Velocity", &vel);
    }

    BENCHMARK_START("System - Process 10,000 entities (movement) x 1000 frames");

    BenchUserData user_data = {0};
    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_RunSystem(&system, &registry, &user_data);
    }

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&registry);

    BENCHMARK_END();
}

void benchmark_system_with_filtering(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 100000, 32, 64};
    YULDUZ_InitializeECSRegistry(&registry, info);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Player", .Size = 0, .Alignment = 0},
        {.Name = "Dead", .Size = 0, .Alignment = 0}};

    YULDUZ_Type types[3];
    for (uint32_t i = 0; i < 3; i++) {
        YULDUZ_RegisterTypeInECSRegistry(&registry, descs[i], &types[i]);
    }

    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);
    YULDUZ_SetQueryWithComponentType(&query, types[0], YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithTagType(&query, types[1]);
    YULDUZ_SetQueryWithoutTagType(&query, types[2]);

    YULDUZ_System system = {0};
    YULDUZ_InitializeSystem(&system, "PlayerSystem", &query, &BenchSystem_Count);

    // Create 1000 alive players
    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos = {0};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
        YULDUZ_AddTagInECSRegistry(&registry, entity, "Player");
    }

    // Create 500 dead players
    for (uint32_t i = 0; i < 500; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos = {0};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
        YULDUZ_AddTagInECSRegistry(&registry, entity, "Player");
        YULDUZ_AddTagInECSRegistry(&registry, entity, "Dead");
    }

    // Create 8500 other entities
    for (uint32_t i = 0; i < 8500; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);
        Position pos = {0};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);
    }

    BENCHMARK_START("System - Filter 10,000 entities (1000 match) x 1000 times");

    BenchUserData user_data = {0};
    for (uint32_t i = 0; i < 1000; i++) {
        YULDUZ_RunSystem(&system, &registry, &user_data);
    }

    YULDUZ_LOG_INFO("  Entities matched: %u", user_data.entity_count / 1000);

    YULDUZ_ReleaseSystem(&system);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseECSRegistry(&registry);

    BENCHMARK_END();
}

void benchmark_multiple_systems(void) {
    YULDUZ_ECSRegistry               registry = {0};
    YULDUZ_ECSRegistryInitializeInfo info     = {16, 100000, 32, 64};
    YULDUZ_InitializeECSRegistry(&registry, info);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)},
        {.Name = "Health", .Size = sizeof(Health), .Alignment = alignof(Health)}};

    YULDUZ_Type types[3];
    for (uint32_t i = 0; i < 3; i++) {
        YULDUZ_RegisterTypeInECSRegistry(&registry, descs[i], &types[i]);
    }

    // Create 3 systems
    YULDUZ_Query  queries[3] = {0};
    YULDUZ_System systems[3] = {0};

    // System 1: Position only
    YULDUZ_InitializeQuery(&queries[0], 16);
    YULDUZ_SetQueryWithComponentType(&queries[0], types[0], YULDUZ_QueryAccessType_Read);
    YULDUZ_InitializeSystem(&systems[0], "PosSystem", &queries[0], &BenchSystem_Count);

    // System 2: Position + Velocity
    YULDUZ_InitializeQuery(&queries[1], 16);
    YULDUZ_SetQueryWithComponentType(&queries[1], types[0], YULDUZ_QueryAccessType_Write);
    YULDUZ_SetQueryWithComponentType(&queries[1], types[1], YULDUZ_QueryAccessType_Read);
    YULDUZ_InitializeSystem(&systems[1], "MoveSystem", &queries[1], &BenchSystem_Count);

    // System 3: Health
    YULDUZ_InitializeQuery(&queries[2], 16);
    YULDUZ_SetQueryWithComponentType(&queries[2], types[2], YULDUZ_QueryAccessType_Read);
    YULDUZ_InitializeSystem(&systems[2], "HealthSystem", &queries[2], &BenchSystem_Count);

    // Create varied entities
    for (uint32_t i = 0; i < 10000; i++) {
        YULDUZ_Entity entity;
        YULDUZ_CreateEntityInECSRegistry(&registry, &entity);

        Position pos = {0};
        YULDUZ_AddComponentInECSRegistry(&registry, entity, "Position", &pos);

        if (i % 2 == 0) {
            Velocity vel = {1.0f, 0.0f, 0.0f};
            YULDUZ_AddComponentInECSRegistry(&registry, entity, "Velocity", &vel);
        }

        if (i % 3 == 0) {
            Health health = {100.0f, 100.0f};
            YULDUZ_AddComponentInECSRegistry(&registry, entity, "Health", &health);
        }
    }

    BENCHMARK_START("System - Run 3 systems on 10,000 entities x 500 frames");

    BenchUserData user_data = {0};
    for (uint32_t frame = 0; frame < 500; frame++) {
        for (uint32_t i = 0; i < 3; i++) {
            YULDUZ_RunSystem(&systems[i], &registry, &user_data);
        }
    }

    for (uint32_t i = 0; i < 3; i++) {
        YULDUZ_ReleaseSystem(&systems[i]);
        YULDUZ_ReleaseQuery(&queries[i]);
    }
    YULDUZ_ReleaseECSRegistry(&registry);

    BENCHMARK_END();
}

// ============================================================================
// Benchmark Runners
// ============================================================================

void run_all_query_benchmarks(void) {
    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("Query Benchmarks");
    YULDUZ_LOG_INFO("========================================\n");

    benchmark_query_creation();
    benchmark_query_archetype_matching();
    benchmark_query_deep_copy();
}

void run_all_system_benchmarks(void) {
    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("System Benchmarks");
    YULDUZ_LOG_INFO("========================================\n");

    benchmark_system_initialization();
    benchmark_system_iteration_empty_entities();
    benchmark_system_component_processing();
    benchmark_system_with_filtering();
    benchmark_multiple_systems();
}

void run_all_benchmarks(void) {
    YULDUZ_LOG_INFO("\n");
    YULDUZ_LOG_INFO("================================================================================");
    YULDUZ_LOG_INFO("                         ECS FRAMEWORK BENCHMARKS");
    YULDUZ_LOG_INFO("================================================================================\n");

    run_all_type_registry_benchmarks();
    run_all_entity_registry_benchmarks();
    run_all_archetype_benchmarks();
    run_all_ecs_registry_benchmarks();
    run_all_query_benchmarks();
    run_all_system_benchmarks();

    YULDUZ_LOG_INFO("\n");
    YULDUZ_LOG_INFO("================================================================================");
    YULDUZ_LOG_INFO("                      BENCHMARKS COMPLETED");
    YULDUZ_LOG_INFO("================================================================================\n");
}