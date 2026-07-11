#include <App.h>

bool AppOnInit(YULDUZ_Engine *engine) {
    SDL_Log("Hello, World!");

    App *app = YULDUZ_EngineGetUserData(engine);

    SDL_zerop(app);

    app->Engine = engine;

    SDL_CreateWindowAndRenderer(
        "Yulduz - Hello World", 800, 600, SDL_WINDOW_RESIZABLE, &app->Window, &app->Renderer);

    SDL_DisplayID display_id = SDL_GetDisplayForWindow(app->Window);

    const SDL_DisplayMode *desktop_display_mode = SDL_GetDesktopDisplayMode(display_id);
    SDL_SetWindowFullscreenMode(app->Window, desktop_display_mode);

    return true;
}

void AppOnQuit(YULDUZ_Engine *engine) {
    App *app = YULDUZ_EngineGetUserData(engine);

    SDL_DestroyRenderer(app->Renderer);
    SDL_DestroyWindow(app->Window);

    SDL_zerop(app);
}

void AppOnEvent(YULDUZ_Engine *engine, const SDL_Event *event) {
    const App *app = YULDUZ_EngineGetUserData(engine);

    switch (event->type) {
        case SDL_EVENT_KEY_DOWN:
            if (!event->key.repeat) {
                if (event->key.key == SDLK_F11) {
                    SDL_SetWindowFullscreen(app->Window, !(SDL_GetWindowFlags(app->Window) & SDL_WINDOW_FULLSCREEN));
                }
            }
            break;

        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        case SDL_EVENT_QUIT: {
            YULDUZ_EngineStop(engine);
        } break;
        default:
    }
}

void AppOnRender(YULDUZ_Engine *engine, double it) {
    const App *app = YULDUZ_EngineGetUserData(engine);

    SDL_SetRenderDrawColorFloat(app->Renderer, 0.2f, 0.3f, 0.3f, 1.f);
    SDL_RenderClear(app->Renderer);
    SDL_RenderPresent(app->Renderer);
}