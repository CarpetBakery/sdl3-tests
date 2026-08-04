#define SDL_MAIN_USE_CALLBACKS 1 // Use callbacks instead of the old overriding main method

#include "test_game.h"
#include "scenes.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>

namespace
{
    constexpr int WINDOW_WIDTH = 640;
    constexpr int WINDOW_HEIGHT = 480;
    
    TestGame game;
}


// Startup function
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    GameConfig config;
    config.window_title = "super mario 5";
    config.window_width = WINDOW_WIDTH;
    config.window_height = WINDOW_HEIGHT;
    config.sdl_gpu = true;
    config.sdl_renderer = false;
    
    if (!game.init(*appstate, config))
    {
        return SDL_APP_FAILURE;
    }
    game.change_scene<SceneGpu>();

    return SDL_APP_CONTINUE;
}

// Runs when a new event occurs
// Replaces iterating over events every frame(?)
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS;
    }

    game.event(appstate, event);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    game.tick(appstate);
    return SDL_APP_CONTINUE;
}

// Function runs at shutdown
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // SDL3 will clean up window/renderer for us???
    game.destroy(appstate, result);
}
