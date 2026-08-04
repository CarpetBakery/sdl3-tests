#pragma once
#include "world.h"
#include "input.h"
#include "debug.h"

#include <SDL3/SDL.h>
#include <memory>


struct GameConfig
{
    int window_width = 640;
    int window_height = 480;
    char *window_title = nullptr;

    RendererType renderer_type = RendererType::Sdl;
};

class Game
{
private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_GPUDevice *device = nullptr;

    // The current scene
    std::unique_ptr<Scene> scene = nullptr;

    int backbuffer_width = 0;
    int backbuffer_height = 0;

    bool renderer_initialize_first_time = true;

    // Engine init
    bool init_sdl();

    // Renderer setup/free
    bool init_renderer_sdl();
    void free_renderer_sdl();

    bool init_renderer_gpu();
    void free_renderer_gpu();

    bool init_renderer(RendererType type);
    void free_renderer();

protected:
    virtual void ready(void *appstate);
    virtual void update(void *appstate);
    virtual void draw(void *appstate);
    virtual void quit(void *appstate, SDL_AppResult result);
    
public:
    Input input;
    GameConfig config;

    Game() = default;
    virtual ~Game() = default;

    Game(const Game&) = delete;
    Game(Game&&) = delete;
    Game &operator=(const Game&) = delete;
    Game &operator=(Game&&) = delete;
    
    bool init(void *appstate, const GameConfig &info);
    void tick(void *appstate);
    void event(void *appstate, SDL_Event *event);
    void destroy(void *appstate, SDL_AppResult result);

    // Change to new scene
    template <class T>
    T *change_scene()
    {
        if (scene != nullptr)
        {
            scene->destroy();
        }

        scene = std::make_unique<T>();

        if (scene->renderer_type != config.renderer_type)
        {
            free_renderer();
            init_renderer(scene->renderer_type);
        }
        // LB_ASSERT(scene->renderer_type == config.renderer_type, "Scene requires different renderer type than initialized.");

        scene->game = this; 
        scene->ready();

        return (T*)scene.get();
    }

    inline SDL_Window *get_window() { return window; }
    inline const SDL_Window *get_window() const { return window; }
    
    inline SDL_Renderer *get_renderer() { return renderer; }
    inline const SDL_Renderer *get_renderer() const { return renderer; }

    inline SDL_GPUDevice *get_device() { return device; }
    inline const SDL_GPUDevice *get_device() const { return device; }

    inline const int window_width() const
    {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        return w;
    }
    
    inline const int window_height() const
    {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        return h;
    }

    inline Scene *get_scene() { return scene.get(); }
    inline const Scene *get_scene() const { return scene.get(); }

    // TEMP solution
    inline const int get_backbuffer_width() const { return backbuffer_width; }
    inline const int get_backbuffer_height() const { return backbuffer_height; }
};