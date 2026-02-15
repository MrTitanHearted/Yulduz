#include <Yulduz/Engine.h>

#include <Tests/Tests.h>

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

    // ========================================================================
    // MAIN TEST SUITE
    // ========================================================================

    YULDUZ_LOG_INFO("\n");
    YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
    YULDUZ_LOG_INFO("║                                                                ║");
    YULDUZ_LOG_INFO("║          YULDUZ ECS COMPREHENSIVE TEST SUITE                   ║");
    YULDUZ_LOG_INFO("║                                                                ║");
    YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");
    YULDUZ_LOG_INFO("\n");

    // Initialize test types
    YULDUZ_TypeRegistry test_registry = {0};
    YULDUZ_ASSERT(YULDUZ_InitializeTypeRegistry(&test_registry, 16),
                  "Failed to initialize test type registry");
    setup_test_types(&test_registry);

    YULDUZ_LOG_INFO("Test environment initialized");
    YULDUZ_LOG_INFO("Starting tests...\n");

    // Run all test suites
    run_all_type_registry_tests();
    run_all_archetype_tests();
    run_all_entity_registry_tests();
    run_all_ecs_registry_tests();
    run_all_query_tests();
    run_all_system_tests();
    // run_all_command_buffer_tests(); // Uncomment when Command Buffer is ready

    // Run benchmarks
    run_all_benchmarks();

    // Run stress tests
    run_all_stress_tests();

    // Cleanup
    YULDUZ_ReleaseTypeRegistry(&test_registry);
    cleanup_test_types();

    YULDUZ_LOG_INFO("\n");
    YULDUZ_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
    YULDUZ_LOG_INFO("║                                                                ║");
    YULDUZ_LOG_INFO("║          ALL TESTS COMPLETED SUCCESSFULLY!                     ║");
    YULDUZ_LOG_INFO("║                                                                ║");
    YULDUZ_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");
    YULDUZ_LOG_INFO("\n");

    YULDUZ_ReleaseContext(&context);

    return 0;
}

void AssertFN(void *user_data, YULDUZ_AssertEntry *entry) {
    (void)user_data;

    char message[2048];
    SDL_vsnprintf(message, sizeof(message), entry->Format, entry->FormatArguments);

    SDL_Log("[%s:%zu][ASSERT][%s]: %s -> %s\n",
            entry->FileName,
            entry->LineNumber,
            entry->FunctionName,
            entry->Condition,
            message);

    SDL_assert(false);
}

void LogFN(YULDUZ_Logger *logger, YULDUZ_LogEntry *entry) {
    char message[2048];
    SDL_vsnprintf(message, sizeof(message), entry->Format, entry->FormatArguments);

    SDL_Log("[%s:%zu][%s][%s][%s]: %s\n",
            entry->FileName,
            entry->LineNumber,
            logger->Name,
            YULDUZ_GetLogLevelName(entry->LogLevel),
            entry->FunctionName,
            message);
}