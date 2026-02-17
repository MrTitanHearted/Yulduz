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

// ============================================================================
// Type Registry Tests
// ============================================================================

bool test_type_registry_initialize_release(void) {
    YULDUZ_TypeRegistry registry = {0};

    // Test initialization
    TEST_ASSERT(YULDUZ_InitializeTypeRegistry(&registry, 16),
                "Failed to initialize type registry");
    TEST_ASSERT(registry.DenseCapacity >= 16, "Capacity not set correctly");
    TEST_ASSERT(registry.DenseCount == 0, "Initial count should be 0");
    TEST_ASSERT(registry.Dense != nullptr, "Dense array should be allocated");

    // Test release
    YULDUZ_ReleaseTypeRegistry(&registry);
    TEST_ASSERT(registry.Dense == nullptr, "Dense should be null after release");

    TEST_PASS();
}

bool test_type_registry_register_single_type(void) {
    YULDUZ_TypeRegistry registry = {0};
    YULDUZ_InitializeTypeRegistry(&registry, 16);

    // Register a single type
    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = sizeof(float) * 3,
        .Alignment = alignof(float)};

    YULDUZ_Type type = YULDUZ_INVALID_TYPE;
    TEST_ASSERT(YULDUZ_RegisterTypesInTypeRegistry(&registry, &desc, &type, 1),
                "Failed to register type");
    TEST_ASSERT(type != YULDUZ_INVALID_TYPE, "Type should be valid");
    TEST_ASSERT(registry.DenseCount == 1, "Count should be 1");

    // Verify type description
    YULDUZ_TypeDescription retrieved = {0};
    TEST_ASSERT(YULDUZ_GetTypeDescriptionsInTypeRegistry(&registry, &type, &retrieved, 1),
                "Failed to get type description");
    TEST_ASSERT(SDL_strcmp(retrieved.Name, "Position") == 0, "Name mismatch");
    TEST_ASSERT(retrieved.Size == sizeof(float) * 3, "Size mismatch");
    TEST_ASSERT(retrieved.Alignment == alignof(float), "Alignment mismatch");

    YULDUZ_ReleaseTypeRegistry(&registry);
    TEST_PASS();
}

bool test_type_registry_register_multiple_types(void) {
    YULDUZ_TypeRegistry registry = {0};
    YULDUZ_InitializeTypeRegistry(&registry, 16);

    // Register multiple types
    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = 12, .Alignment = 4},
        {.Name = "Velocity", .Size = 12, .Alignment = 4},
        {.Name = "Health", .Size = 4, .Alignment = 4},
        {.Name = "Name", .Size = 64, .Alignment = 1}};

    YULDUZ_Type types[4] = {0};
    TEST_ASSERT(YULDUZ_RegisterTypesInTypeRegistry(&registry, descs, types, 4),
                "Failed to register multiple types");
    TEST_ASSERT(registry.DenseCount == 4, "Count should be 4");

    // Verify all types are unique
    for (uint32_t i = 0; i < 4; i++) {
        TEST_ASSERT(types[i] != YULDUZ_INVALID_TYPE, "Type should be valid");
        for (uint32_t j = i + 1; j < 4; j++) {
            TEST_ASSERT(types[i] != types[j], "Types should be unique");
        }
    }

    YULDUZ_ReleaseTypeRegistry(&registry);
    TEST_PASS();
}

bool test_type_registry_get_types_by_name(void) {
    YULDUZ_TypeRegistry registry = {0};
    YULDUZ_InitializeTypeRegistry(&registry, 16);

    // Register types
    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = 12, .Alignment = 4},
        {.Name = "Velocity", .Size = 12, .Alignment = 4}};

    YULDUZ_Type registered_types[2] = {0};
    YULDUZ_RegisterTypesInTypeRegistry(&registry, descs, registered_types, 2);

    // Get types by name
    const char *names[]            = {"Position", "Velocity"};
    YULDUZ_Type retrieved_types[2] = {0};
    TEST_ASSERT(YULDUZ_GetTypesInTypeRegistry(&registry, names, retrieved_types, 2),
                "Failed to get types by name");

    TEST_ASSERT(retrieved_types[0] == registered_types[0], "Position type mismatch");
    TEST_ASSERT(retrieved_types[1] == registered_types[1], "Velocity type mismatch");

    YULDUZ_ReleaseTypeRegistry(&registry);
    TEST_PASS();
}

bool test_type_registry_get_type_infos(void) {
    YULDUZ_TypeRegistry registry = {0};
    YULDUZ_InitializeTypeRegistry(&registry, 16);

    // Register types
    YULDUZ_TypeDescription descs[] = {
        {.Name = "Position", .Size = 12, .Alignment = 4},
        {.Name = "Velocity", .Size = 8, .Alignment = 4}};

    YULDUZ_Type types[2] = {0};
    YULDUZ_RegisterTypesInTypeRegistry(&registry, descs, types, 2);

    // Get type infos
    const char     *names[]  = {"Position", "Velocity"};
    YULDUZ_TypeInfo infos[2] = {0};
    TEST_ASSERT(YULDUZ_GetTypeInfosInTypeRegistry(&registry, names, infos, 2),
                "Failed to get type infos");

    TEST_ASSERT(infos[0].Type == types[0], "Type mismatch for Position");
    TEST_ASSERT(infos[0].Size == 12, "Size mismatch for Position");
    TEST_ASSERT(infos[0].Alignment == 4, "Alignment mismatch for Position");

    TEST_ASSERT(infos[1].Type == types[1], "Type mismatch for Velocity");
    TEST_ASSERT(infos[1].Size == 8, "Size mismatch for Velocity");
    TEST_ASSERT(infos[1].Alignment == 4, "Alignment mismatch for Velocity");

    YULDUZ_ReleaseTypeRegistry(&registry);
    TEST_PASS();
}

bool test_type_registry_duplicate_registration(void) {
    YULDUZ_TypeRegistry registry = {0};
    YULDUZ_InitializeTypeRegistry(&registry, 16);

    // Register a type
    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = 12,
        .Alignment = 4};

    YULDUZ_Type type1 = YULDUZ_INVALID_TYPE;
    YULDUZ_RegisterTypesInTypeRegistry(&registry, &desc, &type1, 1);

    // Try to register the same type again
    YULDUZ_Type type2 = YULDUZ_INVALID_TYPE;
    YULDUZ_RegisterTypesInTypeRegistry(&registry, &desc, &type2, 1);

    // Should return the same type ID
    TEST_ASSERT(YULDUZ_INVALID_TYPE == type2, "Duplicate registration should return invalid type");
    TEST_ASSERT(registry.DenseCount == 1, "Count should still be 1");

    YULDUZ_ReleaseTypeRegistry(&registry);
    TEST_PASS();
}

bool test_type_registry_capacity_growth(void) {
    YULDUZ_TypeRegistry registry = {0};
    YULDUZ_InitializeTypeRegistry(&registry, 4);  // Small initial capacity

    uint32_t initial_capacity = registry.DenseCapacity;

    // Register more types than initial capacity
    char name_buffer[32];
    for (uint32_t i = 0; i < 10; i++) {
        SDL_snprintf(name_buffer, sizeof(name_buffer), "Type%u", i);
        YULDUZ_TypeDescription desc = {
            .Name      = name_buffer,
            .Size      = 4,
            .Alignment = 4};
        YULDUZ_RegisterTypesInTypeRegistry(&registry, &desc, nullptr, 1);
    }

    TEST_ASSERT(registry.DenseCount == 10, "Should have 10 types");
    TEST_ASSERT(registry.DenseCapacity > initial_capacity, "Capacity should have grown");

    YULDUZ_ReleaseTypeRegistry(&registry);
    TEST_PASS();
}

bool test_type_registry_sort_types(void) {
    YULDUZ_TypeRegistry registry = {0};
    YULDUZ_InitializeTypeRegistry(&registry, 16);

    // Register types
    YULDUZ_TypeDescription descs[] = {
        {.Name = "C", .Size = 4, .Alignment = 4},
        {.Name = "A", .Size = 4, .Alignment = 4},
        {.Name = "B", .Size = 4, .Alignment = 4}};

    YULDUZ_Type types[3] = {0};
    YULDUZ_RegisterTypesInTypeRegistry(&registry, descs, types, 3);

    // Create unsorted array
    YULDUZ_Type unsorted[] = {types[2], types[0], types[1]};

    // Sort
    YULDUZ_SDL_SortTypes(unsorted, 3);

    // Verify sorted order (should be by type ID)
    for (uint32_t i = 0; i < 2; i++) {
        TEST_ASSERT(unsorted[i] < unsorted[i + 1], "Types should be sorted");
    }

    YULDUZ_ReleaseTypeRegistry(&registry);
    TEST_PASS();
}

bool test_type_registry_sort_type_infos(void) {
    YULDUZ_TypeRegistry registry = {0};
    YULDUZ_InitializeTypeRegistry(&registry, 16);

    // Register types
    YULDUZ_TypeDescription descs[] = {
        {.Name = "C", .Size = 4, .Alignment = 4},
        {.Name = "A", .Size = 8, .Alignment = 4},
        {.Name = "B", .Size = 12, .Alignment = 4}};

    YULDUZ_Type types[3] = {0};
    YULDUZ_RegisterTypesInTypeRegistry(&registry, descs, types, 3);

    // Create type infos
    YULDUZ_TypeInfo infos[] = {
        {.Type = types[2], .Size = 12, .Alignment = 4},
        {.Type = types[0], .Size = 4, .Alignment = 4},
        {.Type = types[1], .Size = 8, .Alignment = 4}};

    // Sort
    YULDUZ_SDL_SortTypeInfos(infos, 3);

    // Verify sorted order
    for (uint32_t i = 0; i < 2; i++) {
        TEST_ASSERT(infos[i].Type < infos[i + 1].Type, "Type infos should be sorted by type");
    }

    YULDUZ_ReleaseTypeRegistry(&registry);
    TEST_PASS();
}

bool test_type_registry_sort_type_data_infos(void) {
    YULDUZ_TypeRegistry registry = {0};
    YULDUZ_InitializeTypeRegistry(&registry, 16);

    // Register types
    YULDUZ_TypeDescription descs[] = {
        {.Name = "C", .Size = 4, .Alignment = 4},
        {.Name = "A", .Size = 4, .Alignment = 4},
        {.Name = "B", .Size = 4, .Alignment = 4}};

    YULDUZ_Type types[3] = {0};
    YULDUZ_RegisterTypesInTypeRegistry(&registry, descs, types, 3);

    // Create type data infos
    uint32_t            data[3]      = {1, 2, 3};
    YULDUZ_TypeDataInfo data_infos[] = {
        {.Type = types[2], .Data = &data[2]},
        {.Type = types[0], .Data = &data[0]},
        {.Type = types[1], .Data = &data[1]}};

    // Sort
    YULDUZ_SDL_SortTypeDataInfos(data_infos, 3);

    // Verify sorted order
    for (uint32_t i = 0; i < 2; i++) {
        TEST_ASSERT(data_infos[i].Type < data_infos[i + 1].Type,
                    "Type data infos should be sorted by type");
    }

    YULDUZ_ReleaseTypeRegistry(&registry);
    TEST_PASS();
}

bool test_type_registry_null_parameters(void) {
    YULDUZ_TypeRegistry registry = {0};
    YULDUZ_InitializeTypeRegistry(&registry, 16);

    // Register a type without storing result
    YULDUZ_TypeDescription desc = {
        .Name      = "Position",
        .Size      = 12,
        .Alignment = 4};
    TEST_ASSERT(YULDUZ_RegisterTypesInTypeRegistry(&registry, &desc, nullptr, 1),
                "Should succeed even with null type output");

    YULDUZ_ReleaseTypeRegistry(&registry);
    TEST_PASS();
}

// ============================================================================
// Test Runner
// ============================================================================

void run_all_type_registry_tests(void) {
    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("Running Type Registry Tests");
    YULDUZ_LOG_INFO("========================================\n");

    uint32_t passed = 0;
    uint32_t total  = 0;

#define RUN_TEST(test)        \
    do {                      \
        total++;              \
        if (test()) passed++; \
    } while (0)

    RUN_TEST(test_type_registry_initialize_release);
    RUN_TEST(test_type_registry_register_single_type);
    RUN_TEST(test_type_registry_register_multiple_types);
    RUN_TEST(test_type_registry_get_types_by_name);
    RUN_TEST(test_type_registry_get_type_infos);
    RUN_TEST(test_type_registry_duplicate_registration);
    RUN_TEST(test_type_registry_capacity_growth);
    RUN_TEST(test_type_registry_sort_types);
    RUN_TEST(test_type_registry_sort_type_infos);
    RUN_TEST(test_type_registry_sort_type_data_infos);
    RUN_TEST(test_type_registry_null_parameters);

#undef RUN_TEST

    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("Type Registry Tests: %u/%u passed", passed, total);
    YULDUZ_LOG_INFO("========================================\n");
}