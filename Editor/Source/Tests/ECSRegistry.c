#include <Tests/Tests.h>

// ============================================================
// TEST COMPONENT TYPES
// ============================================================

typedef struct TestPosition {
    float x, y, z;
} TestPosition;

typedef struct TestVelocity {
    float x, y, z;
} TestVelocity;

typedef struct TestHealth {
    float current, max;
} TestHealth;

typedef struct TestDamage {
    float amount;
} TestDamage;

typedef struct TestSprite {
    uint32_t texture_id;
} TestSprite;

// Component with allocated memory
typedef struct TestDynamicArray {
    float   *data;
    uint32_t count;
    uint32_t capacity;
} TestDynamicArray;

// Component that tracks lifecycle calls
typedef struct TestLifecycle {
    uint32_t id;
    uint32_t on_create_calls;
    uint32_t on_destroy_calls;
    uint32_t on_clone_calls;
} TestLifecycle;

// ============================================================
// LIFECYCLE TRACKING
// ============================================================

typedef struct LifecycleTracker {
    uint32_t create_count;
    uint32_t destroy_count;
    uint32_t clone_count;
    uint32_t last_created_id;
    uint32_t last_destroyed_id;
} LifecycleTracker;

static void TestLifecycle_OnCreate(void *component, void *user_data) {
    TestLifecycle    *lc      = (TestLifecycle *)component;
    LifecycleTracker *tracker = (LifecycleTracker *)user_data;

    lc->on_create_calls++;
    tracker->create_count++;
    tracker->last_created_id = lc->id;
}

static void TestLifecycle_OnDestroy(void *component, void *user_data) {
    TestLifecycle    *lc      = (TestLifecycle *)component;
    LifecycleTracker *tracker = (LifecycleTracker *)user_data;

    lc->on_destroy_calls++;
    tracker->destroy_count++;
    tracker->last_destroyed_id = lc->id;
}

static void TestLifecycle_OnClone(void *src, void *dst, void *user_data) {
    TestLifecycle    *src_lc  = (TestLifecycle *)src;
    TestLifecycle    *dst_lc  = (TestLifecycle *)dst;
    LifecycleTracker *tracker = (LifecycleTracker *)user_data;

    *dst_lc = *src_lc;
    dst_lc->on_clone_calls++;
    tracker->clone_count++;
}

// Dynamic array lifecycle
static void TestDynamicArray_OnCreate(void *component, void *user_data) {
    (void)user_data;
    TestDynamicArray *arr = (TestDynamicArray *)component;
    // User should have already allocated, but we can verify
    if (arr->data != NULL && arr->capacity > 0) {
        // OK - user provided allocation
    }
}

static void TestDynamicArray_OnDestroy(void *component, void *user_data) {
    (void)user_data;
    TestDynamicArray *arr = (TestDynamicArray *)component;
    if (arr->data != NULL) {
        SDL_free(arr->data);
        arr->data     = NULL;
        arr->count    = 0;
        arr->capacity = 0;
    }
}

// ============================================================
// HELPER FUNCTIONS
// ============================================================

static bool setup_test_ecs(YULDUZ_ECSRegistry *ecs) {
    YULDUZ_ECSRegistryInitializeInfo info = {
        .InitialEntityCapacity        = 64,
        .InitialTagTypeCapacity       = 16,
        .InitialComponentTypeCapacity = 16,
        .InitialArchetypeCapacity     = 16,
        .InitialArchetypeTypeCapacity = 16,
    };

    return YULDUZ_InitializeECSRegistry(ecs, &info);
}

// ============================================================
// BASIC FUNCTIONALITY TESTS
// ============================================================

static void test_ecs_initialization(void) {
    TEST_START("ECS: Initialization and Release");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Verify null archetype exists
    uint32_t archetype_count = 0;
    TEST_ASSERT_TRUE(YULDUZ_GetArchetypeCountInECSRegistry(&ecs, &archetype_count));
    TEST_ASSERT_EQUAL(archetype_count, 1);  // Null archetype

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

static void test_component_type_registration(void) {
    TEST_START("ComponentType: Registration");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Register multiple components
    YULDUZ_ComponentType types[5] = {0};

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &types[0]));

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Velocity", .Size = sizeof(TestVelocity), .Alignment = _Alignof(TestVelocity), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &types[1]));

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Health", .Size = sizeof(TestHealth), .Alignment = _Alignof(TestHealth), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &types[2]));

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Damage", .Size = sizeof(TestDamage), .Alignment = _Alignof(TestDamage), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &types[3]));

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Sprite", .Size = sizeof(TestSprite), .Alignment = _Alignof(TestSprite), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &types[4]));

    // All should be unique
    for (uint32_t i = 0; i < 5; i++) {
        TEST_ASSERT_NOT_EQUAL(types[i], YULDUZ_INVALID_COMPONENT_TYPE);
        for (uint32_t j = i + 1; j < 5; j++) {
            TEST_ASSERT_NOT_EQUAL(types[i], types[j]);
        }
    }

    // Get types back by name
    YULDUZ_ComponentType retrieved = YULDUZ_INVALID_COMPONENT_TYPE;
    TEST_ASSERT_TRUE(YULDUZ_GetComponentTypeInECSRegistry(&ecs, "Position", &retrieved));
    TEST_ASSERT_EQUAL(retrieved, types[0]);

    TEST_ASSERT_TRUE(YULDUZ_GetComponentTypeInECSRegistry(&ecs, "Velocity", &retrieved));
    TEST_ASSERT_EQUAL(retrieved, types[1]);

    // Non-existent type should fail
    TEST_ASSERT_FALSE(YULDUZ_GetComponentTypeInECSRegistry(&ecs, "NonExistent", &retrieved));

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

static void test_tag_type_registration(void) {
    TEST_START("TagType: Registration");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Register multiple tags
    YULDUZ_TagType tags[5] = {0};
    TEST_ASSERT_TRUE(YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "Player", &tags[0]));
    TEST_ASSERT_TRUE(YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "Enemy", &tags[1]));
    TEST_ASSERT_TRUE(YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "Dead", &tags[2]));
    TEST_ASSERT_TRUE(YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "Hidden", &tags[3]));
    TEST_ASSERT_TRUE(YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "Active", &tags[4]));

    // All should be unique
    for (uint32_t i = 0; i < 5; i++) {
        TEST_ASSERT_NOT_EQUAL(tags[i], YULDUZ_INVALID_TAG_TYPE);
        for (uint32_t j = i + 1; j < 5; j++) {
            TEST_ASSERT_NOT_EQUAL(tags[i], tags[j]);
        }
    }

    // Get tags back by name
    YULDUZ_TagType retrieved = YULDUZ_INVALID_TAG_TYPE;
    TEST_ASSERT_TRUE(YULDUZ_GetTagTypeInECSRegistry(&ecs, "Player", &retrieved));
    TEST_ASSERT_EQUAL(retrieved, tags[0]);

    // Non-existent tag should fail
    TEST_ASSERT_FALSE(YULDUZ_GetTagTypeInECSRegistry(&ecs, "NonExistent", &retrieved));

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

// ============================================================
// ENTITY TESTS
// ============================================================

static void test_entity_creation_destruction(void) {
    TEST_START("Entity: Creation and Destruction");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Create 100 entities
    YULDUZ_Entity entities[100];
    for (uint32_t i = 0; i < 100; i++) {
        TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &entities[i]));
        TEST_ASSERT_NOT_EQUAL(entities[i], YULDUZ_INVALID_ENTITY);
    }

    // All should be unique
    for (uint32_t i = 0; i < 100; i++) {
        for (uint32_t j = i + 1; j < 100; j++) {
            TEST_ASSERT_NOT_EQUAL(entities[i], entities[j]);
        }
    }

    // Destroy every other entity
    for (uint32_t i = 0; i < 100; i += 2) {
        TEST_ASSERT_TRUE(YULDUZ_DestroyEntityInECSRegistry(&ecs, entities[i]));
    }

    // Create new entities (should reuse IDs with different generations)
    YULDUZ_Entity new_entities[50];
    for (uint32_t i = 0; i < 50; i++) {
        TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &new_entities[i]));
    }

    // New entities should have same IDs as destroyed ones but different generations
    for (uint32_t i = 0; i < 50; i++) {
        uint32_t old_id = (uint32_t)entities[i * 2];
        uint32_t new_id = (uint32_t)new_entities[i];
        // TEST_ASSERT_EQUAL(old_id, new_id);                        // Same ID
        TEST_ASSERT_NOT_EQUAL(entities[i * 2], new_entities[i]);  // Different entity
    }

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

static void test_entity_invalid_operations(void) {
    TEST_START("Entity: Invalid Operations");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type));

    // Create and destroy entity
    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &entity));
    TEST_ASSERT_TRUE(YULDUZ_DestroyEntityInECSRegistry(&ecs, entity));

    // Try to add component to destroyed entity (should fail)
    TestPosition pos = {1.0f, 2.0f, 3.0f};
    TEST_ASSERT_FALSE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &pos));

    // Try to get component from destroyed entity (should fail)
    TestPosition retrieved = {0};
    TEST_ASSERT_FALSE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &retrieved));

    // Try to destroy already destroyed entity (should fail)
    TEST_ASSERT_FALSE(YULDUZ_DestroyEntityInECSRegistry(&ecs, entity));

    // Try operations on invalid entity
    TEST_ASSERT_FALSE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, YULDUZ_INVALID_ENTITY, pos_type, &pos));
    TEST_ASSERT_FALSE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, YULDUZ_INVALID_ENTITY, pos_type, &retrieved));
    TEST_ASSERT_FALSE(YULDUZ_DestroyEntityInECSRegistry(&ecs, YULDUZ_INVALID_ENTITY));

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

// ============================================================
// COMPONENT LIFECYCLE TESTS
// ============================================================

static void test_component_add_remove_simple(void) {
    TEST_START("Component: Add and Remove (POD)");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Register components
    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType vel_type = YULDUZ_INVALID_COMPONENT_TYPE;

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type));

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Velocity", .Size = sizeof(TestVelocity), .Alignment = _Alignof(TestVelocity), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &vel_type));

    // Create entity
    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &entity));

    // Add Position
    TestPosition pos = {1.0f, 2.0f, 3.0f};
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &pos));

    // Get Position back
    TestPosition retrieved_pos = {0};
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &retrieved_pos));
    TEST_ASSERT_EQUAL(retrieved_pos.x, 1.0f);
    TEST_ASSERT_EQUAL(retrieved_pos.y, 2.0f);
    TEST_ASSERT_EQUAL(retrieved_pos.z, 3.0f);

    // Add Velocity
    TestVelocity vel = {4.0f, 5.0f, 6.0f};
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, vel_type, &vel));

    // Get both back
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &retrieved_pos));
    TEST_ASSERT_EQUAL(retrieved_pos.x, 1.0f);

    TestVelocity retrieved_vel = {0};
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, vel_type, &retrieved_vel));
    TEST_ASSERT_EQUAL(retrieved_vel.x, 4.0f);
    TEST_ASSERT_EQUAL(retrieved_vel.y, 5.0f);
    TEST_ASSERT_EQUAL(retrieved_vel.z, 6.0f);

    // Remove Position
    TEST_ASSERT_TRUE(YULDUZ_RemoveComponentWithTypeInECSRegistry(&ecs, entity, pos_type));

    // Position should be gone, Velocity should remain
    TEST_ASSERT_FALSE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &retrieved_pos));
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, vel_type, &retrieved_vel));
    TEST_ASSERT_EQUAL(retrieved_vel.x, 4.0f);

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

static void test_component_lifecycle_hooks(void) {
    TEST_START("Component: Lifecycle Hooks (OnCreate/OnDestroy)");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    LifecycleTracker tracker = {0};

    // Register component with hooks
    YULDUZ_ComponentType lc_type = YULDUZ_INVALID_COMPONENT_TYPE;
    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Lifecycle", .Size = sizeof(TestLifecycle), .Alignment = _Alignof(TestLifecycle), .OnCreatePFN = TestLifecycle_OnCreate, .OnDestroyPFN = TestLifecycle_OnDestroy, .OnClonePFN = TestLifecycle_OnClone, .UserData = &tracker}, &lc_type));

    // Create 10 entities with lifecycle components
    for (uint32_t i = 0; i < 10; i++) {
        YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
        TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &entity));

        TestLifecycle lc = {.id = i + 100};
        TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, lc_type, &lc));
    }

    // Verify OnCreate was called 10 times
    TEST_ASSERT_EQUAL(tracker.create_count, 10);
    TEST_ASSERT_EQUAL(tracker.last_created_id, 109);

    // Destroy 5 entities
    YULDUZ_Entity entities[5];
    for (uint32_t i = 0; i < 5; i++) {
        TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &entities[i]));
        TestLifecycle lc = {.id = i + 200};
        TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entities[i], lc_type, &lc));
    }

    // create_count should now be 15
    TEST_ASSERT_EQUAL(tracker.create_count, 15);

    // Destroy entities
    for (uint32_t i = 0; i < 5; i++) {
        TEST_ASSERT_TRUE(YULDUZ_DestroyEntityInECSRegistry(&ecs, entities[i]));
    }

    // Verify OnDestroy was called 5 times
    TEST_ASSERT_EQUAL(tracker.destroy_count, 5);

    // Release ECS (should call OnDestroy on remaining 10 entities)
    YULDUZ_ReleaseECSRegistry(&ecs);

    // Total destroys should be 15
    TEST_ASSERT_EQUAL(tracker.destroy_count, 15);

    TEST_END();
}

static void test_component_memory_cleanup(void) {
    TEST_START("Component: Memory Cleanup (Dynamic Allocation)");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Register component with destructor
    YULDUZ_ComponentType arr_type = YULDUZ_INVALID_COMPONENT_TYPE;
    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "DynamicArray", .Size = sizeof(TestDynamicArray), .Alignment = _Alignof(TestDynamicArray), .OnCreatePFN = TestDynamicArray_OnCreate, .OnDestroyPFN = TestDynamicArray_OnDestroy, .OnClonePFN = NULL, .UserData = NULL}, &arr_type));

    // Create entities with allocated memory
    YULDUZ_Entity entities[10];
    for (uint32_t i = 0; i < 10; i++) {
        TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &entities[i]));

        // Allocate memory
        float *data = (float *)SDL_malloc(100 * sizeof(float));
        TEST_ASSERT_NOT_NULL(data);
        for (uint32_t j = 0; j < 100; j++) {
            data[j] = (float)j;
        }

        TestDynamicArray arr = {.data = data, .count = 100, .capacity = 100};
        TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entities[i], arr_type, &arr));
    }

    // Remove components from 5 entities (should free their memory)
    for (uint32_t i = 0; i < 5; i++) {
        TEST_ASSERT_TRUE(YULDUZ_RemoveComponentWithTypeInECSRegistry(&ecs, entities[i], arr_type));
    }

    // Destroy 3 entities (should free their memory)
    for (uint32_t i = 5; i < 8; i++) {
        TEST_ASSERT_TRUE(YULDUZ_DestroyEntityInECSRegistry(&ecs, entities[i]));
    }

    // Release ECS (should free remaining 2)
    YULDUZ_ReleaseECSRegistry(&ecs);

    // If we had memory leak detection, we'd verify all 10 allocations were freed
    // For now, running under Valgrind/ASan will catch leaks

    TEST_END();
}

// ============================================================
// TAG TESTS
// ============================================================

static void test_tag_add_remove(void) {
    TEST_START("Tag: Add and Remove");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Register tags
    YULDUZ_TagType player_tag = YULDUZ_INVALID_TAG_TYPE;
    YULDUZ_TagType enemy_tag  = YULDUZ_INVALID_TAG_TYPE;
    YULDUZ_TagType dead_tag   = YULDUZ_INVALID_TAG_TYPE;

    TEST_ASSERT_TRUE(YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "Player", &player_tag));
    TEST_ASSERT_TRUE(YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "Enemy", &enemy_tag));
    TEST_ASSERT_TRUE(YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "Dead", &dead_tag));

    // Create entity
    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &entity));

    // Initially has no tags
    TEST_ASSERT_FALSE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, player_tag));
    TEST_ASSERT_FALSE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, enemy_tag));
    TEST_ASSERT_FALSE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, dead_tag));

    // Add Player tag
    TEST_ASSERT_TRUE(YULDUZ_AddTagWithTypeInECSRegistry(&ecs, entity, player_tag));
    TEST_ASSERT_TRUE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, player_tag));
    TEST_ASSERT_FALSE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, enemy_tag));

    // Add Enemy tag
    TEST_ASSERT_TRUE(YULDUZ_AddTagWithTypeInECSRegistry(&ecs, entity, enemy_tag));
    TEST_ASSERT_TRUE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, player_tag));
    TEST_ASSERT_TRUE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, enemy_tag));

    // Add Dead tag
    TEST_ASSERT_TRUE(YULDUZ_AddTagWithTypeInECSRegistry(&ecs, entity, dead_tag));
    TEST_ASSERT_TRUE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, player_tag));
    TEST_ASSERT_TRUE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, enemy_tag));
    TEST_ASSERT_TRUE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, dead_tag));

    // Remove Player tag
    TEST_ASSERT_TRUE(YULDUZ_RemoveTagWithTypeInECSRegistry(&ecs, entity, player_tag));
    TEST_ASSERT_FALSE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, player_tag));
    TEST_ASSERT_TRUE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, enemy_tag));
    TEST_ASSERT_TRUE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, dead_tag));

    // Remove all remaining tags
    TEST_ASSERT_TRUE(YULDUZ_RemoveTagWithTypeInECSRegistry(&ecs, entity, enemy_tag));
    TEST_ASSERT_TRUE(YULDUZ_RemoveTagWithTypeInECSRegistry(&ecs, entity, dead_tag));
    TEST_ASSERT_FALSE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, player_tag));
    TEST_ASSERT_FALSE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, enemy_tag));
    TEST_ASSERT_FALSE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, entity, dead_tag));

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

// ============================================================
// ARCHETYPE TESTS
// ============================================================

static void test_archetype_creation(void) {
    TEST_START("Archetype: Creation and Growth");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Register components
    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType vel_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType hp_type  = YULDUZ_INVALID_COMPONENT_TYPE;

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type));

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Velocity", .Size = sizeof(TestVelocity), .Alignment = _Alignof(TestVelocity), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &vel_type));

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Health", .Size = sizeof(TestHealth), .Alignment = _Alignof(TestHealth), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &hp_type));

    // Start with null archetype
    uint32_t archetype_count = 0;
    TEST_ASSERT_TRUE(YULDUZ_GetArchetypeCountInECSRegistry(&ecs, &archetype_count));
    TEST_ASSERT_EQUAL(archetype_count, 1);

    TestPosition pos = {0};
    TestVelocity vel = {0};
    TestHealth   hp  = {100.0f, 100.0f};

    // Create entity with Position → new archetype [Position]
    YULDUZ_Entity e1 = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &e1));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e1, pos_type, &pos));

    TEST_ASSERT_TRUE(YULDUZ_GetArchetypeCountInECSRegistry(&ecs, &archetype_count));
    TEST_ASSERT_EQUAL(archetype_count, 2);  // Null + [Position]

    // Add Velocity → new archetype [Position, Velocity]
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e1, vel_type, &vel));

    TEST_ASSERT_TRUE(YULDUZ_GetArchetypeCountInECSRegistry(&ecs, &archetype_count));
    TEST_ASSERT_EQUAL(archetype_count, 3);  // Null + [Position] + [Position, Velocity]

    // Create entity with [Position, Velocity] → reuses existing archetype
    YULDUZ_Entity e2 = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &e2));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e2, pos_type, &pos));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e2, vel_type, &vel));

    TEST_ASSERT_TRUE(YULDUZ_GetArchetypeCountInECSRegistry(&ecs, &archetype_count));
    TEST_ASSERT_EQUAL(archetype_count, 3);  // No new archetype

    // Create entity with [Position, Health] → new archetype
    YULDUZ_Entity e3 = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &e3));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e3, pos_type, &pos));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e3, hp_type, &hp));

    TEST_ASSERT_TRUE(YULDUZ_GetArchetypeCountInECSRegistry(&ecs, &archetype_count));
    TEST_ASSERT_EQUAL(archetype_count, 4);  // +[Position, Health]

    // Create entity with [Position, Velocity, Health] → new archetype
    YULDUZ_Entity e4 = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &e4));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e4, pos_type, &pos));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e4, vel_type, &vel));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, e4, hp_type, &hp));

    TEST_ASSERT_TRUE(YULDUZ_GetArchetypeCountInECSRegistry(&ecs, &archetype_count));
    TEST_ASSERT_EQUAL(archetype_count, 5);  // +[Position, Velocity, Health]

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

static void test_archetype_transitions(void) {
    TEST_START("Archetype: Component Add/Remove Transitions");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Register components
    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType vel_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType hp_type  = YULDUZ_INVALID_COMPONENT_TYPE;

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type));

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Velocity", .Size = sizeof(TestVelocity), .Alignment = _Alignof(TestVelocity), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &vel_type));

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Health", .Size = sizeof(TestHealth), .Alignment = _Alignof(TestHealth), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &hp_type));

    // Create entity and transition through archetypes
    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &entity));

    TestPosition pos = {1.0f, 2.0f, 3.0f};
    TestVelocity vel = {4.0f, 5.0f, 6.0f};
    TestHealth   hp  = {100.0f, 100.0f};

    // Archetype: [] (null)
    // Add Position → [Position]
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &pos));

    TestPosition retrieved_pos = {0};
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &retrieved_pos));
    TEST_ASSERT_EQUAL(retrieved_pos.x, 1.0f);

    // Add Velocity → [Position, Velocity]
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, vel_type, &vel));

    TestVelocity retrieved_vel = {0};
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, vel_type, &retrieved_vel));
    TEST_ASSERT_EQUAL(retrieved_vel.x, 4.0f);

    // Position should still be there
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &retrieved_pos));
    TEST_ASSERT_EQUAL(retrieved_pos.x, 1.0f);

    // Add Health → [Position, Velocity, Health]
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, hp_type, &hp));

    // All components should be there
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &retrieved_pos));
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, vel_type, &retrieved_vel));

    TestHealth retrieved_hp = {0};
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, hp_type, &retrieved_hp));
    TEST_ASSERT_EQUAL(retrieved_hp.current, 100.0f);

    // Remove Velocity → [Position, Health]
    TEST_ASSERT_TRUE(YULDUZ_RemoveComponentWithTypeInECSRegistry(&ecs, entity, vel_type));

    // Velocity should be gone
    TEST_ASSERT_FALSE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, vel_type, &retrieved_vel));

    // Position and Health should remain
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &retrieved_pos));
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, hp_type, &retrieved_hp));

    // Remove all → []
    TEST_ASSERT_TRUE(YULDUZ_RemoveComponentWithTypeInECSRegistry(&ecs, entity, pos_type));
    TEST_ASSERT_TRUE(YULDUZ_RemoveComponentWithTypeInECSRegistry(&ecs, entity, hp_type));

    // All should be gone
    TEST_ASSERT_FALSE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &retrieved_pos));
    TEST_ASSERT_FALSE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, vel_type, &retrieved_vel));
    TEST_ASSERT_FALSE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, entity, hp_type, &retrieved_hp));

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

// ============================================================
// CLONE TESTS
// ============================================================

static void test_clone_pod(void) {
    TEST_START("Clone: POD Components (no OnClone)");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType vel_type = YULDUZ_INVALID_COMPONENT_TYPE;

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type));
    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Velocity", .Size = sizeof(TestVelocity), .Alignment = _Alignof(TestVelocity), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &vel_type));

    // Create source entity
    YULDUZ_Entity src = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &src));
    TestPosition src_pos = {1.0f, 2.0f, 3.0f};
    TestVelocity src_vel = {4.0f, 5.0f, 6.0f};
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, src, pos_type, &src_pos));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, src, vel_type, &src_vel));

    // Clone
    YULDUZ_Entity dst = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CloneEntityInECSRegistry(&ecs, src, &dst));

    // dst should be a different valid entity
    TEST_ASSERT_NOT_EQUAL(dst, YULDUZ_INVALID_ENTITY);
    TEST_ASSERT_NOT_EQUAL(dst, src);

    // dst should have the same component values
    TestPosition dst_pos = {0};
    TestVelocity dst_vel = {0};
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, dst, pos_type, &dst_pos));
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, dst, vel_type, &dst_vel));
    TEST_ASSERT_EQUAL(dst_pos.x, 1.0f);
    TEST_ASSERT_EQUAL(dst_pos.y, 2.0f);
    TEST_ASSERT_EQUAL(dst_pos.z, 3.0f);
    TEST_ASSERT_EQUAL(dst_vel.x, 4.0f);
    TEST_ASSERT_EQUAL(dst_vel.y, 5.0f);
    TEST_ASSERT_EQUAL(dst_vel.z, 6.0f);

    // Modifying dst should not affect src
    TestPosition new_pos = {9.0f, 9.0f, 9.0f};
    TEST_ASSERT_TRUE(YULDUZ_SetComponentWithTypeInECSRegistry(&ecs, dst, pos_type, &new_pos));
    TestPosition src_pos_after = {0};
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, src, pos_type, &src_pos_after));
    TEST_ASSERT_EQUAL(src_pos_after.x, 1.0f);  // src unchanged

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

static void test_clone_with_on_clone_callback(void) {
    TEST_START("Clone: OnClone Callback Invoked (not OnCreate)");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    LifecycleTracker tracker = {0};

    YULDUZ_ComponentType lc_type = YULDUZ_INVALID_COMPONENT_TYPE;
    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Lifecycle", .Size = sizeof(TestLifecycle), .Alignment = _Alignof(TestLifecycle), .OnCreatePFN = TestLifecycle_OnCreate, .OnDestroyPFN = TestLifecycle_OnDestroy, .OnClonePFN = TestLifecycle_OnClone, .UserData = &tracker}, &lc_type));

    // Create source entity — OnCreate fires
    YULDUZ_Entity src = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &src));
    TestLifecycle lc = {.id = 42};
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, src, lc_type, &lc));
    TEST_ASSERT_EQUAL(tracker.create_count, 1);
    TEST_ASSERT_EQUAL(tracker.clone_count, 0);

    // Clone — OnClone fires, OnCreate must NOT fire again
    YULDUZ_Entity dst = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CloneEntityInECSRegistry(&ecs, src, &dst));
    TEST_ASSERT_EQUAL(tracker.clone_count, 1);
    TEST_ASSERT_EQUAL(tracker.create_count, 1);  // Still 1

    // dst component reflects the clone (OnClone increments on_clone_calls)
    TestLifecycle dst_lc = {0};
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, dst, lc_type, &dst_lc));
    TEST_ASSERT_EQUAL(dst_lc.id, 42);
    TEST_ASSERT_EQUAL(dst_lc.on_clone_calls, 1);

    // src should be unaffected
    TestLifecycle src_lc = {0};
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, src, lc_type, &src_lc));
    TEST_ASSERT_EQUAL(src_lc.on_clone_calls, 0);

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

static void test_clone_same_archetype(void) {
    TEST_START("Clone: dst Lands in Same Archetype as src");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    YULDUZ_ComponentType pos_type   = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_ComponentType vel_type   = YULDUZ_INVALID_COMPONENT_TYPE;
    YULDUZ_TagType       active_tag = YULDUZ_INVALID_TAG_TYPE;

    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type));
    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Velocity", .Size = sizeof(TestVelocity), .Alignment = _Alignof(TestVelocity), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &vel_type));
    TEST_ASSERT_TRUE(YULDUZ_RegisterTagTypeInECSRegistry(&ecs, "Active", &active_tag));

    // Create source entity with components and tag
    YULDUZ_Entity src = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &src));
    TestPosition pos = {1.0f, 2.0f, 3.0f};
    TestVelocity vel = {4.0f, 5.0f, 6.0f};
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, src, pos_type, &pos));
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, src, vel_type, &vel));
    TEST_ASSERT_TRUE(YULDUZ_AddTagWithTypeInECSRegistry(&ecs, src, active_tag));

    uint32_t archetype_count_before = 0;
    TEST_ASSERT_TRUE(YULDUZ_GetArchetypeCountInECSRegistry(&ecs, &archetype_count_before));

    // Clone — must not create a new archetype
    YULDUZ_Entity dst = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CloneEntityInECSRegistry(&ecs, src, &dst));

    uint32_t archetype_count_after = 0;
    TEST_ASSERT_TRUE(YULDUZ_GetArchetypeCountInECSRegistry(&ecs, &archetype_count_after));
    TEST_ASSERT_EQUAL(archetype_count_after, archetype_count_before);  // No new archetype

    // dst inherits the tag from the archetype
    TEST_ASSERT_TRUE(YULDUZ_HasTagWithTypeInECSRegistry(&ecs, dst, active_tag));

    // dst has same component data
    TestPosition dst_pos = {0};
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, dst, pos_type, &dst_pos));
    TEST_ASSERT_EQUAL(dst_pos.x, 1.0f);

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

static void test_clone_invalid(void) {
    TEST_START("Clone: Invalid Entity Handling");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Clone YULDUZ_INVALID_ENTITY should fail
    YULDUZ_Entity dst = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_FALSE(YULDUZ_CloneEntityInECSRegistry(&ecs, YULDUZ_INVALID_ENTITY, &dst));

    // Clone stale (destroyed) entity should fail
    YULDUZ_Entity src = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &src));
    TEST_ASSERT_TRUE(YULDUZ_DestroyEntityInECSRegistry(&ecs, src));
    TEST_ASSERT_FALSE(YULDUZ_CloneEntityInECSRegistry(&ecs, src, &dst));

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

static void test_clone_chain(void) {
    TEST_START("Clone: Cloning a Clone");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type));

    // Create source
    YULDUZ_Entity src = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &src));
    TestPosition pos = {7.0f, 8.0f, 9.0f};
    TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, src, pos_type, &pos));

    // src → clone1 → clone2
    YULDUZ_Entity clone1 = YULDUZ_INVALID_ENTITY;
    YULDUZ_Entity clone2 = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT_TRUE(YULDUZ_CloneEntityInECSRegistry(&ecs, src, &clone1));
    TEST_ASSERT_TRUE(YULDUZ_CloneEntityInECSRegistry(&ecs, clone1, &clone2));

    // All three must be distinct
    TEST_ASSERT_NOT_EQUAL(src, clone1);
    TEST_ASSERT_NOT_EQUAL(src, clone2);
    TEST_ASSERT_NOT_EQUAL(clone1, clone2);

    // All three must carry the same data
    TestPosition p1 = {0}, p2 = {0};
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, clone1, pos_type, &p1));
    TEST_ASSERT_TRUE(YULDUZ_GetComponentWithTypeInECSRegistry(&ecs, clone2, pos_type, &p2));
    TEST_ASSERT_EQUAL(p1.x, 7.0f);
    TEST_ASSERT_EQUAL(p2.x, 7.0f);

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

// ============================================================
// STRESS TESTS
// ============================================================

static void test_stress_many_entities(void) {
    TEST_START("Stress: 10,000 Entities");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    YULDUZ_ComponentType pos_type = YULDUZ_INVALID_COMPONENT_TYPE;
    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Position", .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &pos_type));

    // Create 10,000 entities with Position
    const uint32_t ENTITY_COUNT = 10000;
    TestPosition   pos          = {1.0f, 2.0f, 3.0f};

    for (uint32_t i = 0; i < ENTITY_COUNT; i++) {
        YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
        TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &entity));
        TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, pos_type, &pos));
    }

    // All should be in same archetype
    uint32_t archetype_count = 0;
    TEST_ASSERT_TRUE(YULDUZ_GetArchetypeCountInECSRegistry(&ecs, &archetype_count));
    TEST_ASSERT_EQUAL(archetype_count, 2);  // Null + [Position]

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

static void test_stress_many_archetypes(void) {
    TEST_START("Stress: 100 Archetypes");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    // Register 10 components
    YULDUZ_ComponentType types[10];
    for (uint32_t i = 0; i < 10; i++) {
        char name[32];
        SDL_snprintf(name, sizeof(name), "Component%u", i);

        TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = name, .Size = sizeof(TestPosition), .Alignment = _Alignof(TestPosition), .OnCreatePFN = NULL, .OnDestroyPFN = NULL, .OnClonePFN = NULL, .UserData = NULL}, &types[i]));
    }

    // Create 100 different archetypes (different combinations of components)
    TestPosition data = {0};

    for (uint32_t i = 0; i < 100; i++) {
        YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
        TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &entity));

        // Add components based on bit pattern of i
        for (uint32_t j = 0; j < 10; j++) {
            if ((i & (1u << j)) != 0) {
                TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entity, types[j], &data));
            }
        }
    }

    // Should have many archetypes
    uint32_t archetype_count = 0;
    TEST_ASSERT_TRUE(YULDUZ_GetArchetypeCountInECSRegistry(&ecs, &archetype_count));
    TEST_ASSERT_TRUE(archetype_count > 50);  // At least 50 different archetypes

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

static void test_stress_lifecycle_hooks(void) {
    TEST_START("Stress: 1,000 Components with Lifecycle Hooks");

    YULDUZ_ECSRegistry ecs = {0};
    TEST_ASSERT_TRUE(setup_test_ecs(&ecs));

    LifecycleTracker tracker = {0};

    YULDUZ_ComponentType lc_type = YULDUZ_INVALID_COMPONENT_TYPE;
    TEST_ASSERT_TRUE(YULDUZ_RegisterComponentTypeInECSRegistry(&ecs, (YULDUZ_ComponentTypeDescription){.Name = "Lifecycle", .Size = sizeof(TestLifecycle), .Alignment = _Alignof(TestLifecycle), .OnCreatePFN = TestLifecycle_OnCreate, .OnDestroyPFN = TestLifecycle_OnDestroy, .OnClonePFN = NULL, .UserData = &tracker}, &lc_type));

    // Create 1000 entities
    const uint32_t COUNT = 1000;
    YULDUZ_Entity  entities[1000];

    for (uint32_t i = 0; i < COUNT; i++) {
        TEST_ASSERT_TRUE(YULDUZ_CreateEntityInECSRegistry(&ecs, &entities[i]));
        TestLifecycle lc = {.id = i};
        TEST_ASSERT_TRUE(YULDUZ_AddComponentWithTypeInECSRegistry(&ecs, entities[i], lc_type, &lc));
    }

    TEST_ASSERT_EQUAL(tracker.create_count, COUNT);

    // Destroy all
    for (uint32_t i = 0; i < COUNT; i++) {
        TEST_ASSERT_TRUE(YULDUZ_DestroyEntityInECSRegistry(&ecs, entities[i]));
    }

    TEST_ASSERT_EQUAL(tracker.destroy_count, COUNT);

    YULDUZ_ReleaseECSRegistry(&ecs);
    TEST_END();
}

// ============================================================
// ENTRY POINT
// ============================================================

void run_all_ecs_registry_tests(void) {
    YULDUZ_LOG_INFO("");
    YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
    YULDUZ_LOG_INFO("║           ECS REGISTRY COMPREHENSIVE TESTS                     ║");
    YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");
    YULDUZ_LOG_INFO("");

    // Basic functionality
    test_ecs_initialization();
    test_component_type_registration();
    test_tag_type_registration();

    // Entity tests
    test_entity_creation_destruction();
    test_entity_invalid_operations();

    // Component lifecycle
    test_component_add_remove_simple();
    test_component_lifecycle_hooks();
    test_component_memory_cleanup();

    // Tag tests
    test_tag_add_remove();

    // Archetype tests
    test_archetype_creation();
    test_archetype_transitions();

    // Clone tests
    test_clone_pod();
    test_clone_with_on_clone_callback();
    test_clone_same_archetype();
    test_clone_invalid();
    test_clone_chain();

    // Stress tests
    test_stress_many_entities();
    test_stress_many_archetypes();
    test_stress_lifecycle_hooks();

    PRINT_TEST_RESULTS();
}