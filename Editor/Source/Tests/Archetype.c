#include <Tests/Tests.h>

// ============================================================================
// ARCHETYPE TESTS
// ============================================================================

void run_all_archetype_tests(void) {
    YULDUZ_LOG_INFO("\n");
    YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
    YULDUZ_LOG_INFO("║           ARCHETYPE TESTS                                      ║");
    YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");
    YULDUZ_LOG_INFO("\n");

    test_archetype_basic_operations();
    test_archetype_add_remove();
    test_archetype_move_entities();
    test_archetype_query_stores();
    test_archetype_capacity_growth();
    test_archetype_swap_remove();
    test_archetype_multiple_components();
    test_archetype_tags();
    test_archetype_edge_cases();
}

void test_archetype_basic_operations(void) {
    TEST_START("Archetype Basic Operations");

    YULDUZ_Archetype archetype = {0};

    YULDUZ_TypeInfo component_types[] = {
        {.Type = g_position_type, .Size = sizeof(Position), .Alignment = _Alignof(Position)}};

    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&archetype, component_types, 1, nullptr, 0, 4),
                  "Failed to initialize archetype");

    YULDUZ_ASSERT(archetype.StoreCount == 1, "Should have 1 store");
    YULDUZ_ASSERT(archetype.TagCount == 0, "Should have 0 tags");
    YULDUZ_ASSERT(archetype.DenseCount == 0, "Should start with 0 entities");
    YULDUZ_ASSERT(archetype.DenseCapacity == 4, "Capacity should be 4");

    YULDUZ_ReleaseArchetype(&archetype);

    TEST_END("Archetype Basic Operations");
}

void test_archetype_add_remove(void) {
    TEST_START("Archetype Add/Remove");

    YULDUZ_Archetype archetype = {0};

    YULDUZ_TypeInfo component_types[] = {
        {.Type = g_position_type, .Size = sizeof(Position), .Alignment = _Alignof(Position)},
        {.Type = g_velocity_type, .Size = sizeof(Velocity), .Alignment = _Alignof(Velocity)},
    };
    YULDUZ_SDL_SortTypeInfos(component_types, sizeof(component_types) / sizeof(component_types[0]));
    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&archetype, component_types, 2, nullptr, 0, 4),
                  "Failed to initialize archetype");

    // Add entity
    Position pos = {10.0f, 20.0f, 30.0f};
    Velocity vel = {1.0f, 2.0f, 3.0f};

    YULDUZ_TypeDataInfo component_data[] = {
        {.Type = g_position_type, .Data = &pos},
        {.Type = g_velocity_type, .Data = &vel}};

    YULDUZ_ArchetypeIndex index;
    YULDUZ_Entity         entity = 100;

    YULDUZ_ASSERT(YULDUZ_AddInArchetype(&archetype, entity, component_data, &index),
                  "Failed to add entity");

    YULDUZ_ASSERT(archetype.DenseCount == 1, "Should have 1 entity");
    YULDUZ_ASSERT(index == 0, "First entity should be at index 0");

    // Verify entity stored correctly
    YULDUZ_Entity stored_entity = YULDUZ_GetEntityInArchetype(&archetype, index);
    YULDUZ_ASSERT(stored_entity == entity, "Stored entity should match");

    // Verify component data
    YULDUZ_ComponentStore *pos_store = YULDUZ_QueryStoreInArchetype(&archetype, g_position_type);
    YULDUZ_ASSERT(pos_store != nullptr, "Position store should exist");

    Position *stored_pos = (Position *)YULDUZ_GetComponentInComponentStore(pos_store, index);
    YULDUZ_ASSERT(stored_pos->x == 10.0f && stored_pos->y == 20.0f && stored_pos->z == 30.0f,
                  "Position should match");

    YULDUZ_ComponentStore *vel_store  = YULDUZ_QueryStoreInArchetype(&archetype, g_velocity_type);
    Velocity              *stored_vel = (Velocity *)YULDUZ_GetComponentInComponentStore(vel_store, index);
    YULDUZ_ASSERT(stored_vel->x == 1.0f && stored_vel->y == 2.0f && stored_vel->z == 3.0f,
                  "Velocity should match");

    // Remove entity
    YULDUZ_Entity moved_entity;
    YULDUZ_ASSERT(YULDUZ_RemoveInArchetype(&archetype, index, &moved_entity),
                  "Failed to remove entity");

    YULDUZ_ASSERT(archetype.DenseCount == 0, "Should have 0 entities after removal");

    YULDUZ_ReleaseArchetype(&archetype);

    TEST_END("Archetype Add/Remove");
}

void test_archetype_move_entities(void) {
    TEST_START("Archetype Move Entities Between Archetypes");

    // Source: Position, Velocity
    YULDUZ_Archetype src         = {0};
    YULDUZ_TypeInfo  src_comps[] = {
        {.Type = g_position_type, .Size = sizeof(Position), .Alignment = _Alignof(Position)},
        {.Type = g_velocity_type, .Size = sizeof(Velocity), .Alignment = _Alignof(Velocity)},
    };
    YULDUZ_SDL_SortTypeInfos(src_comps, sizeof(src_comps) / sizeof(src_comps[0]));
    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&src, src_comps, 2, nullptr, 0, 4),
                  "Failed to init source");

    // Destination: Position, Velocity, Health
    YULDUZ_Archetype dst         = {0};
    YULDUZ_TypeInfo  dst_comps[] = {
        {.Type = g_position_type, .Size = sizeof(Position), .Alignment = _Alignof(Position)},
        {.Type = g_velocity_type, .Size = sizeof(Velocity), .Alignment = _Alignof(Velocity)},
        {.Type = g_health_type, .Size = sizeof(Health), .Alignment = _Alignof(Health)}};
    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&dst, dst_comps, 3, nullptr, 0, 4),
                  "Failed to init destination");

    // Add entity to source
    Position pos = {100.0f, 200.0f, 300.0f};
    Velocity vel = {5.0f, 10.0f, 15.0f};

    YULDUZ_TypeDataInfo src_data[] = {
        {.Type = g_position_type, .Data = &pos},
        {.Type = g_velocity_type, .Data = &vel}};

    YULDUZ_ArchetypeIndex src_idx;
    YULDUZ_Entity         entity = 500;
    YULDUZ_ASSERT(YULDUZ_AddInArchetype(&src, entity, src_data, &src_idx),
                  "Failed to add to source");

    // Move to destination with new Health component
    Health              health      = {75.0f, 100.0f};
    YULDUZ_TypeDataInfo move_data[] = {
        {.Type = g_health_type, .Data = &health}};

    YULDUZ_ArchetypeIndex dst_idx;
    YULDUZ_Entity         moved_entity;
    YULDUZ_ASSERT(YULDUZ_MoveEntityInArchetype(&src, &dst, move_data, 1,
                                               src_idx, &dst_idx, &moved_entity),
                  "Failed to move entity");

    YULDUZ_ASSERT(src.DenseCount == 0, "Source should be empty");
    YULDUZ_ASSERT(dst.DenseCount == 1, "Destination should have 1 entity");

    // Verify all components in destination
    YULDUZ_ComponentStore *dst_pos   = YULDUZ_QueryStoreInArchetype(&dst, g_position_type);
    Position              *final_pos = (Position *)YULDUZ_GetComponentInComponentStore(dst_pos, dst_idx);
    YULDUZ_ASSERT(final_pos->x == 100.0f && final_pos->y == 200.0f,
                  "Position should be preserved");

    YULDUZ_ComponentStore *dst_vel   = YULDUZ_QueryStoreInArchetype(&dst, g_velocity_type);
    Velocity              *final_vel = (Velocity *)YULDUZ_GetComponentInComponentStore(dst_vel, dst_idx);
    YULDUZ_ASSERT(final_vel->x == 5.0f && final_vel->y == 10.0f,
                  "Velocity should be preserved");

    YULDUZ_ComponentStore *dst_health   = YULDUZ_QueryStoreInArchetype(&dst, g_health_type);
    Health                *final_health = (Health *)YULDUZ_GetComponentInComponentStore(dst_health, dst_idx);
    YULDUZ_ASSERT(final_health->current == 75.0f && final_health->max == 100.0f,
                  "Health should be set");

    YULDUZ_ReleaseArchetype(&src);
    YULDUZ_ReleaseArchetype(&dst);

    TEST_END("Archetype Move Entities Between Archetypes");
}

void test_archetype_query_stores(void) {
    TEST_START("Archetype Query Stores and Tags");

    YULDUZ_Archetype archetype = {0};

    YULDUZ_TypeInfo comps[] = {
        {.Type = g_position_type, .Size = sizeof(Position), .Alignment = _Alignof(Position)},
        {.Type = g_velocity_type, .Size = sizeof(Velocity), .Alignment = _Alignof(Velocity)},
        {.Type = g_health_type, .Size = sizeof(Health), .Alignment = _Alignof(Health)},
    };
    YULDUZ_SDL_SortTypeInfos(comps, sizeof(comps) / sizeof(comps[0]));
    YULDUZ_Type tags[] = {g_tag_player, g_tag_active};
    YULDUZ_SDL_SortTypes(tags, sizeof(tags) / sizeof(tags[0]));
    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&archetype, comps, 3, tags, 2, 4),
                  "Failed to initialize archetype");

    // Query existing components
    YULDUZ_ComponentStore *pos = YULDUZ_QueryStoreInArchetype(&archetype, g_position_type);
    YULDUZ_ASSERT(pos != nullptr, "Position store should exist");
    YULDUZ_ASSERT(pos->Type == g_position_type, "Store type should match");

    YULDUZ_ComponentStore *vel = YULDUZ_QueryStoreInArchetype(&archetype, g_velocity_type);
    YULDUZ_ASSERT(vel != nullptr, "Velocity store should exist");

    YULDUZ_ComponentStore *health = YULDUZ_QueryStoreInArchetype(&archetype, g_health_type);
    YULDUZ_ASSERT(health != nullptr, "Health store should exist");

    // Query non-existing component
    YULDUZ_ComponentStore *damage = YULDUZ_QueryStoreInArchetype(&archetype, g_damage_type);
    YULDUZ_ASSERT(damage == nullptr, "Damage store should not exist");

    // Query existing tags
    YULDUZ_Type *player = YULDUZ_QueryTagInArchetype(&archetype, g_tag_player);
    YULDUZ_ASSERT(player != nullptr, "Player tag should exist");
    YULDUZ_ASSERT(*player == g_tag_player, "Player tag value should match");

    YULDUZ_Type *active = YULDUZ_QueryTagInArchetype(&archetype, g_tag_active);
    YULDUZ_ASSERT(active != nullptr, "Active tag should exist");

    // Query non-existing tag
    YULDUZ_Type *enemy = YULDUZ_QueryTagInArchetype(&archetype, g_tag_enemy);
    YULDUZ_ASSERT(enemy == nullptr, "Enemy tag should not exist");

    YULDUZ_ReleaseArchetype(&archetype);

    TEST_END("Archetype Query Stores and Tags");
}

void test_archetype_capacity_growth(void) {
    TEST_START("Archetype Capacity Growth");

    YULDUZ_Archetype archetype = {0};

    YULDUZ_TypeInfo comps[] = {
        {.Type = g_position_type, .Size = sizeof(Position), .Alignment = _Alignof(Position)}};

    // Start with capacity of 2
    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&archetype, comps, 1, nullptr, 0, 2),
                  "Failed to initialize archetype");

    YULDUZ_ASSERT(archetype.DenseCapacity == 2, "Initial capacity should be 2");

    // Add entities to trigger growth
    for (uint32_t i = 0; i < 20; i++) {
        Position            pos    = {(float)i, (float)(i * 2), (float)(i * 3)};
        YULDUZ_TypeDataInfo data[] = {{.Type = g_position_type, .Data = &pos}};

        YULDUZ_ArchetypeIndex idx;
        YULDUZ_Entity         entity = 1000 + i;

        YULDUZ_ASSERT(YULDUZ_AddInArchetype(&archetype, entity, data, &idx),
                      "Failed to add entity %u", i);
    }

    YULDUZ_ASSERT(archetype.DenseCount == 20, "Should have 20 entities");
    YULDUZ_ASSERT(archetype.DenseCapacity >= 20, "Capacity should have grown to fit");

    // Verify all data preserved after growth
    YULDUZ_ComponentStore *store = YULDUZ_QueryStoreInArchetype(&archetype, g_position_type);
    for (uint32_t i = 0; i < 20; i++) {
        Position *pos = (Position *)YULDUZ_GetComponentInComponentStore(store, i);
        YULDUZ_ASSERT(pos->x == (float)i, "Position[%u] x should be preserved", i);
        YULDUZ_ASSERT(pos->y == (float)(i * 2), "Position[%u] y should be preserved", i);
        YULDUZ_ASSERT(pos->z == (float)(i * 3), "Position[%u] z should be preserved", i);
    }

    YULDUZ_ReleaseArchetype(&archetype);

    TEST_END("Archetype Capacity Growth");
}

void test_archetype_swap_remove(void) {
    TEST_START("Archetype Swap-Remove Behavior");

    YULDUZ_Archetype archetype = {0};

    YULDUZ_TypeInfo comps[] = {
        {.Type = g_position_type, .Size = sizeof(Position), .Alignment = _Alignof(Position)}};

    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&archetype, comps, 1, nullptr, 0, 8),
                  "Failed to initialize archetype");

    // Add 5 entities with unique positions
    YULDUZ_Entity entities[5];
    for (uint32_t i = 0; i < 5; i++) {
        Position            pos    = {(float)(i * 100), 0.0f, 0.0f};
        YULDUZ_TypeDataInfo data[] = {{.Type = g_position_type, .Data = &pos}};

        YULDUZ_ArchetypeIndex idx;
        entities[i] = 2000 + i;
        YULDUZ_ASSERT(YULDUZ_AddInArchetype(&archetype, entities[i], data, &idx),
                      "Failed to add entity %u", i);
    }

    YULDUZ_ASSERT(archetype.DenseCount == 5, "Should have 5 entities");

    // Remove entity at index 2
    YULDUZ_Entity moved_entity;
    YULDUZ_ASSERT(YULDUZ_RemoveInArchetype(&archetype, 2, &moved_entity),
                  "Failed to remove entity at index 2");

    YULDUZ_ASSERT(archetype.DenseCount == 4, "Should have 4 entities after removal");
    YULDUZ_ASSERT(moved_entity == entities[4], "Last entity should be moved");

    // Verify swap: entity at index 2 should now be the last entity (entities[4])
    YULDUZ_Entity entity_at_2 = YULDUZ_GetEntityInArchetype(&archetype, 2);
    YULDUZ_ASSERT(entity_at_2 == entities[4], "Entity at index 2 should be swapped entity");

    // Verify component data swapped correctly
    YULDUZ_ComponentStore *store    = YULDUZ_QueryStoreInArchetype(&archetype, g_position_type);
    Position              *pos_at_2 = (Position *)YULDUZ_GetComponentInComponentStore(store, 2);
    YULDUZ_ASSERT(pos_at_2->x == 400.0f, "Position at index 2 should be from last entity");

    YULDUZ_ReleaseArchetype(&archetype);

    TEST_END("Archetype Swap-Remove Behavior");
}

void test_archetype_multiple_components(void) {
    TEST_START("Archetype Multiple Components");

    YULDUZ_Archetype archetype = {0};

    YULDUZ_TypeInfo comps[] = {
        {.Type = g_position_type, .Size = sizeof(Position), .Alignment = _Alignof(Position)},
        {.Type = g_velocity_type, .Size = sizeof(Velocity), .Alignment = _Alignof(Velocity)},
        {.Type = g_health_type, .Size = sizeof(Health), .Alignment = _Alignof(Health)},
        {.Type = g_damage_type, .Size = sizeof(Damage), .Alignment = _Alignof(Damage)}};
    YULDUZ_SDL_SortTypeInfos(comps, 4);
    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&archetype, comps, 4, nullptr, 0, 4),
                  "Failed to initialize archetype with 4 components");

    YULDUZ_ASSERT(archetype.StoreCount == 4, "Should have 4 component stores");

    // Add entity with all components
    Position pos    = {1.0f, 2.0f, 3.0f};
    Velocity vel    = {0.1f, 0.2f, 0.3f};
    Health   health = {80.0f, 100.0f};
    Damage   damage = {25.0f};

    YULDUZ_TypeDataInfo data[] = {
        {.Type = g_position_type, .Data = &pos},
        {.Type = g_velocity_type, .Data = &vel},
        {.Type = g_health_type, .Data = &health},
        {.Type = g_damage_type, .Data = &damage}};

    YULDUZ_ArchetypeIndex idx;
    YULDUZ_Entity         entity = 777;

    YULDUZ_ASSERT(YULDUZ_AddInArchetype(&archetype, entity, data, &idx),
                  "Failed to add entity with all components");

    // Verify all component data
    YULDUZ_ComponentStore *pos_store = YULDUZ_QueryStoreInArchetype(&archetype, g_position_type);
    Position              *s_pos     = (Position *)YULDUZ_GetComponentInComponentStore(pos_store, idx);
    YULDUZ_ASSERT(s_pos->x == 1.0f && s_pos->y == 2.0f && s_pos->z == 3.0f,
                  "Position data should match");

    YULDUZ_ComponentStore *vel_store = YULDUZ_QueryStoreInArchetype(&archetype, g_velocity_type);
    Velocity              *s_vel     = (Velocity *)YULDUZ_GetComponentInComponentStore(vel_store, idx);
    YULDUZ_ASSERT(s_vel->x == 0.1f && s_vel->y == 0.2f && s_vel->z == 0.3f,
                  "Velocity data should match");

    YULDUZ_ComponentStore *health_store = YULDUZ_QueryStoreInArchetype(&archetype, g_health_type);
    Health                *s_health     = (Health *)YULDUZ_GetComponentInComponentStore(health_store, idx);
    YULDUZ_ASSERT(s_health->current == 80.0f && s_health->max == 100.0f,
                  "Health data should match");

    YULDUZ_ComponentStore *damage_store = YULDUZ_QueryStoreInArchetype(&archetype, g_damage_type);
    Damage                *s_damage     = (Damage *)YULDUZ_GetComponentInComponentStore(damage_store, idx);
    YULDUZ_ASSERT(s_damage->value == 25.0f, "Damage data should match");

    YULDUZ_ReleaseArchetype(&archetype);

    TEST_END("Archetype Multiple Components");
}

void test_archetype_tags(void) {
    TEST_START("Archetype Tags");

    YULDUZ_Archetype archetype = {0};

    YULDUZ_TypeInfo comps[] = {
        {.Type = g_position_type, .Size = sizeof(Position), .Alignment = _Alignof(Position)}};

    YULDUZ_Type tags[] = {g_tag_player, g_tag_active, g_tag_enemy};
    YULDUZ_SDL_SortTypes(tags, 3);
    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&archetype, comps, 1, tags, 3, 4),
                  "Failed to initialize archetype with tags");

    YULDUZ_ASSERT(archetype.TagCount == 3, "Should have 3 tags");

    // Verify all tags exist
    for (uint32_t i = 0; i < 3; i++) {
        YULDUZ_Type *tag = YULDUZ_QueryTagInArchetype(&archetype, tags[i]);
        YULDUZ_ASSERT(tag != nullptr, "Tag %u should exist", i);
        YULDUZ_ASSERT(*tag == tags[i], "Tag %u value should match", i);
    }

    // Tags shouldn't affect entity storage
    Position            pos    = {5.0f, 10.0f, 15.0f};
    YULDUZ_TypeDataInfo data[] = {{.Type = g_position_type, .Data = &pos}};

    YULDUZ_ArchetypeIndex idx;
    YULDUZ_Entity         entity = 888;

    YULDUZ_ASSERT(YULDUZ_AddInArchetype(&archetype, entity, data, &idx),
                  "Adding entity with tags should work");

    YULDUZ_ASSERT(archetype.DenseCount == 1, "Should have 1 entity");

    YULDUZ_ReleaseArchetype(&archetype);

    TEST_END("Archetype Tags");
}

void test_archetype_edge_cases(void) {
    TEST_START("Archetype Edge Cases");

    // Test 1: Empty archetype (no components, no tags)
    YULDUZ_Archetype empty = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&empty, nullptr, 0, nullptr, 0, 4),
                  "Should be able to create empty archetype");

    YULDUZ_ASSERT(empty.StoreCount == 0, "Empty archetype should have 0 stores");
    YULDUZ_ASSERT(empty.TagCount == 0, "Empty archetype should have 0 tags");

    YULDUZ_ReleaseArchetype(&empty);

    // Test 2: Tags only (no components)
    YULDUZ_Archetype tags_only = {0};
    YULDUZ_Type      tags[]    = {g_tag_player};
    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&tags_only, nullptr, 0, tags, 1, 4),
                  "Should be able to create tags-only archetype");

    YULDUZ_ASSERT(tags_only.StoreCount == 0, "Tags-only archetype should have 0 stores");
    YULDUZ_ASSERT(tags_only.TagCount == 1, "Tags-only archetype should have tags");

    YULDUZ_ReleaseArchetype(&tags_only);

    // Test 3: Move with no overlapping components
    YULDUZ_Archetype src = {0};
    YULDUZ_Archetype dst = {0};

    YULDUZ_TypeInfo src_comp[] = {
        {.Type = g_position_type, .Size = sizeof(Position), .Alignment = _Alignof(Position)}};
    YULDUZ_TypeInfo dst_comp[] = {
        {.Type = g_health_type, .Size = sizeof(Health), .Alignment = _Alignof(Health)}};

    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&src, src_comp, 1, nullptr, 0, 4),
                  "Failed to init src");
    YULDUZ_ASSERT(YULDUZ_InitializeArchetype(&dst, dst_comp, 1, nullptr, 0, 4),
                  "Failed to init dst");

    Position            pos        = {1.0f, 2.0f, 3.0f};
    YULDUZ_TypeDataInfo src_data[] = {{.Type = g_position_type, .Data = &pos}};

    YULDUZ_ArchetypeIndex src_idx;
    YULDUZ_ASSERT(YULDUZ_AddInArchetype(&src, 100, src_data, &src_idx),
                  "Failed to add to src");

    Health              health      = {50.0f, 100.0f};
    YULDUZ_TypeDataInfo move_data[] = {{.Type = g_health_type, .Data = &health}};

    YULDUZ_ArchetypeIndex dst_idx;
    YULDUZ_Entity         moved;
    YULDUZ_ASSERT(YULDUZ_MoveEntityInArchetype(&src, &dst, move_data, 1,
                                               src_idx, &dst_idx, &moved),
                  "Move with no overlap should succeed");

    YULDUZ_ASSERT(src.DenseCount == 0, "Source should be empty");
    YULDUZ_ASSERT(dst.DenseCount == 1, "Destination should have entity");

    YULDUZ_ReleaseArchetype(&src);
    YULDUZ_ReleaseArchetype(&dst);

    TEST_END("Archetype Edge Cases");
}