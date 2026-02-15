#include <Tests/Tests.h>

// ============================================================================
// TYPE REGISTRY TESTS
// ============================================================================

void run_all_type_registry_tests(void) {
    YULDUZ_LOG_INFO("\n");
    YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
    YULDUZ_LOG_INFO("║           TYPE REGISTRY TESTS                                  ║");
    YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");
    YULDUZ_LOG_INFO("\n");

    test_type_registry_basic();
    test_type_registry_batch_registration();
    test_type_registry_sorting();
}

void test_type_registry_basic(void) {
    TEST_START("Type Registry Basic Operations");

    YULDUZ_TypeRegistry registry = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeTypeRegistry(&registry, 4),
                  "Failed to initialize type registry");

    // Register a single type
    YULDUZ_TypeDescription position_desc = {
        .Name = "Position",
        .Size = sizeof(Position),
        .Alignment = _Alignof(Position)
    };

    YULDUZ_Type position_type;
    YULDUZ_ASSERT(YULDUZ_RegisterTypesInTypeRegistry(&registry, &position_desc, &position_type, 1),
                  "Failed to register Position type");

    YULDUZ_ASSERT(position_type != YULDUZ_INVALID_COMPONENT_TYPE,
                  "Position type should be valid");
    YULDUZ_ASSERT(registry.DenseCount == 1, "Registry should have 1 type");

    // Get type by name
    YULDUZ_Type retrieved_type;
    const char *name = "Position";
    YULDUZ_ASSERT(YULDUZ_GetTypesInTypeRegistry(&registry, &name, &retrieved_type, 1),
                  "Failed to get Position type");
    YULDUZ_ASSERT(retrieved_type == position_type, "Retrieved type should match");

    // Get type info
    YULDUZ_TypeInfo info;
    YULDUZ_ASSERT(YULDUZ_GetTypeInfosInTypeRegistry(&registry, &name, &info, 1),
                  "Failed to get type info");
    YULDUZ_ASSERT(info.Type == position_type, "Type should match");
    YULDUZ_ASSERT(info.Size == sizeof(Position), "Size should match");
    YULDUZ_ASSERT(info.Alignment == _Alignof(Position), "Alignment should match");

    // Get type description
    YULDUZ_TypeDescription retrieved_desc;
    YULDUZ_ASSERT(YULDUZ_GetTypeDescriptionsInTypeRegistry(&registry, &position_type,
                                                            &retrieved_desc, 1),
                  "Failed to get type description");
    YULDUZ_ASSERT(SDL_strcmp(retrieved_desc.Name, "Position") == 0, "Name should match");
    YULDUZ_ASSERT(retrieved_desc.Size == sizeof(Position), "Size should match");

    YULDUZ_ReleaseTypeRegistry(&registry);

    TEST_END("Type Registry Basic Operations");
}

void test_type_registry_batch_registration(void) {
    TEST_START("Type Registry Batch Registration");

    YULDUZ_TypeRegistry registry = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeTypeRegistry(&registry, 8),
                  "Failed to initialize type registry");

    // Register multiple types at once
    YULDUZ_TypeDescription descriptions[] = {
        {.Name = "Position", .Size = sizeof(Position), .Alignment = _Alignof(Position)},
        {.Name = "Velocity", .Size = sizeof(Velocity), .Alignment = _Alignof(Velocity)},
        {.Name = "Health", .Size = sizeof(Health), .Alignment = _Alignof(Health)},
        {.Name = "Damage", .Size = sizeof(Damage), .Alignment = _Alignof(Damage)}
    };

    YULDUZ_Type types[4];
    YULDUZ_ASSERT(YULDUZ_RegisterTypesInTypeRegistry(&registry, descriptions, types, 4),
                  "Failed to register types in batch");

    YULDUZ_ASSERT(registry.DenseCount == 4, "Registry should have 4 types");

    // Verify all types are valid and unique
    for (uint32_t i = 0; i < 4; i++) {
        YULDUZ_ASSERT(types[i] != YULDUZ_INVALID_COMPONENT_TYPE,
                      "Type %u should be valid", i);

        for (uint32_t j = i + 1; j < 4; j++) {
            YULDUZ_ASSERT(types[i] != types[j], "Types should be unique");
        }
    }

    // Get all type infos in batch
    const char *names[] = {"Position", "Velocity", "Health", "Damage"};
    YULDUZ_TypeInfo infos[4];
    YULDUZ_ASSERT(YULDUZ_GetTypeInfosInTypeRegistry(&registry, names, infos, 4),
                  "Failed to get type infos");

    YULDUZ_ASSERT(infos[0].Size == sizeof(Position), "Position size should match");
    YULDUZ_ASSERT(infos[1].Size == sizeof(Velocity), "Velocity size should match");
    YULDUZ_ASSERT(infos[2].Size == sizeof(Health), "Health size should match");
    YULDUZ_ASSERT(infos[3].Size == sizeof(Damage), "Damage size should match");

    YULDUZ_ReleaseTypeRegistry(&registry);

    TEST_END("Type Registry Batch Registration");
}

void test_type_registry_sorting(void) {
    TEST_START("Type Registry Sorting Functions");

    // Test YULDUZ_Type sorting
    YULDUZ_Type types[] = {7, 2, 9, 1, 5, 3};
    uint32_t original_count = 6;

    YULDUZ_SDL_SortTypes(types, original_count);

    for (uint32_t i = 0; i < original_count - 1; i++) {
        YULDUZ_ASSERT(types[i] < types[i + 1],
                      "Types should be sorted in ascending order");
    }

    // Test YULDUZ_TypeInfo sorting
    YULDUZ_TypeInfo infos[] = {
        {.Type = 5, .Size = 12, .Alignment = 4},
        {.Type = 2, .Size = 8, .Alignment = 4},
        {.Type = 8, .Size = 4, .Alignment = 4},
        {.Type = 1, .Size = 16, .Alignment = 8},
        {.Type = 3, .Size = 1, .Alignment = 1}
    };

    YULDUZ_SDL_SortTypeInfos(infos, 5);

    for (uint32_t i = 0; i < 4; i++) {
        YULDUZ_ASSERT(infos[i].Type < infos[i + 1].Type,
                      "TypeInfos should be sorted by Type field");
    }

    // Verify data integrity after sort
    YULDUZ_ASSERT(infos[0].Type == 1 && infos[0].Size == 16, "First element data preserved");
    YULDUZ_ASSERT(infos[1].Type == 2 && infos[1].Size == 8, "Second element data preserved");
    YULDUZ_ASSERT(infos[2].Type == 3 && infos[2].Size == 1, "Third element data preserved");

    // Test YULDUZ_TypeDataInfo sorting
    float dummy1 = 1.0f, dummy2 = 2.0f, dummy3 = 3.0f, dummy4 = 4.0f;
    YULDUZ_TypeDataInfo data_infos[] = {
        {.Type = 6, .Data = &dummy1},
        {.Type = 3, .Data = &dummy2},
        {.Type = 9, .Data = &dummy3},
        {.Type = 1, .Data = &dummy4}
    };

    YULDUZ_SDL_SortTypeDataInfos(data_infos, 4);

    for (uint32_t i = 0; i < 3; i++) {
        YULDUZ_ASSERT(data_infos[i].Type < data_infos[i + 1].Type,
                      "TypeDataInfos should be sorted by Type field");
    }

    // Verify data pointers preserved
    YULDUZ_ASSERT(data_infos[0].Type == 1 && data_infos[0].Data == &dummy4,
                  "Data pointers should be preserved");
    YULDUZ_ASSERT(data_infos[1].Type == 3 && data_infos[1].Data == &dummy2,
                  "Data pointers should be preserved");

    // Test compare function directly
    YULDUZ_Type a = 5, b = 10;
    int32_t result = YULDUZ_SDL_CompareTypes(&a, &b);
    YULDUZ_ASSERT(result < 0, "5 should be less than 10");

    result = YULDUZ_SDL_CompareTypes(&b, &a);
    YULDUZ_ASSERT(result > 0, "10 should be greater than 5");

    YULDUZ_Type c = 5;
    result = YULDUZ_SDL_CompareTypes(&a, &c);
    YULDUZ_ASSERT(result == 0, "Equal types should compare as 0");

    TEST_END("Type Registry Sorting Functions");
}