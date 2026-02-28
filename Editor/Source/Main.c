#include <Yulduz/Engine.h>

void AssertFN(void *user_data, YULDUZ_AssertEntry *entry);
void LogFN(YULDUZ_Logger *logger, YULDUZ_LogEntry *entry);

int32_t main(int32_t argc, char **argv) {
    (void)argc;
    (void)argv;

    YULDUZ_ContextInitializeInfo context_initialize_info = {0};

    context_initialize_info.AssertPFN         = &AssertFN;
    context_initialize_info.AssertUserData    = nullptr;
    context_initialize_info.EngineLogger      = (YULDUZ_Logger){"ENGINE", nullptr, &LogFN, YULDUZ_LogLevel_Trace};
    context_initialize_info.ApplicationLogger = (YULDUZ_Logger){"EDITOR", nullptr, &LogFN, YULDUZ_LogLevel_Trace};

    YULDUZ_Context context = {0};
    YULDUZ_InitializeContext(&context, context_initialize_info);

    YULDUZ_Engine engine = {0};
    YULDUZ_InitializeEngine(&engine, nullptr);

    YULDUZ_StartEngine(&engine);

    SDL_Window   *window   = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_CreateWindowAndRenderer(
        "Yulduz", 1200, 1000, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE,
        &window, &renderer);

    SDL_ShowWindow(window);

    while (YULDUZ_IsRunningInEngine(&engine)) {
        SDL_Event event = {0};
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
                    YULDUZ_StopEngine(&engine);
                } break;
                default:
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    YULDUZ_ReleaseEngine(&engine);

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