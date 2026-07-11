#pragma once

#include <Yulduz/Yulduz.h>

typedef struct App App;

struct App {
    YULDUZ_Engine *Engine;

    SDL_Window   *Window;
    SDL_Renderer *Renderer;
};

bool AppOnInit(YULDUZ_Engine *engine);
void AppOnQuit(YULDUZ_Engine *engine);
void AppOnEvent(YULDUZ_Engine *engine, const SDL_Event *event);
void AppOnRender(YULDUZ_Engine *engine, double it);