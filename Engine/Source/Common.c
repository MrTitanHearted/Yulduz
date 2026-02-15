#include <Yulduz/Common.h>

bool YULDUZ_InitializeContext(YULDUZ_Context *context, YULDUZ_ContextInitializeInfo initialize_info) {
    SDL_zerop(context);

    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        return false;
    }

    context->EngineLogger      = initialize_info.EngineLogger;
    context->ApplicationLogger = initialize_info.ApplicationLogger;

    if (!YULDUZ_InitializeAsserts(initialize_info.AssertPFN, initialize_info.AssertUserData)) {
        SDL_Quit();
        return false;
    }

    return YULDUZ_InitializeLogs(&context->EngineLogger, &context->ApplicationLogger);
}

void YULDUZ_ReleaseContext(YULDUZ_Context *context) {
    YULDUZ_ReleaseLogs();
    YULDUZ_ReleaseAsserts();

    SDL_Quit();

    SDL_zerop(context);
}