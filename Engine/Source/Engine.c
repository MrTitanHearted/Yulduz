#include <Yulduz/Engine.h>

YULDUZ_Engine *g_pEngine = nullptr;

bool YULDUZ_InitializeEngine(YULDUZ_Engine *engine, YULDUZ_NULLABLE const YULDUZ_EngineInitializeInfo *initialize_info) {
    SDL_zerop(engine);

    SDL_SetAtomicU32(&engine->IsRunning, false);

    g_pEngine = engine;

    return true;
}

void YULDUZ_ReleaseEngine(YULDUZ_Engine *engine) {

    SDL_zerop(engine);
}

bool YULDUZ_RunEngine(YULDUZ_Engine *engine) {
    SDL_SetAtomicU32(&engine->IsRunning, true);

    return true;
}

YULDUZ_Engine *YULDUZ_GetEngine() {
    return g_pEngine;
}
