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

// ============================================================================
// Query Tests
// ============================================================================

bool test_query_initialize_release(void) {
    YULDUZ_Query query = {0};

    TEST_ASSERT(YULDUZ_InitializeQuery(&query, 16),
                "Failed to initialize query");
    TEST_ASSERT(query.WithComponentCapacity >= 16, "Capacity not set correctly");
    TEST_ASSERT(query.WithComponentCount == 0, "Initial count should be 0");

    YULDUZ_ReleaseQuery(&query);
    TEST_ASSERT(query.WithComponentTypes == NULL, "Should be null after release");

    TEST_PASS();
}

bool test_query_with_component_read(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Type type;
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &desc, &type, 1);

    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);

    // Add component with read access
    TEST_ASSERT(YULDUZ_SetQueryWithComponentType(&query, type, YULDUZ_QueryAccessType_Read),
                "Failed to set query with component");
    TEST_ASSERT(query.WithComponentCount == 1, "Component count should be 1");
    TEST_ASSERT(query.WithComponentTypes[0] == type, "Component type mismatch");
    TEST_ASSERT(query.WithComponentAccessTypes[0] == YULDUZ_QueryAccessType_Read,
                "Access type should be Read");

    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_query_with_component_write(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Type type;
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &desc, &type, 1);

    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);

    // Add component with write access
    TEST_ASSERT(YULDUZ_SetQueryWithComponentType(&query, type, YULDUZ_QueryAccessType_Write),
                "Failed to set query with component");
    TEST_ASSERT(query.WithComponentAccessTypes[0] == YULDUZ_QueryAccessType_Write,
                "Access type should be Write");

    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_query_with_multiple_components(void) {
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

    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);

    // Add multiple components
    YULDUZ_SetQueryWithComponentType(&query, types[0], YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&query, types[1], YULDUZ_QueryAccessType_Write);
    YULDUZ_SetQueryWithComponentType(&query, types[2], YULDUZ_QueryAccessType_Read);

    TEST_ASSERT(query.WithComponentCount == 3, "Should have 3 components");

    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_query_with_tag(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription desc = {
        .Name      = "Player",
        .Size      = 0,
        .Alignment = 0};

    YULDUZ_Type type;
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &desc, &type, 1);

    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);

    // Add tag
    TEST_ASSERT(YULDUZ_SetQueryWithTagType(&query, type),
                "Failed to set query with tag");
    TEST_ASSERT(query.WithTagCount == 1, "Tag count should be 1");
    TEST_ASSERT(query.WithTagTypes[0] == type, "Tag type mismatch");

    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_query_with_multiple_tags(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Player", .Size = 0, .Alignment = 0},
        {.Name = "Alive", .Size = 0, .Alignment = 0},
        {.Name = "Moving", .Size = 0, .Alignment = 0}};

    YULDUZ_Type types[3];
    for (uint32_t i = 0; i < 3; i++) {
        YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &descs[i], &types[i], 1);
    }

    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);

    // Add multiple tags
    for (uint32_t i = 0; i < 3; i++) {
        YULDUZ_SetQueryWithTagType(&query, types[i]);
    }

    TEST_ASSERT(query.WithTagCount == 3, "Should have 3 tags");

    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_query_without_component(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription desc = {
        .Name      = "Dead",
        .Size      = 0,
        .Alignment = 0};

    YULDUZ_Type type;
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &desc, &type, 1);

    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);

    // Add exclusion
    TEST_ASSERT(YULDUZ_SetQueryWithoutComponentType(&query, type),
                "Failed to set query without component");
    TEST_ASSERT(query.WithoutComponentCount == 1, "Without component count should be 1");
    TEST_ASSERT(query.WithoutComponentTypes[0] == type, "Component type mismatch");

    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_query_without_tag(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription desc = {
        .Name      = "Dead",
        .Size      = 0,
        .Alignment = 0};

    YULDUZ_Type type;
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &desc, &type, 1);

    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);

    // Add tag exclusion
    TEST_ASSERT(YULDUZ_SetQueryWithoutTagType(&query, type),
                "Failed to set query without tag");
    TEST_ASSERT(query.WithoutTagCount == 1, "Without tag count should be 1");
    TEST_ASSERT(query.WithoutTagTypes[0] == type, "Tag type mismatch");

    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_query_complex(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)},
        {.Name = "Player", .Size = 0, .Alignment = 0},
        {.Name = "Dead", .Size = 0, .Alignment = 0}};

    YULDUZ_Type types[4];
    for (uint32_t i = 0; i < 4; i++) {
        YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &descs[i], &types[i], 1);
    }

    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);

    // Complex query: with Position (read), Velocity (write), Player tag, without Dead tag
    YULDUZ_SetQueryWithComponentType(&query, types[0], YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&query, types[1], YULDUZ_QueryAccessType_Write);
    YULDUZ_SetQueryWithTagType(&query, types[2]);
    YULDUZ_SetQueryWithoutTagType(&query, types[3]);

    TEST_ASSERT(query.WithComponentCount == 2, "Should have 2 components");
    TEST_ASSERT(query.WithTagCount == 1, "Should have 1 tag");
    TEST_ASSERT(query.WithoutTagCount == 1, "Should exclude 1 tag");

    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_query_deep_copy(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Type type;
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &desc, &type, 1);

    YULDUZ_Query src_query = {0};
    YULDUZ_InitializeQuery(&src_query, 16);
    YULDUZ_SetQueryWithComponentType(&src_query, type, YULDUZ_QueryAccessType_Write);

    YULDUZ_Query dst_query = {0};
    TEST_ASSERT(YULDUZ_DeepCopyQuery(&src_query, &dst_query),
                "Failed to deep copy query");

    TEST_ASSERT(dst_query.WithComponentCount == src_query.WithComponentCount,
                "Component count mismatch");
    TEST_ASSERT(dst_query.WithComponentTypes[0] == src_query.WithComponentTypes[0],
                "Component type mismatch");
    TEST_ASSERT(dst_query.WithComponentAccessTypes[0] == src_query.WithComponentAccessTypes[0],
                "Access type mismatch");

    YULDUZ_ReleaseQuery(&src_query);
    YULDUZ_ReleaseQuery(&dst_query);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_query_info_creation(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Type type;
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &desc, &type, 1);

    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);
    YULDUZ_SetQueryWithComponentType(&query, type, YULDUZ_QueryAccessType_Read);

    YULDUZ_QueryInfo info = {0};
    TEST_ASSERT(YULDUZ_CreateQueryInfo(&info, &query),
                "Failed to create query info");
    TEST_ASSERT(info.WithComponentCount == 1, "Component count should be 1");
    TEST_ASSERT(info.WithComponentTypes[0] == type, "Component type mismatch");

    YULDUZ_DestroyQueryInfo(&info);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_query_archetype_matching_simple(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Type type;
    YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &desc, &type, 1);

    // Create archetype with Position
    YULDUZ_TypeInfo type_info = {
        .Type      = type,
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Archetype archetype = {0};
    YULDUZ_InitializeArchetype(&archetype, &type_info, 1, NULL, 0, 16);

    // Create query for Position
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);
    YULDUZ_SetQueryWithComponentType(&query, type, YULDUZ_QueryAccessType_Read);

    YULDUZ_QueryInfo info = {0};
    YULDUZ_CreateQueryInfo(&info, &query);

    // Test matching
    TEST_ASSERT(YULDUZ_ArchetypeSupportsQueryInfo(&info, &archetype),
                "Archetype should match query");

    YULDUZ_DestroyQueryInfo(&info);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_query_archetype_matching_multiple_components(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)}};

    YULDUZ_Type types[2];
    for (uint32_t i = 0; i < 2; i++) {
        YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &descs[i], &types[i], 1);
    }
    YULDUZ_SDL_SortTypes(types, 2);

    // Create archetype with Position and Velocity
    YULDUZ_TypeInfo type_infos[2];
    for (uint32_t i = 0; i < 2; i++) {
        type_infos[i].Type      = types[i];
        type_infos[i].Size      = descs[i].Size;
        type_infos[i].Alignment = descs[i].Alignment;
    }
    YULDUZ_SDL_SortTypeInfos(type_infos, 2);

    YULDUZ_Archetype archetype = {0};
    YULDUZ_InitializeArchetype(&archetype, type_infos, 2, NULL, 0, 16);

    // Query for just Position (should match)
    YULDUZ_Query query1 = {0};
    YULDUZ_InitializeQuery(&query1, 16);
    YULDUZ_SetQueryWithComponentType(&query1, types[0], YULDUZ_QueryAccessType_Read);

    YULDUZ_QueryInfo info1 = {0};
    YULDUZ_CreateQueryInfo(&info1, &query1);
    TEST_ASSERT(YULDUZ_ArchetypeSupportsQueryInfo(&info1, &archetype),
                "Archetype should match single component query");

    // Query for Position and Velocity (should match)
    YULDUZ_Query query2 = {0};
    YULDUZ_InitializeQuery(&query2, 16);
    YULDUZ_SetQueryWithComponentType(&query2, types[0], YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&query2, types[1], YULDUZ_QueryAccessType_Write);

    YULDUZ_QueryInfo info2 = {0};
    YULDUZ_CreateQueryInfo(&info2, &query2);
    TEST_ASSERT(YULDUZ_ArchetypeSupportsQueryInfo(&info2, &archetype),
                "Archetype should match multi-component query");

    YULDUZ_DestroyQueryInfo(&info1);
    YULDUZ_DestroyQueryInfo(&info2);
    YULDUZ_ReleaseQuery(&query1);
    YULDUZ_ReleaseQuery(&query2);
    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_query_archetype_matching_with_tags(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Player", .Size = 0, .Alignment = 0}};

    YULDUZ_Type types[2];
    for (uint32_t i = 0; i < 2; i++) {
        YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &descs[i], &types[i], 1);
    }

    // Create archetype with Position component and Player tag
    YULDUZ_TypeInfo type_info = {
        .Type      = types[0],
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Type tag = types[1];

    YULDUZ_Archetype archetype = {0};
    YULDUZ_InitializeArchetype(&archetype, &type_info, 1, &tag, 1, 16);

    // Query for Position + Player tag
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);
    YULDUZ_SetQueryWithComponentType(&query, types[0], YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithTagType(&query, types[1]);

    YULDUZ_QueryInfo info = {0};
    YULDUZ_CreateQueryInfo(&info, &query);
    TEST_ASSERT(YULDUZ_ArchetypeSupportsQueryInfo(&info, &archetype),
                "Archetype should match query with tag");

    YULDUZ_DestroyQueryInfo(&info);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_query_archetype_no_match_missing_component(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = alignof(Velocity)}};

    YULDUZ_Type types[2];
    for (uint32_t i = 0; i < 2; i++) {
        YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &descs[i], &types[i], 1);
    }

    // Archetype with only Position
    YULDUZ_TypeInfo type_info = {
        .Type      = types[0],
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};

    YULDUZ_Archetype archetype = {0};
    YULDUZ_InitializeArchetype(&archetype, &type_info, 1, NULL, 0, 16);

    // Query for Position and Velocity (Velocity is missing)
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);
    YULDUZ_SetQueryWithComponentType(&query, types[0], YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithComponentType(&query, types[1], YULDUZ_QueryAccessType_Read);

    YULDUZ_QueryInfo info = {0};
    YULDUZ_CreateQueryInfo(&info, &query);
    TEST_ASSERT(!YULDUZ_ArchetypeSupportsQueryInfo(&info, &archetype),
                "Archetype should not match query (missing component)");

    YULDUZ_DestroyQueryInfo(&info);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

bool test_query_archetype_exclusion(void) {
    YULDUZ_TypeRegistry type_registry = {0};
    YULDUZ_InitializeTypeRegistry(&type_registry, 16);

    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = alignof(Position)},
        {.Name = "Dead", .Size = 0, .Alignment = 0}};

    YULDUZ_Type types[2];
    for (uint32_t i = 0; i < 2; i++) {
        YULDUZ_RegisterTypesInTypeRegistry(&type_registry, &descs[i], &types[i], 1);
    }

    // Archetype with Position and Dead tag
    YULDUZ_TypeInfo type_info = {
        .Type      = types[0],
        .Size      = sizeof(Position),
        .Alignment = alignof(Position)};
    YULDUZ_Type tag = types[1];

    YULDUZ_Archetype archetype = {0};
    YULDUZ_InitializeArchetype(&archetype, &type_info, 1, &tag, 1, 16);

    // Query for Position WITHOUT Dead tag
    YULDUZ_Query query = {0};
    YULDUZ_InitializeQuery(&query, 16);
    YULDUZ_SetQueryWithComponentType(&query, types[0], YULDUZ_QueryAccessType_Read);
    YULDUZ_SetQueryWithoutTagType(&query, types[1]);

    YULDUZ_QueryInfo info = {0};
    YULDUZ_CreateQueryInfo(&info, &query);
    TEST_ASSERT(!YULDUZ_ArchetypeSupportsQueryInfo(&info, &archetype),
                "Archetype should not match (has excluded tag)");

    YULDUZ_DestroyQueryInfo(&info);
    YULDUZ_ReleaseQuery(&query);
    YULDUZ_ReleaseArchetype(&archetype);
    YULDUZ_ReleaseTypeRegistry(&type_registry);
    TEST_PASS();
}

// ============================================================================
// Test Runner
// ============================================================================

void run_all_query_tests(void) {
    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("Running Query Tests");
    YULDUZ_LOG_INFO("========================================\n");

    uint32_t passed = 0;
    uint32_t total  = 0;

#define RUN_TEST(test)        \
    do {                      \
        total++;              \
        if (test()) passed++; \
    } while (0)

    RUN_TEST(test_query_initialize_release);
    RUN_TEST(test_query_with_component_read);
    RUN_TEST(test_query_with_component_write);
    RUN_TEST(test_query_with_multiple_components);
    RUN_TEST(test_query_with_tag);
    RUN_TEST(test_query_with_multiple_tags);
    RUN_TEST(test_query_without_component);
    RUN_TEST(test_query_without_tag);
    RUN_TEST(test_query_complex);
    RUN_TEST(test_query_deep_copy);
    RUN_TEST(test_query_info_creation);
    RUN_TEST(test_query_archetype_matching_simple);
    RUN_TEST(test_query_archetype_matching_multiple_components);
    RUN_TEST(test_query_archetype_matching_with_tags);
    RUN_TEST(test_query_archetype_no_match_missing_component);
    RUN_TEST(test_query_archetype_exclusion);

#undef RUN_TEST

    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("Query Tests: %u/%u passed", passed, total);
    YULDUZ_LOG_INFO("========================================\n");
}