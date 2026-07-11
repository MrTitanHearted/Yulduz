#pragma once

#include <Yulduz/PCH.h>

typedef struct YULDUZ_EngineInitInfo YULDUZ_EngineInitInfo;

typedef struct YULDUZ_Engine YULDUZ_Engine;

typedef bool (*YULDUZ_EngineOnInitPFN)(YULDUZ_Engine *engine);
typedef void (*YULDUZ_EngineOnQuitPFN)(YULDUZ_Engine *engine);
typedef bool (*YULDUZ_EngineOnStartPFN)(YULDUZ_Engine *engine);
typedef void (*YULDUZ_EngineOnStopPFN)(YULDUZ_Engine *engine);
typedef void (*YULDUZ_EngineOnEventPFN)(YULDUZ_Engine *engine, const SDL_Event *event);
typedef void (*YULDUZ_EngineOnUpdatePFN)(YULDUZ_Engine *engine, double dt);
typedef void (*YULDUZ_EngineOnDrawUiPFN)(YULDUZ_Engine *engine, double dt);
typedef void (*YULDUZ_EngineOnRenderPFN)(YULDUZ_Engine *engine, double it);

struct YULDUZ_EngineInitInfo {
    void *UserData;

    Uint64 MaxTickCount;

    double FixedDeltaTime;
    double MaxFrameDeltaTime;

    YULDUZ_EngineOnInitPFN OnInitPFN;
    YULDUZ_EngineOnQuitPFN OnQuitPFN;

    YULDUZ_EngineOnStartPFN OnStartPFN;
    YULDUZ_EngineOnStopPFN  OnStopPFN;

    YULDUZ_EngineOnEventPFN  OnEventPFN;
    YULDUZ_EngineOnUpdatePFN OnFixedUpdatePFN;
    YULDUZ_EngineOnUpdatePFN OnFrameUpdatePFN;
    YULDUZ_EngineOnDrawUiPFN OnDrawUiPFN;
    YULDUZ_EngineOnRenderPFN OnRenderPFN;
};

struct YULDUZ_Engine {
    void *UserData;

    YULDUZ_EngineOnInitPFN OnInitPFN;
    YULDUZ_EngineOnQuitPFN OnQuitPFN;

    YULDUZ_EngineOnStartPFN OnStartPFN;
    YULDUZ_EngineOnStopPFN  OnStopPFN;

    YULDUZ_EngineOnEventPFN  OnEventPFN;
    YULDUZ_EngineOnUpdatePFN OnFixedUpdatePFN;
    YULDUZ_EngineOnUpdatePFN OnFrameUpdatePFN;
    YULDUZ_EngineOnDrawUiPFN OnDrawUiPFN;
    YULDUZ_EngineOnRenderPFN OnRenderPFN;

    Uint64 LastCounter;  // Timestamp
    Uint64 TickCounter;
    Uint64 MaxTickCount;  // Max allowed Fixed Counter while for Consume
    Uint64 FixedCounter;
    Uint64 FrameCounter;

    double AccumulatedTime;
    double FixedDeltaTime;
    double FrameDeltaTime;
    double MaxFrameDeltaTime;

    bool IsRunning;

    Uint8 _yulduz_padding[7];
};

YULDUZ_API bool YULDUZ_EngineInit(YULDUZ_Engine *engine, const YULDUZ_EngineInitInfo *init_info);
YULDUZ_API void YULDUZ_EngineQuit(YULDUZ_Engine *engine);

YULDUZ_API bool YULDUZ_EngineStart(YULDUZ_Engine *engine);
YULDUZ_API void YULDUZ_EngineStop(YULDUZ_Engine *engine);

YULDUZ_API void YULDUZ_EngineTick(YULDUZ_Engine *engine);
YULDUZ_API void YULDUZ_EngineHandleEvent(YULDUZ_Engine *engine, const SDL_Event *event);
YULDUZ_API void YULDUZ_EngineFixedUpdate(YULDUZ_Engine *engine);
YULDUZ_API void YULDUZ_EngineFrameUpdate(YULDUZ_Engine *engine);
YULDUZ_API void YULDUZ_EngineDrawUI(YULDUZ_Engine *engine);
YULDUZ_API void YULDUZ_EngineRender(YULDUZ_Engine *engine);
YULDUZ_API void YULDUZ_EngineIterate(YULDUZ_Engine *engine);

YULDUZ_API bool YULDUZ_EngineShouldFixedUpdate(const YULDUZ_Engine *engine);
YULDUZ_API bool YULDUZ_EngineIsRunning(const YULDUZ_Engine *engine);

YULDUZ_API void *YULDUZ_EngineGetUserData(const YULDUZ_Engine *engine);

YULDUZ_API Uint64 YULDUZ_EngineGetLastCounter(const YULDUZ_Engine *engine);
YULDUZ_API Uint64 YULDUZ_EngineGetTickCounter(const YULDUZ_Engine *engine);
YULDUZ_API Uint64 YULDUZ_EngineGetFixedCounter(const YULDUZ_Engine *engine);
YULDUZ_API Uint64 YULDUZ_EngineGetFrameCounter(const YULDUZ_Engine *engine);

YULDUZ_API double YULDUZ_EngineGetFixedDeltaTime(const YULDUZ_Engine *engine);
YULDUZ_API double YULDUZ_EngineGetFrameDeltaTime(const YULDUZ_Engine *engine);
YULDUZ_API double YULDUZ_EngineGetAccumulatedTime(const YULDUZ_Engine *engine);
YULDUZ_API double YULDUZ_EngineGetInterpolationTime(const YULDUZ_Engine *engine);