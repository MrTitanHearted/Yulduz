#include <Tests/Tests.h>

// ============================================================================
// ENTITY REGISTRY TESTS
// ============================================================================

void run_all_entity_registry_tests(void) {
    YULDUZ_LOG_INFO("\n");
    YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
    YULDUZ_LOG_INFO("║           ENTITY REGISTRY TESTS                                ║");
    YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");
    YULDUZ_LOG_INFO("\n");

    test_entity_registry_create_destroy();
    test_entity_registry_reuse();
    test_entity_registry_records();
    test_entity_registry_batch_operations();
    test_entity_registry_free_list();
    test_entity_registry_capacity_growth();
    test_entity_registry_invalid_operations();
}

void test_entity_registry_create_destroy(void) {
    TEST_START("Entity Registry Create/Destroy");

    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeEntityRegistry(&registry, 8),
                  "Failed to initialize entity registry");

    // Create entities
    YULDUZ_Entity entities[5];
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_ASSERT(YULDUZ_CreateEntityInEntityRegistry(&registry, i, i * 10, &entities[i]),
                      "Failed to create entity %u", i);
        YULDUZ_ASSERT(entities[i] != YULDUZ_INVALID_ENTITY, "Entity should be valid");
    }

    // Entities should be sequential starting from 0
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_ASSERT(entities[i] == i, "Entity ID should be %u", i);
    }

    // Destroy middle entity
    YULDUZ_ASSERT(YULDUZ_DestroyEntityInEntityRegistry(&registry, entities[2]),
                  "Failed to destroy entity");

    // Verify we can't get record for destroyed entity
    YULDUZ_EntityRecord record;
    bool result = YULDUZ_GetEntityRecordsInEntityRegistry(&registry, &entities[2], &record, 1);
    YULDUZ_ASSERT(!result, "Should not be able to get record for destroyed entity");

    YULDUZ_ReleaseEntityRegistry(&registry);

    TEST_END("Entity Registry Create/Destroy");
}

void test_entity_registry_reuse(void) {
    TEST_START("Entity Registry ID Reuse");

    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeEntityRegistry(&registry, 8),
                  "Failed to initialize");

    // Create entities
    YULDUZ_Entity e1, e2, e3;
    YULDUZ_CreateEntityInEntityRegistry(&registry, 0, 0, &e1);
    YULDUZ_CreateEntityInEntityRegistry(&registry, 1, 10, &e2);
    YULDUZ_CreateEntityInEntityRegistry(&registry, 2, 20, &e3);

    YULDUZ_LOG_INFO("Created entities: %u, %u, %u", e1, e2, e3);

    // Destroy first entity
    YULDUZ_DestroyEntityInEntityRegistry(&registry, e1);

    YULDUZ_ASSERT(registry.FreeListCount == 1, "Free list should have 1 entry");

    // Create new entity - should reuse e1's ID
    YULDUZ_Entity e4;
    YULDUZ_CreateEntityInEntityRegistry(&registry, 3, 30, &e4);

    YULDUZ_ASSERT(e4 == e1, "New entity should reuse first destroyed entity's ID");
    YULDUZ_ASSERT(registry.FreeListCount == 0, "Free list should be empty after reuse");

    // Verify new entity has correct archetype assignment
    YULDUZ_EntityRecord record;
    YULDUZ_GetEntityRecordsInEntityRegistry(&registry, &e4, &record, 1);
    YULDUZ_ASSERT(record.ArchetypeType == 3, "Reused entity should have new archetype type");
    YULDUZ_ASSERT(record.ArchetypeIndex == 30, "Reused entity should have new archetype index");

    YULDUZ_ReleaseEntityRegistry(&registry);

    TEST_END("Entity Registry ID Reuse");
}

void test_entity_registry_records(void) {
    TEST_START("Entity Registry Get/Set Records");

    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeEntityRegistry(&registry, 8),
                  "Failed to initialize");

    // Create entities with different archetype assignments
    YULDUZ_Entity entities[3];
    YULDUZ_CreateEntityInEntityRegistry(&registry, 5, 100, &entities[0]);
    YULDUZ_CreateEntityInEntityRegistry(&registry, 7, 200, &entities[1]);
    YULDUZ_CreateEntityInEntityRegistry(&registry, 9, 300, &entities[2]);

    // Get records
    YULDUZ_EntityRecord records[3];
    YULDUZ_ASSERT(YULDUZ_GetEntityRecordsInEntityRegistry(&registry, entities, records, 3),
                  "Failed to get records");

    YULDUZ_ASSERT(records[0].ArchetypeType == 5 && records[0].ArchetypeIndex == 100,
                  "Record 0 should match");
    YULDUZ_ASSERT(records[1].ArchetypeType == 7 && records[1].ArchetypeIndex == 200,
                  "Record 1 should match");
    YULDUZ_ASSERT(records[2].ArchetypeType == 9 && records[2].ArchetypeIndex == 300,
                  "Record 2 should match");

    // Update records
    YULDUZ_EntityRecord new_records[3] = {
        {.ArchetypeType = 10, .ArchetypeIndex = 1000},
        {.ArchetypeType = 20, .ArchetypeIndex = 2000},
        {.ArchetypeType = 30, .ArchetypeIndex = 3000}
    };

    YULDUZ_ASSERT(YULDUZ_SetEntityRecordsInEntityRegistry(&registry, entities, new_records, 3),
                  "Failed to set records");

    // Verify updates
    YULDUZ_EntityRecord verify[3];
    YULDUZ_GetEntityRecordsInEntityRegistry(&registry, entities, verify, 3);

    YULDUZ_ASSERT(verify[0].ArchetypeType == 10 && verify[0].ArchetypeIndex == 1000,
                  "Updated record 0 should match");
    YULDUZ_ASSERT(verify[1].ArchetypeType == 20 && verify[1].ArchetypeIndex == 2000,
                  "Updated record 1 should match");
    YULDUZ_ASSERT(verify[2].ArchetypeType == 30 && verify[2].ArchetypeIndex == 3000,
                  "Updated record 2 should match");

    YULDUZ_ReleaseEntityRegistry(&registry);

    TEST_END("Entity Registry Get/Set Records");
}

void test_entity_registry_batch_operations(void) {
    TEST_START("Entity Registry Batch Operations");

    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeEntityRegistry(&registry, 16),
                  "Failed to initialize");

    const uint32_t count = 10;
    YULDUZ_Entity entities[count];

    // Batch create
    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, i, i * 100, &entities[i]);
    }

    // Batch get
    YULDUZ_EntityRecord records[count];
    YULDUZ_ASSERT(YULDUZ_GetEntityRecordsInEntityRegistry(&registry, entities, records, count),
                  "Batch get should succeed");

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_ASSERT(records[i].ArchetypeType == i, "Type for entity %u should match", i);
        YULDUZ_ASSERT(records[i].ArchetypeIndex == i * 100, "Index for entity %u should match", i);
    }

    // Batch update
    YULDUZ_EntityRecord new_records[count];
    for (uint32_t i = 0; i < count; i++) {
        new_records[i].ArchetypeType = 50 + i;
        new_records[i].ArchetypeIndex = 5000 + i;
    }

    YULDUZ_ASSERT(YULDUZ_SetEntityRecordsInEntityRegistry(&registry, entities, new_records, count),
                  "Batch set should succeed");

    // Verify
    YULDUZ_EntityRecord verify[count];
    YULDUZ_GetEntityRecordsInEntityRegistry(&registry, entities, verify, count);

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_ASSERT(verify[i].ArchetypeType == 50 + i, "Updated type should match");
        YULDUZ_ASSERT(verify[i].ArchetypeIndex == 5000 + i, "Updated index should match");
    }

    YULDUZ_ReleaseEntityRegistry(&registry);

    TEST_END("Entity Registry Batch Operations");
}

void test_entity_registry_free_list(void) {
    TEST_START("Entity Registry Free List Management");

    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeEntityRegistry(&registry, 8),
                  "Failed to initialize");

    // Create 5 entities
    YULDUZ_Entity entities[5];
    for (uint32_t i = 0; i < 5; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, i, i, &entities[i]);
    }

    YULDUZ_ASSERT(registry.FreeListCount == 0, "Free list should be empty");

    // Destroy 3 entities
    for (uint32_t i = 0; i < 3; i++) {
        YULDUZ_DestroyEntityInEntityRegistry(&registry, entities[i]);
    }

    YULDUZ_ASSERT(registry.FreeListCount == 3, "Free list should have 3 entries");

    // Create 3 new entities - should all come from free list
    YULDUZ_Entity new_entities[3];
    for (uint32_t i = 0; i < 3; i++) {
        YULDUZ_CreateEntityInEntityRegistry(&registry, 10 + i, 100 + i, &new_entities[i]);
    }

    YULDUZ_ASSERT(registry.FreeListCount == 0, "Free list should be empty again");

    // New entities should have reused the destroyed IDs
    bool all_reused = true;
    for (uint32_t i = 0; i < 3; i++) {
        bool found = false;
        for (uint32_t j = 0; j < 3; j++) {
            if (new_entities[i] == entities[j]) {
                found = true;
                break;
            }
        }
        if (!found) {
            all_reused = false;
            break;
        }
    }

    YULDUZ_ASSERT(all_reused, "All new entities should have reused destroyed IDs");

    YULDUZ_ReleaseEntityRegistry(&registry);

    TEST_END("Entity Registry Free List Management");
}

void test_entity_registry_capacity_growth(void) {
    TEST_START("Entity Registry Capacity Growth");

    YULDUZ_EntityRegistry registry = {0};

    // Start with small capacity
    YULDUZ_ASSERT(YULDUZ_InitializeEntityRegistry(&registry, 2),
                  "Failed to initialize with small capacity");

    YULDUZ_ASSERT(registry.SparseCapacity == 2, "Initial capacity should be 2");

    // Create entities beyond initial capacity
    YULDUZ_Entity entities[20];
    for (uint32_t i = 0; i < 20; i++) {
        YULDUZ_ASSERT(YULDUZ_CreateEntityInEntityRegistry(&registry, i, i * 10, &entities[i]),
                      "Failed to create entity %u", i);
    }

    YULDUZ_ASSERT(registry.SparseCapacity >= 20, "Capacity should have grown to at least 20");

    // Verify all entities are still valid and accessible
    for (uint32_t i = 0; i < 20; i++) {
        YULDUZ_EntityRecord record;
        YULDUZ_ASSERT(YULDUZ_GetEntityRecordsInEntityRegistry(&registry, &entities[i], &record, 1),
                      "Should be able to get record for entity %u", i);
        YULDUZ_ASSERT(record.ArchetypeType == i, "Archetype type should match for entity %u", i);
    }

    YULDUZ_ReleaseEntityRegistry(&registry);

    TEST_END("Entity Registry Capacity Growth");
}

void test_entity_registry_invalid_operations(void) {
    TEST_START("Entity Registry Invalid Operations");

    YULDUZ_EntityRegistry registry = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeEntityRegistry(&registry, 8),
                  "Failed to initialize");

    // Try to destroy non-existent entity
    bool result = YULDUZ_DestroyEntityInEntityRegistry(&registry, 999);
    YULDUZ_ASSERT(!result, "Destroying non-existent entity should fail");

    // Try to get record for invalid entity
    YULDUZ_Entity invalid = YULDUZ_INVALID_ENTITY;
    YULDUZ_EntityRecord record;
    result = YULDUZ_GetEntityRecordsInEntityRegistry(&registry, &invalid, &record, 1);
    YULDUZ_ASSERT(!result, "Getting record for INVALID_ENTITY should fail");

    // Try to set record for invalid entity
    YULDUZ_EntityRecord new_record = {.ArchetypeType = 0, .ArchetypeIndex = 0};
    result = YULDUZ_SetEntityRecordsInEntityRegistry(&registry, &invalid, &new_record, 1);
    YULDUZ_ASSERT(!result, "Setting record for INVALID_ENTITY should fail");

    // Create entity then destroy it, then try operations on it
    YULDUZ_Entity entity;
    YULDUZ_CreateEntityInEntityRegistry(&registry, 0, 0, &entity);
    YULDUZ_DestroyEntityInEntityRegistry(&registry, entity);

    result = YULDUZ_GetEntityRecordsInEntityRegistry(&registry, &entity, &record, 1);
    YULDUZ_ASSERT(!result, "Getting record for destroyed entity should fail");

    result = YULDUZ_SetEntityRecordsInEntityRegistry(&registry, &entity, &new_record, 1);
    YULDUZ_ASSERT(!result, "Setting record for destroyed entity should fail");

    YULDUZ_ReleaseEntityRegistry(&registry);

    TEST_END("Entity Registry Invalid Operations");
}