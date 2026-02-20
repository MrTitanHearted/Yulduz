#include <Yulduz/Engine.h>

// Forward declarations for test runners
void run_all_ecs_registry_tests(void);  // Comprehensive tests
void run_all_system_tests(void);

// Forward declarations for benchmark runners
void run_all_ecs_benchmarks(void);  // Comprehensive benchmarks

void AssertFN(void *user_data, YULDUZ_AssertEntry *entry);
void LogFN(YULDUZ_Logger *logger, YULDUZ_LogEntry *entry);

int32_t main(int32_t argc, char **argv) {
    (void)argc;
    (void)argv;

    YULDUZ_ContextInitializeInfo context_initialize_info = {0};

    context_initialize_info.AssertPFN         = &AssertFN;
    context_initialize_info.AssertUserData    = nullptr;
    context_initialize_info.EngineLogger      = (YULDUZ_Logger){"ENGINE", nullptr, &LogFN, YULDUZ_LogLevel_Trace};
    context_initialize_info.ApplicationLogger = (YULDUZ_Logger){"TESTS", nullptr, &LogFN, YULDUZ_LogLevel_Trace};

    YULDUZ_Context context = {0};
    YULDUZ_InitializeContext(&context, context_initialize_info);

    YULDUZ_LOG_INFO("\n");
    YULDUZ_LOG_INFO("================================================================================");
    YULDUZ_LOG_INFO("                    YULDUZ ECS FRAMEWORK TEST SUITE");
    YULDUZ_LOG_INFO("================================================================================\n");

    bool run_tests      = true;
    bool run_benchmarks = true;

    // Parse command line arguments
    for (int32_t i = 1; i < argc; i++) {
        if (SDL_strcmp(argv[i], "--tests-only") == 0) {
            run_benchmarks = false;
        } else if (SDL_strcmp(argv[i], "--benchmarks-only") == 0) {
            run_tests = false;
        } else if (SDL_strcmp(argv[i], "--help") == 0) {
            YULDUZ_LOG_INFO("Usage: %s [OPTIONS]\n", argv[0]);
            YULDUZ_LOG_INFO("Options:\n");
            YULDUZ_LOG_INFO("  --tests-only       Run only tests, skip benchmarks\n");
            YULDUZ_LOG_INFO("  --benchmarks-only  Run only benchmarks, skip tests\n");
            YULDUZ_LOG_INFO("  --help             Show this help message\n");
            YULDUZ_ReleaseContext(&context);
            return 0;
        }
    }

    // ========================================
    // Run Tests
    // ========================================
    if (run_tests) {
        YULDUZ_LOG_INFO("\n");
        YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
        YULDUZ_LOG_INFO("║                              RUNNING TESTS                     ║");
        YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");

        run_all_ecs_registry_tests();
        run_all_system_tests();

        YULDUZ_LOG_INFO("\n");
        YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
        YULDUZ_LOG_INFO("║                           TESTS COMPLETED                      ║");
        YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝\n");
    }

    // ========================================
    // Run Benchmarks
    // ========================================
    if (run_benchmarks) {
        YULDUZ_LOG_INFO("\n");
        YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
        YULDUZ_LOG_INFO("║                           RUNNING BENCHMARKS                   ║");
        YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");

        run_all_ecs_benchmarks();

        YULDUZ_LOG_INFO("\n");
        YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
        YULDUZ_LOG_INFO("║                        BENCHMARKS COMPLETED                    ║");
        YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝\n");
    }

    YULDUZ_LOG_INFO("\n");
    YULDUZ_LOG_INFO("================================================================================");
    YULDUZ_LOG_INFO("                         ALL OPERATIONS COMPLETED");
    YULDUZ_LOG_INFO("================================================================================\n");

    YULDUZ_ReleaseContext(&context);
    return 0;
}

void AssertFN(void *user_data, YULDUZ_AssertEntry *entry) {
    (void)user_data;
    char message[2048];
    SDL_vsnprintf(message, sizeof(message), entry->Format, entry->FormatArguments);
    SDL_Log("[%s:%zu][ASSERT][%s]: %s -> %s\n",
            entry->FileName, entry->LineNumber, entry->FunctionName,
            entry->Condition, message);
    SDL_assert(false);
}

void LogFN(YULDUZ_Logger *logger, YULDUZ_LogEntry *entry) {
    char message[2048];
    SDL_vsnprintf(message, sizeof(message), entry->Format, entry->FormatArguments);
    SDL_Log("[%s:%-4zu][%s][%s][%s]: %s\n",
            entry->FileName, entry->LineNumber,
            logger->Name,
            YULDUZ_GetLogLevelName(entry->LogLevel),
            entry->FunctionName,
            message);
}