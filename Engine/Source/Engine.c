#include <Yulduz/Engine.h>

bool YULDUZ_EngineInit(YULDUZ_Engine *engine, const YULDUZ_EngineInitInfo *init_info) {
    SDL_assert(init_info->FixedDeltaTime > 0.f && "Hehe, saved you from infinite loop!");
    SDL_assert(init_info->MaxFrameDeltaTime > 0.f && "Hehe, saved you from infinite loop!");
    SDL_zerop(engine);

    engine->UserData = init_info->UserData;

    engine->OnInitPFN = init_info->OnInitPFN;
    engine->OnQuitPFN = init_info->OnQuitPFN;

    engine->OnStartPFN = init_info->OnStartPFN;
    engine->OnStopPFN  = init_info->OnStopPFN;

    engine->OnEventPFN       = init_info->OnEventPFN;
    engine->OnFixedUpdatePFN = init_info->OnFixedUpdatePFN;
    engine->OnFrameUpdatePFN = init_info->OnFrameUpdatePFN;
    engine->OnDrawUiPFN      = init_info->OnDrawUiPFN;
    engine->OnRenderPFN      = init_info->OnRenderPFN;

    engine->LastCounter  = SDL_GetPerformanceCounter();
    engine->TickCounter  = 0;
    engine->MaxTickCount = init_info->MaxTickCount;
    engine->FixedCounter = 0;
    engine->FrameCounter = 0;

    engine->AccumulatedTime   = 0.f;
    engine->FixedDeltaTime    = init_info->FixedDeltaTime;
    engine->FrameDeltaTime    = 0.f;
    engine->MaxFrameDeltaTime = init_info->MaxFrameDeltaTime;

    engine->IsRunning = false;

    if (nullptr != engine->OnInitPFN)
        return engine->OnInitPFN(engine);
    return true;
}

void YULDUZ_EngineQuit(YULDUZ_Engine *engine) {
    if (nullptr != engine->OnQuitPFN)
        engine->OnQuitPFN(engine);
    SDL_zerop(engine);
}

bool YULDUZ_EngineStart(YULDUZ_Engine *engine) {
    engine->IsRunning = true;
    if (nullptr != engine->OnStartPFN)
        return engine->OnStartPFN(engine);
    return true;
}

void YULDUZ_EngineStop(YULDUZ_Engine *engine) {
    engine->IsRunning = false;
    if (nullptr != engine->OnStopPFN)
        engine->OnStopPFN(engine);
}

void YULDUZ_EngineTick(YULDUZ_Engine *engine) {
    const Uint64 current_counter = SDL_GetPerformanceCounter();

    engine->FrameDeltaTime =
        (double)(current_counter - engine->LastCounter) /
        (double)SDL_GetPerformanceFrequency();

    engine->LastCounter = current_counter;

    if (engine->FrameDeltaTime > engine->MaxFrameDeltaTime)
        engine->FrameDeltaTime = engine->MaxFrameDeltaTime;

    engine->AccumulatedTime += engine->FrameDeltaTime;
    engine->TickCounter = 0;
}

void YULDUZ_EngineHandleEvent(YULDUZ_Engine *engine, const SDL_Event *event) {
    if (nullptr != engine->OnEventPFN)
        engine->OnEventPFN(engine, event);
}

void YULDUZ_EngineFixedUpdate(YULDUZ_Engine *engine) {
    if (nullptr != engine->OnFixedUpdatePFN)
        engine->OnFixedUpdatePFN(engine, engine->FixedDeltaTime);
    engine->AccumulatedTime -= engine->FixedDeltaTime;
    engine->FixedCounter++;
    engine->TickCounter++;
}

void YULDUZ_EngineFrameUpdate(YULDUZ_Engine *engine) {
    if (nullptr != engine->OnFrameUpdatePFN)
        engine->OnFrameUpdatePFN(engine, engine->FrameDeltaTime);
    engine->FrameCounter++;
}

void YULDUZ_EngineDrawUI(YULDUZ_Engine *engine) {
    if (nullptr != engine->OnDrawUiPFN)
        engine->OnDrawUiPFN(engine, engine->FrameDeltaTime);
}

void YULDUZ_EngineRender(YULDUZ_Engine *engine) {
    if (nullptr != engine->OnRenderPFN)
        engine->OnRenderPFN(engine, YULDUZ_EngineGetInterpolationTime(engine));
}

void YULDUZ_EngineIterate(YULDUZ_Engine *engine) {
    YULDUZ_EngineTick(engine);

    SDL_Event event = {0};
    while (SDL_PollEvent(&event))
        YULDUZ_EngineHandleEvent(engine, &event);

    while (YULDUZ_EngineShouldFixedUpdate(engine))
        YULDUZ_EngineFixedUpdate(engine);

    YULDUZ_EngineFrameUpdate(engine);
    YULDUZ_EngineDrawUI(engine);
    YULDUZ_EngineRender(engine);
}

bool YULDUZ_EngineShouldFixedUpdate(const YULDUZ_Engine *engine) {
    return engine->AccumulatedTime >= engine->FixedDeltaTime &&
           engine->TickCounter < engine->MaxTickCount;
}

bool YULDUZ_EngineIsRunning(const YULDUZ_Engine *engine) {
    return engine->IsRunning;
}

void *YULDUZ_EngineGetUserData(const YULDUZ_Engine *engine) {
    return engine->UserData;
}

Uint64 YULDUZ_EngineGetLastCounter(const YULDUZ_Engine *engine) {
    return engine->LastCounter;
}

Uint64 YULDUZ_EngineGetTickCounter(const YULDUZ_Engine *engine) {
    return engine->TickCounter;
}

Uint64 YULDUZ_EngineGetFixedCounter(const YULDUZ_Engine *engine) {
    return engine->FixedCounter;
}

Uint64 YULDUZ_EngineGetFrameCounter(const YULDUZ_Engine *engine) {
    return engine->FrameCounter;
}

double YULDUZ_EngineGetFixedDeltaTime(const YULDUZ_Engine *engine) {
    return engine->FixedDeltaTime;
}

double YULDUZ_EngineGetFrameDeltaTime(const YULDUZ_Engine *engine) {
    return engine->FrameDeltaTime;
}

double YULDUZ_EngineGetAccumulatedTime(const YULDUZ_Engine *engine) {
    return engine->AccumulatedTime;
}

double YULDUZ_EngineGetInterpolationTime(const YULDUZ_Engine *engine) {
    return engine->AccumulatedTime / engine->FixedDeltaTime;
}