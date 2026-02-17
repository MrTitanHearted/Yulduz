#pragma once

#include <Yulduz/Engine.h>

// Test helper macros using YULDUZ logging
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

// Benchmark helper macros using YULDUZ logging
#define BENCHMARK_START(name)                              \
    do {                                                   \
        YULDUZ_LOG_INFO("BENCHMARK: %s", name);            \
        uint64_t start_time = SDL_GetPerformanceCounter(); \
        uint64_t freq       = SDL_GetPerformanceFrequency();

#define BENCHMARK_END()                                              \
    uint64_t end_time   = SDL_GetPerformanceCounter();               \
    double   elapsed_ms = ((end_time - start_time) * 1000.0) / freq; \
    YULDUZ_LOG_INFO("  Time: %.3f ms", elapsed_ms);                  \
    }                                                                \
    while (0)

// Common component structures
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

typedef struct Transform {
    float matrix[16];
} Transform;

typedef struct Name {
    char data[64];
} Name;