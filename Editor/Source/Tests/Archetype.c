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

// Helper structures for testing
typedef struct Position {
    float x, y, z;
} Position;

typedef struct Velocity {
    float x, y, z;
} Velocity;

typedef struct Health {
    float value;
} Health;

typedef struct Name {
    char data[64];
} Name;

// ============================================================================
// Archetype Tests
// ============================================================================

bool test_archetype_initialize_release_empty(void) {
    YULDUZ_Archetype archetype = {0};

    // Initialize with no components or tags
    TEST_ASSERT(YULDUZ_InitializeArchetype(&archetype, nullptr, 0, nullptr, 0, 16),
                "Failed to initialize empty archetype");
    TEST_ASSERT(archetype.StoreCount == 0, "Should have no stores");
    TEST_ASSERT(archetype.TagCount == 0, "Should have no tags");
    TEST_ASSERT(archetype.DenseCapacity >= 16, "Capacity not set correctly");
    TEST_ASSERT(archetype.DenseCount == 0, "Should be empty");

    YULDUZ_ReleaseArchetype(&archetype);
    TEST_PASS();
}

bool test_archetype_initialize_with_components(void) {
    // Setup type registry
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)}};

    YULDUZ_Type types[2];
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, descs, types, 2);

    // Sort types for archetype
    YULDUZ_SDL_SortTypes(types, 2);

    // Create type infos
    YULDUZ_TypeInfo type_infos[2];
    for (uint32_t i = 0; i < 2; i++) {
        type_infos[i].Type      = types[i];
        type_infos[i].Size      = descs[i].Size;
        type_infos[i].Alignment = descs[i].Alignment;
    }
    YULDUZ_SDL_SortTypeInfos(type_infos, 2);

    // Initialize archetype
    YULDUZ_Archetype archetype = {0};
    TEST_ASSERT(YULDUZ_InitializeArchetype(&archetype, type_infos, 2, nullptr, 0, 16),
                "Failed to initialize archetype with components");
    TEST_ASSERT(archetype.StoreCount == 2, "Should have 2 stores");
    TEST_ASSERT(archetype.Stores != nullptr, "Stores should be allocated");

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_archetype_initialize_with_tags(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Player", .Size = 0, .Alignment = 0},
        {.Name = "Enemy", .Size = 0, .Alignment = 0}};

    YULDUZ_Type tags[2];
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, descs, tags, 2);
    YULDUZ_SDL_SortTypes(tags, 2);

    YULDUZ_Archetype archetype = {0};
    TEST_ASSERT(YULDUZ_InitializeArchetype(&archetype, nullptr, 0, tags, 2, 16),
                "Failed to initialize archetype with tags");
    TEST_ASSERT(archetype.TagCount == 2, "Should have 2 tags");
    TEST_ASSERT(archetype.Tags != nullptr, "Tags should be allocated");

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_archetype_add_entity(void) {
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
    YULDUZ_InitializeArchetype(&archetype, &type_info, 1, nullptr, 0, 16);

    // Add entity with component data
    Position            pos       = {1.0f, 2.0f, 3.0f};
    YULDUZ_TypeDataInfo data_info = {
        .Type = type,
        .Data = &pos};

    YULDUZ_ArchetypeIndex index = YULDUZ_INVALID_ARCHETYPE_INDEX;
    TEST_ASSERT(YULDUZ_AddInArchetype(&archetype, 0, &data_info, &index),
                "Failed to add entity");
    TEST_ASSERT(index != YULDUZ_INVALID_ARCHETYPE_INDEX, "Index should be valid");
    TEST_ASSERT(archetype.DenseCount == 1, "Count should be 1");

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_archetype_add_multiple_entities(void) {
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
    YULDUZ_InitializeArchetype(&archetype, &type_info, 1, nullptr, 0, 16);

    // Add multiple entities
    for (uint32_t i = 0; i < 10; i++) {
        Position            pos       = {(float)i, (float)i * 2, (float)i * 3};
        YULDUZ_TypeDataInfo data_info = {.Type = type, .Data = &pos};

        YULDUZ_ArchetypeIndex index;
        TEST_ASSERT(YULDUZ_AddInArchetype(&archetype, i, &data_info, &index),
                    "Failed to add entity");
        TEST_ASSERT(index == i, "Index should match insertion order");
    }

    TEST_ASSERT(archetype.DenseCount == 10, "Count should be 10");

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_archetype_remove_entity(void) {
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
    YULDUZ_InitializeArchetype(&archetype, &type_info, 1, nullptr, 0, 16);

    // Add entities
    for (uint32_t i = 0; i < 5; i++) {
        Position              pos       = {(float)i, (float)i, (float)i};
        YULDUZ_TypeDataInfo   data_info = {.Type = type, .Data = &pos};
        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, i, &data_info, &index);
    }

    // Remove entity at index 2
    YULDUZ_Entity moved_entity = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT(YULDUZ_RemoveInArchetype(&archetype, 2, &moved_entity),
                "Failed to remove entity");
    TEST_ASSERT(archetype.DenseCount == 4, "Count should be 4");
    TEST_ASSERT(moved_entity == 4, "Last entity should have moved");

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_archetype_get_entity(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_Archetype archetype = {0};
    YULDUZ_InitializeArchetype(&archetype, nullptr, 0, nullptr, 0, 16);

    // Manually set entities for testing
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_ArchetypeIndex index = YULDUZ_INVALID_ARCHETYPE_INDEX;
        YULDUZ_AddInArchetype(&archetype, i * 10, nullptr, &index);
    }

    // Get entities
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_Entity entity = YULDUZ_GetEntityInArchetype(&archetype, i);
        TEST_ASSERT(entity == i * 10, "Entity mismatch");
    }

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_archetype_query_store(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)}};

    YULDUZ_Type types[2];
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, descs, types, 2);
    YULDUZ_SDL_SortTypes(types, 2);

    YULDUZ_TypeInfo type_infos[2];
    for (uint32_t i = 0; i < 2; i++) {
        type_infos[i].Type      = types[i];
        type_infos[i].Size      = descs[i].Size;
        type_infos[i].Alignment = descs[i].Alignment;
    }
    YULDUZ_SDL_SortTypeInfos(type_infos, 2);

    YULDUZ_Archetype archetype = {0};
    YULDUZ_InitializeArchetype(&archetype, type_infos, 2, nullptr, 0, 16);

    // Query for Position store
    YULDUZ_ComponentStore *pos_store = YULDUZ_QueryStoreInArchetype(&archetype, types[0]);
    TEST_ASSERT(pos_store != nullptr, "Position store should exist");

    // Query for Velocity store
    YULDUZ_ComponentStore *vel_store = YULDUZ_QueryStoreInArchetype(&archetype, types[1]);
    TEST_ASSERT(vel_store != nullptr, "Velocity store should exist");

    // Query for non-existent store
    YULDUZ_ComponentStore *null_store = YULDUZ_QueryStoreInArchetype(&archetype, 999);
    TEST_ASSERT(null_store == nullptr, "Non-existent store should return null");

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_archetype_query_tag(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Player", .Size = 0, .Alignment = 0},
        {.Name = "Enemy", .Size = 0, .Alignment = 0}};

    YULDUZ_Type tags[2];
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, descs, tags, 2);
    YULDUZ_SDL_SortTypes(tags, 2);

    YULDUZ_Archetype archetype = {0};
    YULDUZ_InitializeArchetype(&archetype, nullptr, 0, tags, 2, 16);

    // Query for existing tags
    YULDUZ_Type *player_tag = YULDUZ_QueryTagInArchetype(&archetype, tags[0]);
    TEST_ASSERT(player_tag != nullptr, "Player tag should exist");

    YULDUZ_Type *enemy_tag = YULDUZ_QueryTagInArchetype(&archetype, tags[1]);
    TEST_ASSERT(enemy_tag != nullptr, "Enemy tag should exist");

    // Query for non-existent tag
    YULDUZ_Type *null_tag = YULDUZ_QueryTagInArchetype(&archetype, 999);
    TEST_ASSERT(null_tag == nullptr, "Non-existent tag should return null");

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_archetype_capacity_growth(void) {
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
    YULDUZ_InitializeArchetype(&archetype, &type_info, 1, nullptr, 0, 4);

    uint32_t initial_capacity = archetype.DenseCapacity;

    // Add more entities than initial capacity
    for (uint32_t i = 0; i < 10; i++) {
        Position              pos       = {(float)i, (float)i, (float)i};
        YULDUZ_TypeDataInfo   data_info = {.Type = type, .Data = &pos};
        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, i, &data_info, &index);
    }

    TEST_ASSERT(archetype.DenseCapacity > initial_capacity, "Capacity should have grown");
    TEST_ASSERT(archetype.DenseCount == 10, "Should have 10 entities");

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_archetype_move_entity(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)}};

    YULDUZ_Type types[2];
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, descs, types, 2);
    YULDUZ_SDL_SortTypes(types, 2);

    // Create source archetype with Position only
    YULDUZ_TypeInfo src_info = {
        .Type      = types[0],
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Archetype src_archetype = {0};
    YULDUZ_InitializeArchetype(&src_archetype, &src_info, 1, nullptr, 0, 16);

    // Create destination archetype with Position and Velocity
    YULDUZ_TypeInfo dst_infos[2];
    for (uint32_t i = 0; i < 2; i++) {
        dst_infos[i].Type      = types[i];
        dst_infos[i].Size      = descs[i].Size;
        dst_infos[i].Alignment = descs[i].Alignment;
    }
    YULDUZ_SDL_SortTypeInfos(dst_infos, 2);

    YULDUZ_Archetype dst_archetype = {0};
    YULDUZ_InitializeArchetype(&dst_archetype, dst_infos, 2, nullptr, 0, 16);

    // Add entity to source
    Position              pos      = {1.0f, 2.0f, 3.0f};
    YULDUZ_TypeDataInfo   src_data = {.Type = types[0], .Data = &pos};
    YULDUZ_ArchetypeIndex src_index;
    YULDUZ_AddInArchetype(&src_archetype, 42, &src_data, &src_index);

    // Move entity to destination with additional component
    Velocity            vel        = {10.0f, 20.0f, 30.0f};
    YULDUZ_TypeDataInfo dst_data[] = {
        {.Type = types[0], .Data = &pos},
        {.Type = types[1], .Data = &vel}};
    YULDUZ_SDL_SortTypeDataInfos(dst_data, 2);

    YULDUZ_ArchetypeIndex dst_index;
    YULDUZ_Entity         moved_entity;
    TEST_ASSERT(YULDUZ_MoveEntityInArchetype(&src_archetype, &dst_archetype,
                                             dst_data, 2, src_index, &dst_index, &moved_entity),
                "Failed to move entity");

    TEST_ASSERT(src_archetype.DenseCount == 0, "Source should be empty");
    TEST_ASSERT(dst_archetype.DenseCount == 1, "Destination should have 1 entity");

    YULDUZ_ReleaseArchetype(&src_archetype);
    YULDUZ_ReleaseArchetype(&dst_archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_archetype_component_data_integrity(void) {
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
    YULDUZ_InitializeArchetype(&archetype, &type_info, 1, nullptr, 0, 16);

    // Add entities with specific data
    Position positions[5];
    for (uint32_t i = 0; i < 5; i++) {
        positions[i]                    = (Position){(float)i, (float)(i * 2), (float)(i * 3)};
        YULDUZ_TypeDataInfo   data_info = {.Type = type, .Data = &positions[i]};
        YULDUZ_ArchetypeIndex index;
        YULDUZ_AddInArchetype(&archetype, i, &data_info, &index);
    }

    // Verify data integrity
    YULDUZ_ComponentStore *store = YULDUZ_QueryStoreInArchetype(&archetype, type);
    for (uint32_t i = 0; i < 5; i++) {
        Position *stored_pos = (Position *)YULDUZ_GetComponentInComponentStore(store, i);
        TEST_ASSERT(stored_pos->x == (float)i, "X component mismatch");
        TEST_ASSERT(stored_pos->y == (float)(i * 2), "Y component mismatch");
        TEST_ASSERT(stored_pos->z == (float)(i * 3), "Z component mismatch");
    }

    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

// ============================================================================
// Test Runner
// ============================================================================

void run_all_archetype_tests(void) {
    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("Running Archetype Tests");
    YULDUZ_LOG_INFO("========================================\n");

    uint32_t passed = 0;
    uint32_t total  = 0;

#define RUN_TEST(test)        \
    do {                      \
        total++;              \
        if (test()) passed++; \
    } while (0)

    RUN_TEST(test_archetype_initialize_release_empty);
    RUN_TEST(test_archetype_initialize_with_components);
    RUN_TEST(test_archetype_initialize_with_tags);
    RUN_TEST(test_archetype_add_entity);
    RUN_TEST(test_archetype_add_multiple_entities);
    RUN_TEST(test_archetype_remove_entity);
    RUN_TEST(test_archetype_get_entity);
    RUN_TEST(test_archetype_query_store);
    RUN_TEST(test_archetype_query_tag);
    RUN_TEST(test_archetype_capacity_growth);
    RUN_TEST(test_archetype_move_entity);
    RUN_TEST(test_archetype_component_data_integrity);

#undef RUN_TEST

    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("Archetype Tests: %u/%u passed", passed, total);
    YULDUZ_LOG_INFO("========================================\n");
}