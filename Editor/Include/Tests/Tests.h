#pragma once

#include <Yulduz/Engine.h>

// ============================================================================
// TEST HELPER MACROS
// ============================================================================

#define TEST_START(name) \
    YULDUZ_LOG_INFO("========================================"); \
    YULDUZ_LOG_INFO("TEST: %s", name); \
    YULDUZ_LOG_INFO("========================================")

#define TEST_END(name) \
    YULDUZ_LOG_INFO("✓ PASSED: %s\n", name)

#define BENCHMARK_START(name, iters) \
    YULDUZ_LOG_INFO("========================================"); \
    YULDUZ_LOG_INFO("BENCHMARK: %s (%u iterations)", name, iters); \
    uint32_t _bench_iterations = iters; \
    clock_t _bench_start = clock()

#define BENCHMARK_END() \
    clock_t _bench_end = clock(); \
    double _bench_time = ((double)(_bench_end - _bench_start)) / CLOCKS_PER_SEC; \
    YULDUZ_LOG_INFO("Time: %.6f seconds (%.2f ops/sec)", \
                    _bench_time, _bench_iterations / (_bench_time + 0.000001)); \
    YULDUZ_LOG_INFO("========================================\n")

// ============================================================================
// TEST COMPONENT TYPES
// ============================================================================

typedef struct Position {
    float x, y, z;
} Position;

typedef struct Velocity {
    float x, y, z;
} Velocity;

typedef struct Health {
    float current;
    float max;
} Health;

typedef struct Damage {
    float value;
} Damage;

typedef struct Name {
    char buffer[64];
} Name;

typedef struct Sprite {
    uint32_t texture_id;
    float width, height;
} Sprite;

typedef struct Transform {
    float matrix[16];
} Transform;

// ============================================================================
// GLOBAL TYPE VARIABLES
// ============================================================================

extern YULDUZ_Type g_position_type;
extern YULDUZ_Type g_velocity_type;
extern YULDUZ_Type g_health_type;
extern YULDUZ_Type g_damage_type;
extern YULDUZ_Type g_name_type;
extern YULDUZ_Type g_sprite_type;
extern YULDUZ_Type g_transform_type;
extern YULDUZ_Type g_tag_player;
extern YULDUZ_Type g_tag_enemy;
extern YULDUZ_Type g_tag_dead;
extern YULDUZ_Type g_tag_active;
extern YULDUZ_Type g_tag_inactive;

// ============================================================================
// TEST FUNCTION DECLARATIONS
// ============================================================================

// Type Registry Tests
void run_all_type_registry_tests(void);
void test_type_registry_basic(void);
void test_type_registry_batch_registration(void);
void test_type_registry_sorting(void);

// Archetype Tests
void run_all_archetype_tests(void);
void test_archetype_basic_operations(void);
void test_archetype_add_remove(void);
void test_archetype_move_entities(void);
void test_archetype_query_stores(void);
void test_archetype_capacity_growth(void);
void test_archetype_swap_remove(void);
void test_archetype_multiple_components(void);
void test_archetype_tags(void);
void test_archetype_edge_cases(void);

// Entity Registry Tests
void run_all_entity_registry_tests(void);
void test_entity_registry_create_destroy(void);
void test_entity_registry_reuse(void);
void test_entity_registry_records(void);
void test_entity_registry_batch_operations(void);
void test_entity_registry_free_list(void);
void test_entity_registry_capacity_growth(void);
void test_entity_registry_invalid_operations(void);

// ECS Registry Tests
void run_all_ecs_registry_tests(void);
void test_ecs_null_archetype(void);
void test_ecs_entity_lifecycle(void);
void test_ecs_add_remove_components(void);
void test_ecs_add_remove_tags(void);
void test_ecs_set_get_components(void);
void test_ecs_archetype_transitions(void);
void test_ecs_archetype_creation(void);
// void test_ecs_query_archetypes(void);
void test_ecs_complex_scenarios(void);

// Query Tests
void run_all_query_tests(void);
void test_query_initialization(void);
void test_query_component_requirements(void);
void test_query_tag_requirements(void);
void test_query_mixed_requirements(void);
void test_query_access_types(void);
void test_query_deep_copy(void);
void test_query_archetype_matching(void);

// System Tests
void run_all_system_tests(void);
void test_system_initialization(void);
void test_system_execution(void);
void test_system_query_filtering(void);
void test_system_read_write_access(void);
void test_system_multiple_archetypes(void);
void test_system_with_user_data(void);

// Command Buffer Tests
void run_all_command_buffer_tests(void);
void test_command_buffer_deferred_operations(void);
void test_command_buffer_entity_creation(void);
void test_command_buffer_add_remove_components(void);
void test_command_buffer_flush(void);

// Benchmarks
void run_all_benchmarks(void);
void benchmark_archetype_add(void);
void benchmark_archetype_remove(void);
void benchmark_archetype_move(void);
void benchmark_archetype_iteration(void);
void benchmark_entity_create_destroy(void);
void benchmark_entity_reuse(void);
void benchmark_ecs_component_add(void);
void benchmark_ecs_component_remove(void);
void benchmark_ecs_component_set_get(void);
void benchmark_ecs_archetype_transition(void);
void benchmark_query_matching(void);
void benchmark_system_execution(void);

// Stress Tests
void run_all_stress_tests(void);
void stress_test_many_entities(void);
void stress_test_many_archetypes(void);
void stress_test_component_churn(void);
void stress_test_archetype_transitions(void);
void stress_test_random_operations(void);

// Utility Functions
void setup_test_types(YULDUZ_TypeRegistry *registry);
void cleanup_test_types(void);