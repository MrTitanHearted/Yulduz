#include <Yulduz/Yulduz.h>

Sint32 main() {
    SDL_Log("Hello, World!");

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window   *window   = nullptr;
    SDL_Renderer *renderer = nullptr;

    SDL_CreateWindowAndRenderer(
        "Yulduz - Hello World", 800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer);

    SDL_DisplayID display_id = SDL_GetDisplayForWindow(window);

    const SDL_DisplayMode *desktop_display_mode = SDL_GetDesktopDisplayMode(display_id);
    SDL_SetWindowFullscreenMode(window, desktop_display_mode);

    bool running = true;

    while (running) {
        SDL_Event event = {0};
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_KEY_DOWN:
                    if (!event.key.repeat) {
                        if (event.key.key == SDLK_F11) {
                            SDL_SetWindowFullscreen(window, !(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN));
                        }
                    }
                    break;

                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                case SDL_EVENT_QUIT: {
                    running = false;
                } break;
                default:
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return EXIT_SUCCESS;
}