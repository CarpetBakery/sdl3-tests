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
    if (!game.init(*appstate, WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        return SDL_APP_FAILURE;
    }
    game.change_scene<SceneAudio>();

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
    // SDL will clean up window/renderer for us???
    game.destroy(appstate, result);
}
