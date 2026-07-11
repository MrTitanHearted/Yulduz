#include <App.h>

Sint32 main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    App app = {0};

    YULDUZ_EngineInitInfo engine_init_info = {
        .UserData = &app,

        .MaxTickCount = 0,

        .FixedDeltaTime    = 1.,
        .MaxFrameDeltaTime = 1.,

        .OnInitPFN   = AppOnInit,
        .OnQuitPFN   = AppOnQuit,
        .OnEventPFN  = AppOnEvent,
        .OnRenderPFN = AppOnRender,
    };

    YULDUZ_Engine engine = {0};
    YULDUZ_EngineInit(&engine, &engine_init_info);

    YULDUZ_EngineStart(&engine);
    while (YULDUZ_EngineIsRunning(&engine))
        YULDUZ_EngineIterate(&engine);

    YULDUZ_EngineQuit(&engine);

    SDL_Quit();

    return EXIT_SUCCESS;
}