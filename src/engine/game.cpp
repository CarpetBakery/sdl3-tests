#include "game.h"
#include "time.h"

#include <atomic>
#include <iostream>

namespace
{
    constexpr float draw_fps = 60.0f;
    constexpr float game_fps = 60.0f;

    Uint64 game_time_last = 0;
    Uint64 game_time_accumulator = 0;
    int max_updates = 5; // Maximum updates to run before "giving up" and reducing frame rate
}

bool Game::init(void *appstate, const GameConfig &info)
{
    config = info;

    backbuffer_width = config.window_width;
    backbuffer_height = config.window_height;

    if (!init_sdl())
    {
        return false;
    }

    if (config.sdl_gpu && !init_gpu())
    {
        return false;
    }

    // Init frame limiter
    game_time_last = Time::getTicks();
    game_time_accumulator = 0;

    input.init();

    ready(appstate);
    return true;
}

void Game::quit(void *appstate, SDL_AppResult result)
{
    if (scene)
    {
        scene->destroy();
    }
}

bool Game::init_sdl()
{
    SDL_SetAppMetadata("First SDL3 Test", "1.0", "com.example.sdl3test");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(config.window_title, config.window_width, config.window_height, SDL_WINDOW_RESIZABLE);
    if (window == NULL)
    {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return false;
    }

    if (config.sdl_renderer)
    {
        renderer = SDL_CreateRenderer(window, NULL);
        if (renderer == NULL)
        {
            SDL_Log("Couldn't create renderer: %s", SDL_GetError());
            return false;
        }
        SDL_SetRenderLogicalPresentation(renderer, config.window_width, config.window_height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    }
    
    return true;
}

bool Game::init_gpu()
{
    SDL_GPUShaderFormat shader_formats = SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL;
    device = SDL_CreateGPUDevice(shader_formats, false, NULL);
    if (device == NULL)
    {
        SDL_Log("Couldn't create GPU device: %s", SDL_GetError());
        return false;
    }

    // Print info
    SDL_Log("Using %s GPU implementation.", SDL_GetGPUDeviceDriver(device));

    // Bind to window
    if (!SDL_ClaimWindowForGPUDevice(device, window))
    {
        SDL_Log("Failed to bind GPU device to window: %s", SDL_GetError());
        return false;
    }

    return true;
}

void Game::tick(void *appstate)
{
    // -- Limit framerate --
    Uint64 timeTarget = (Uint64)((1.0 / draw_fps) * Time::ticksPerSecond);
    Uint64 timeCurr = Time::getTicks();
    Uint64 timeDiff = timeCurr - game_time_last;
    game_time_last = timeCurr;
    game_time_accumulator += timeDiff;

    // Don't let us run too fast
    while (game_time_accumulator < timeTarget)
    {
        int milliseconds = (int)(timeTarget - game_time_accumulator) / (Time::ticksPerSecond / 1000);
        if (milliseconds >= 0)
        {
            SDL_Delay((Uint32)milliseconds);
        }

        Uint64 timeCurr = Time::getTicks();
        Uint64 timeDiff = timeCurr - game_time_last;
        game_time_last = timeCurr;
        game_time_accumulator += timeDiff;
    }

    // Don't let us fall behind on too many updates
    Uint64 timeMax = max_updates * timeTarget;
    if (game_time_accumulator > timeMax)
    {
        game_time_accumulator = timeMax;
    }

    while (game_time_accumulator >= timeTarget)
    {
        game_time_accumulator -= timeTarget;

        Time::delta = (1.0f / draw_fps);
        Time::deltaTime = ((Time::delta * Time::ticksPerSecond) / (float)timeTarget / (draw_fps / game_fps));

        if (Time::pauseTimer > 0)
        {
            Time::pauseTimer -= Time::delta;
            if (Time::pauseTimer <= -0.0001)
            {
                Time::delta = -Time::pauseTimer;
            }
            else
            {
                continue;
            }
        }

        Time::previousTicks = Time::ticks;
        Time::ticks += timeTarget;
        Time::previousSeconds = Time::seconds;
        Time::seconds += Time::delta;

        input.update();
        update(appstate);
        if (scene)
        {
            scene->update();
        }
    }

    draw(appstate);
    if (scene)
    {
        scene->draw();
    }

    SDL_RenderPresent(renderer);
}

void Game::ready(void *appstate)
{
}

void Game::update(void *appstate)
{
}

void Game::draw(void *appstate)
{
}

void Game::event(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_KEY_DOWN)
    {
        input.keyboard_state[event->key.scancode] = true;
    }
    else if (event->type == SDL_EVENT_KEY_UP)
    {
        input.keyboard_state[event->key.scancode] = false;
    }
}

void Game::destroy(void *appstate, SDL_AppResult result)
{
}