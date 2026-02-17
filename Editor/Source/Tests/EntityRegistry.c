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
// Entity Registry Tests
// ============================================================================

bool test_entity_registry_initialize_release(void) {
    YULDUZ_EntityRegistry registry = {0};

    // Test initialization
    TEST_ASSERT(YULDUZ_InitializeEntityRegistry(&registry, 16),
                "Failed to initialize entity registry");
    TEST_ASSERT(registry.SparseCapacity >= 16, "Capacity not set correctly");
    TEST_ASSERT(registry.NextEntity == 0, "NextEntity should start at 0");
    TEST_ASSERT(registry.FreeListCount == 0, "FreeList should be empty");
    TEST_ASSERT(registry.Sparse != nullptr, "Sparse array should be allocated");

    // Test release
    YULDUZ_ReleaseEntityRegistry(&registry);
    TEST_ASSERT(registry.Sparse == nullptr, "Sparse should be null after release");
    TEST_ASSERT(registry.FreeList == nullptr, "FreeList should be null after release");

    TEST_PASS();
}

bool test_entity_registry_create_single_entity(void) {
    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    TEST_ASSERT(YULDUZ_CreateEntityInEntityRegistry(&registry, 0, 0, &entity),
                "Failed to create entity");
    TEST_ASSERT(entity != YULDUZ_INVALID_ENTITY, "Entity should be valid");
    TEST_ASSERT(entity == 0, "First entity should be 0");
    TEST_ASSERT(registry.NextEntity == 1, "NextEntity should be 1");

    YULDUZ_ReleaseEntityRegistry(&registry);
    TEST_PASS();
}

bool test_entity_registry_create_multiple_entities(void) {
    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    YULDUZ_Entity entities[10];
    for (uint32_t i = 0; i < 10; i++) {
        TEST_ASSERT(YULDUZ_CreateEntityInEntityRegistry(&registry, i, i * 2, &entities[i]),
                    "Failed to create entity");
        TEST_ASSERT(entities[i] == i, "Entity ID should match creation order");
    }

    TEST_ASSERT(registry.NextEntity == 10, "NextEntity should be 10");

    // Verify all entities are unique
    for (uint32_t i = 0; i < 10; i++) {
        for (uint32_t j = i + 1; j < 10; j++) {
            TEST_ASSERT(entities[i] != entities[j], "Entities should be unique");
        }
    }

    YULDUZ_ReleaseEntityRegistry(&registry);
    TEST_PASS();
}

bool test_entity_registry_destroy_entity(void) {
    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    // Create entity
    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    YULDUZ_CreateEntityInEntityRegistry(&registry, 5, 10, &entity);

    // Destroy entity
    TEST_ASSERT(YULDUZ_DestroyEntityInEntityRegistry(&registry, entity),
                "Failed to destroy entity");
    TEST_ASSERT(registry.FreeListCount == 1, "FreeList should have 1 entry");

    YULDUZ_ReleaseEntityRegistry(&registry);
    TEST_PASS();
}

bool test_entity_registry_entity_reuse(void) {
    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    // Create and destroy entity
    YULDUZ_Entity entity1 = YULDUZ_INVALID_ENTITY;
    YULDUZ_CreateEntityInEntityRegistry(&registry, 0, 0, &entity1);
    YULDUZ_DestroyEntityInEntityRegistry(&registry, entity1);

    // Create new entity - should reuse the ID
    YULDUZ_Entity entity2 = YULDUZ_INVALID_ENTITY;
    YULDUZ_CreateEntityInEntityRegistry(&registry, 1, 1, &entity2);

    TEST_ASSERT(entity2 == entity1, "Entity ID should be reused");
    TEST_ASSERT(registry.FreeListCount == 0, "FreeList should be empty after reuse");

    YULDUZ_ReleaseEntityRegistry(&registry);
    TEST_PASS();
}

bool test_entity_registry_get_entity_record(void) {
    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    // Create entity
    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    YULDUZ_CreateEntityInEntityRegistry(&registry, 7, 14, &entity);

    // Get entity record
    YULDUZ_EntityRecord record = {0};
    TEST_ASSERT(YULDUZ_GetEntityRecordsInEntityRegistry(&registry, &entity, &record, 1),
                "Failed to get entity record");
    TEST_ASSERT(record.ArchetypeType == 7, "ArchetypeType mismatch");
    TEST_ASSERT(record.ArchetypeIndex == 14, "ArchetypeIndex mismatch");

    YULDUZ_ReleaseEntityRegistry(&registry);
    TEST_PASS();
}

bool test_entity_registry_get_multiple_entity_records(void) {
    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    // Create entities
    YULDUZ_Entity entities[5];
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, i * 2, i * 3, &entities[i]);
    }

    // Get all records
    YULDUZ_EntityRecord records[5];
    TEST_ASSERT(YULDUZ_GetEntityRecordsInEntityRegistry(&registry, entities, records, 5),
                "Failed to get entity records");

    // Verify all records
    for (uint32_t i = 0; i < 5; i++) {
        TEST_ASSERT(records[i].ArchetypeType == i * 2, "ArchetypeType mismatch");
        TEST_ASSERT(records[i].ArchetypeIndex == i * 3, "ArchetypeIndex mismatch");
    }

    YULDUZ_ReleaseEntityRegistry(&registry);
    TEST_PASS();
}

bool test_entity_registry_set_entity_record(void) {
    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    // Create entity
    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    YULDUZ_CreateEntityInEntityRegistry(&registry, 0, 0, &entity);

    // Update entity record
    YULDUZ_EntityRecord new_record = {
        .ArchetypeType  = 42,
        .ArchetypeIndex = 84};
    TEST_ASSERT(YULDUZ_SetEntityRecordsInEntityRegistry(&registry, &entity, &new_record, 1),
                "Failed to set entity record");

    // Verify update
    YULDUZ_EntityRecord retrieved = {0};
    YULDUZ_GetEntityRecordsInEntityRegistry(&registry, &entity, &retrieved, 1);
    TEST_ASSERT(retrieved.ArchetypeType == 42, "ArchetypeType not updated");
    TEST_ASSERT(retrieved.ArchetypeIndex == 84, "ArchetypeIndex not updated");

    YULDUZ_ReleaseEntityRegistry(&registry);
    TEST_PASS();
}

bool test_entity_registry_set_multiple_entity_records(void) {
    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    // Create entities
    YULDUZ_Entity entities[5];
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, 0, 0, &entities[i]);
    }

    // Update all records
    YULDUZ_EntityRecord new_records[5];
    for (uint32_t i = 0; i < 5; i++) {
        new_records[i].ArchetypeType  = i * 10;
        new_records[i].ArchetypeIndex = i * 20;
    }
    TEST_ASSERT(YULDUZ_SetEntityRecordsInEntityRegistry(&registry, entities, new_records, 5),
                "Failed to set entity records");

    // Verify all updates
    YULDUZ_EntityRecord retrieved[5];
    YULDUZ_GetEntityRecordsInEntityRegistry(&registry, entities, retrieved, 5);
    for (uint32_t i = 0; i < 5; i++) {
        TEST_ASSERT(retrieved[i].ArchetypeType == i * 10, "ArchetypeType not updated");
        TEST_ASSERT(retrieved[i].ArchetypeIndex == i * 20, "ArchetypeIndex not updated");
    }

    YULDUZ_ReleaseEntityRegistry(&registry);
    TEST_PASS();
}

bool test_entity_registry_capacity_growth(void) {
    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 4);  // Small initial capacity

    uint32_t initial_capacity = registry.SparseCapacity;

    // Create more entities than initial capacity
    YULDUZ_Entity entities[10];
    for (uint32_t i = 0; i < 10; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, 0, 0, &entities[i]);
    }

    TEST_ASSERT(registry.SparseCapacity > initial_capacity, "Capacity should have grown");
    TEST_ASSERT(registry.NextEntity == 10, "Should have created 10 entities");

    YULDUZ_ReleaseEntityRegistry(&registry);
    TEST_PASS();
}

bool test_entity_registry_invalid_entity_operations(void) {
    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    // Try to destroy invalid entity
    TEST_ASSERT(!YULDUZ_DestroyEntityInEntityRegistry(&registry, YULDUZ_INVALID_ENTITY),
                "Should fail to destroy invalid entity");

    // Try to get record for invalid entity
    YULDUZ_EntityRecord record = {0};
    TEST_ASSERT(!YULDUZ_GetEntityRecordsInEntityRegistry(&registry,
                                                         &(YULDUZ_Entity){YULDUZ_INVALID_ENTITY}, &record, 1),
                "Should fail to get record for invalid entity");

    YULDUZ_ReleaseEntityRegistry(&registry);
    TEST_PASS();
}

bool test_entity_registry_destroy_nonexistent_entity(void) {
    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    // Try to destroy entity that was never created
    TEST_ASSERT(!YULDUZ_DestroyEntityInEntityRegistry(&registry, 999),
                "Should fail to destroy non-existent entity");

    YULDUZ_ReleaseEntityRegistry(&registry);
    TEST_PASS();
}

bool test_entity_registry_double_destroy(void) {
    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    // Create and destroy entity
    YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
    YULDUZ_CreateEntityInEntityRegistry(&registry, 0, 0, &entity);
    YULDUZ_DestroyEntityInEntityRegistry(&registry, entity);

    // Try to destroy again
    TEST_ASSERT(!YULDUZ_DestroyEntityInEntityRegistry(&registry, entity),
                "Should fail to destroy entity twice");

    YULDUZ_ReleaseEntityRegistry(&registry);
    TEST_PASS();
}

bool test_entity_registry_fragmentation_handling(void) {
    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    // Create 10 entities
    YULDUZ_Entity entities[10];
    for (uint32_t i = 0; i < 10; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, 0, 0, &entities[i]);
    }

    // Destroy every other entity
    for (uint32_t i = 0; i < 10; i += 2) {
        YULDUZ_DestroyEntityInEntityRegistry(&registry, entities[i]);
    }

    TEST_ASSERT(registry.FreeListCount == 5, "FreeList should have 5 entries");

    // Create 5 new entities - should reuse destroyed IDs
    YULDUZ_Entity new_entities[5];
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, 1, 1, &new_entities[i]);
    }

    TEST_ASSERT(registry.FreeListCount == 0, "FreeList should be empty after reuse");

    // Verify new entities reused old IDs
    for (uint32_t i = 0; i < 5; i++) {
        bool found = false;
        for (uint32_t j = 0; j < 10; j += 2) {
            if (new_entities[i] == entities[j]) {
                found = true;
                break;
            }
        }
        TEST_ASSERT(found, "New entity should have reused destroyed ID");
    }

    YULDUZ_ReleaseEntityRegistry(&registry);
    TEST_PASS();
}

bool test_entity_registry_stress_create_destroy(void) {
    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_InitializeEntityRegistry(&registry, 16);

    // Rapidly create and destroy entities
    for (uint32_t iteration = 0; iteration < 100; iteration++) {
        YULDUZ_Entity entity = YULDUZ_INVALID_ENTITY;
        YULDUZ_CreateEntityInEntityRegistry(&registry, iteration, iteration * 2, &entity);
        YULDUZ_DestroyEntityInEntityRegistry(&registry, entity);
    }

    TEST_ASSERT(registry.FreeListCount > 0, "FreeList should have entries");

    YULDUZ_ReleaseEntityRegistry(&registry);
    TEST_PASS();
}

// ============================================================================
// Test Runner
// ============================================================================

void run_all_entity_registry_tests(void) {
    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("Running Entity Registry Tests");
    YULDUZ_LOG_INFO("========================================\n");

    uint32_t passed = 0;
    uint32_t total  = 0;

#define RUN_TEST(test)        \
    do {                      \
        total++;              \
        if (test()) passed++; \
    } while (0)

    RUN_TEST(test_entity_registry_initialize_release);
    RUN_TEST(test_entity_registry_create_single_entity);
    RUN_TEST(test_entity_registry_create_multiple_entities);
    RUN_TEST(test_entity_registry_destroy_entity);
    RUN_TEST(test_entity_registry_entity_reuse);
    RUN_TEST(test_entity_registry_get_entity_record);
    RUN_TEST(test_entity_registry_get_multiple_entity_records);
    RUN_TEST(test_entity_registry_set_entity_record);
    RUN_TEST(test_entity_registry_set_multiple_entity_records);
    RUN_TEST(test_entity_registry_capacity_growth);
    RUN_TEST(test_entity_registry_invalid_entity_operations);
    RUN_TEST(test_entity_registry_destroy_nonexistent_entity);
    RUN_TEST(test_entity_registry_double_destroy);
    RUN_TEST(test_entity_registry_fragmentation_handling);
    RUN_TEST(test_entity_registry_stress_create_destroy);

#undef RUN_TEST

    YULDUZ_LOG_INFO("\n========================================");
    YULDUZ_LOG_INFO("Entity Registry Tests: %u/%u passed", passed, total);
    YULDUZ_LOG_INFO("========================================\n");
}