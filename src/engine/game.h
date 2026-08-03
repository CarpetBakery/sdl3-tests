#pragma once
#include "world.h"
#include "input.h"

#include <SDL3/SDL.h>
#include <memory>


class Game
{
private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    std::unique_ptr<Scene> scene = nullptr;

    int backbuffer_width = 0;
    int backbuffer_height = 0;

protected:
    virtual void ready(void *appstate);
    virtual void update(void *appstate);
    virtual void draw(void *appstate);
    virtual void quit(void *appstate, SDL_AppResult result);
    
public:
    Input input;

    Game() = default;
    virtual ~Game() = default;

    Game(const Game&) = delete;
    Game(Game&&) = delete;
    Game &operator=(const Game&) = delete;
    Game &operator=(Game&&) = delete;
    
    bool init(void *appstate, int width, int height);
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
        scene->game = this; 
        scene->ready();

        return (T*)scene.get();
    }

    inline SDL_Window *get_window() { return window; }
    inline const SDL_Window *get_window() const { return window; }
    inline SDL_Renderer *get_renderer() { return renderer; }
    inline const SDL_Renderer *get_renderer() const { return renderer; }

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