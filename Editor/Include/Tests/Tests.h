#pragma once

#include <Yulduz/Engine.h>

// ============================================================
// TEST STATE (file-local, one translation unit per .c file)
// ============================================================

static uint32_t    _test_total          = 0;
static uint32_t    _test_passed         = 0;
static uint32_t    _test_failed         = 0;
static bool        _test_current_failed = false;
static const char *_test_current_name   = NULL;

// ============================================================
// TEST MACROS
// ============================================================

/*
 * TEST_START("My Test Name")
 *   Opens a named test case, increments the test counter, and logs "[TEST N] name - STARTED".
 */
#define TEST_START(name)                                                \
    do {                                                                \
        _test_total++;                                                  \
        _test_current_name   = (name);                                  \
        _test_current_failed = false;                                   \
        YULDUZ_LOG_INFO("[TEST %u] %s - STARTED", _test_total, (name)); \
    } while (0)

/*
 * TEST_END()
 *   Closes the current test case.  If no assertion failed since TEST_START,
 *   the test is counted as PASSED; otherwise it was already counted as FAILED
 *   by the failing assertion macro.
 */
#define TEST_END()                                            \
    do {                                                      \
        if (!_test_current_failed) {                          \
            _test_passed++;                                   \
            YULDUZ_LOG_INFO("[TEST %u] %s - PASSED",          \
                            _test_total, _test_current_name); \
        }                                                     \
    } while (0)

/* Internal helper – records a failure and returns from the current function. */
#define _TEST_FAIL(fmt, ...)                                              \
    do {                                                                  \
        if (!_test_current_failed) {                                      \
            _test_current_failed = true;                                  \
            _test_failed++;                                               \
        }                                                                 \
        YULDUZ_LOG_ERROR("[TEST %u] %s - FAILED: " fmt,                   \
                         _test_total, _test_current_name, ##__VA_ARGS__); \
        return;                                                           \
    } while (0)

/*
 * TEST_ASSERT_TRUE(expr)
 *   Fails the test if expr evaluates to false / zero.
 */
#define TEST_ASSERT_TRUE(expr)                                      \
    do {                                                            \
        if (!(expr)) {                                              \
            _TEST_FAIL("Expected TRUE but got FALSE  (%s)", #expr); \
        }                                                           \
    } while (0)

/*
 * TEST_ASSERT_FALSE(expr)
 *   Fails the test if expr evaluates to true / non-zero.
 */
#define TEST_ASSERT_FALSE(expr)                                     \
    do {                                                            \
        if ((expr)) {                                               \
            _TEST_FAIL("Expected FALSE but got TRUE  (%s)", #expr); \
        }                                                           \
    } while (0)

/*
 * TEST_ASSERT_EQUAL(a, b)
 *   Fails the test if a != b.  Uses generic equality (==).
 */
#define TEST_ASSERT_EQUAL(a, b)                               \
    do {                                                      \
        if (!((a) == (b))) {                                  \
            _TEST_FAIL("Expected EQUAL  (%s == %s)", #a, #b); \
        }                                                     \
    } while (0)

/*
 * TEST_ASSERT_NOT_EQUAL(a, b)
 *   Fails the test if a == b.
 */
#define TEST_ASSERT_NOT_EQUAL(a, b)                               \
    do {                                                          \
        if ((a) == (b)) {                                         \
            _TEST_FAIL("Expected NOT EQUAL  (%s != %s)", #a, #b); \
        }                                                         \
    } while (0)

/*
 * TEST_ASSERT_NOT_NULL(a)
 *   Fails the test if a == NULL.
 */
#define TEST_ASSERT_NOT_NULL(a)                                 \
    do {                                                        \
        if ((a) == (NULL)) {                                    \
            _TEST_FAIL("Expected NOT EQUAL  (%s != NULL)", #a); \
        }                                                       \
    } while (0)

/*
 * TEST_ASSERT_NULL(a)
 *   Fails the test if a != NULL.
 */
#define TEST_ASSERT_NULL(a)                                 \
    do {                                                        \
        if ((a) != (NULL)) {                                    \
            _TEST_FAIL("Expected NOT EQUAL  (%s == NULL)", #a); \
        }                                                       \
    } while (0)

/*
 * PRINT_TEST_RESULTS()
 *   Prints a formatted summary box showing total / passed / failed counts.
 *   Call at the end of a test runner function (e.g. run_all_ecs_registry_tests).
 */
#define PRINT_TEST_RESULTS()                                                                                \
    do {                                                                                                    \
        YULDUZ_LOG_INFO("");                                                                                \
        YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");              \
        YULDUZ_LOG_INFO("║                       TEST SUMMARY                             ║");              \
        YULDUZ_LOG_INFO("╠════════════════════════════════════════════════════════════════╣");              \
        YULDUZ_LOG_INFO("║  Total:  %4u                                                  ║", _test_total);  \
        YULDUZ_LOG_INFO("║  Passed: %4u                                                  ║", _test_passed); \
        YULDUZ_LOG_INFO("║  Failed: %4u                                                  ║", _test_failed); \
        YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");              \
        if (_test_failed == 0) {                                                                            \
            YULDUZ_LOG_INFO("ALL TESTS PASSED!");                                                           \
        } else {                                                                                            \
            YULDUZ_LOG_ERROR("%u TEST(S) FAILED!", _test_failed);                                           \
        }                                                                                                   \
        YULDUZ_LOG_INFO("");                                                                                \
    } while (0)

// ============================================================
// BENCHMARK STATE
// ============================================================

static uint64_t _bench_start_ns   = 0;
static uint32_t _bench_iterations = 0;

static uint32_t _bench_total   = 0;
static uint32_t _bench_fastest = 0; /* reserved for future use */

// ============================================================
// BENCHMARK MACROS
// ============================================================

/*
 * BENCHMARK_START("Benchmark Name", iterations)
 *   Records the current high-resolution timestamp and the expected iteration
 *   count.  Logs "[BENCHMARK] name - N iterations".
 */
#define BENCHMARK_START(name, iterations)                                                  \
    do {                                                                                   \
        _bench_iterations = (iterations);                                                  \
        _bench_total++;                                                                    \
        YULDUZ_LOG_INFO("[BENCHMARK] %s - %u iterations", (name), (uint32_t)(iterations)); \
        _bench_start_ns = SDL_GetTicksNS();                                                \
    } while (0)

/*
 * BENCHMARK_END("Short Label")
 *   Calculates elapsed time since BENCHMARK_START and logs:
 *     "[BENCHMARK] label - X.XX M ops/sec (Y.YYYYYY sec total)"
 */
#define BENCHMARK_END(label)                                                                \
    do {                                                                                    \
        uint64_t _bench_end_ns  = SDL_GetTicksNS();                                         \
        double   _bench_elapsed = (double)(_bench_end_ns - _bench_start_ns) * 1e-9;         \
        double   _bench_mops    = (_bench_elapsed > 0.0)                                    \
                                      ? ((double)_bench_iterations / _bench_elapsed) * 1e-6 \
                                      : 0.0;                                                \
        YULDUZ_LOG_INFO("[BENCHMARK] %s - %.2f M ops/sec (%.6f sec total)",                 \
                        (label), _bench_mops, _bench_elapsed);                              \
    } while (0)

/*
 * PRINT_BENCHMARK_RESULTS()
 *   Prints a short summary of how many benchmarks were run.
 *   Extend with min/max/avg tracking if desired.
 */
#define PRINT_BENCHMARK_RESULTS()                                                                           \
    do {                                                                                                    \
        YULDUZ_LOG_INFO("");                                                                                \
        YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");              \
        YULDUZ_LOG_INFO("║                    BENCHMARK SUMMARY                           ║");              \
        YULDUZ_LOG_INFO("╠════════════════════════════════════════════════════════════════╣");              \
        YULDUZ_LOG_INFO("║  Benchmarks run: %4u                                          ║", _bench_total); \
        YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");              \
        YULDUZ_LOG_INFO("");                                                                                \
    } while (0)