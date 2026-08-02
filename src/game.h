#pragma once
#include <SDL3/SDL.h>

class Game
{
private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    virtual void ready(void *appstate);
    virtual void update(void *appstate);
    virtual void draw(void *appstate);
    virtual void quit(void *appstate, SDL_AppResult result);
    
public:
    virtual ~Game() = default;

    bool init(void *appstate, int width, int height);
    void tick(void *appstate);
    void event(void *appstate, SDL_Event *event);
    void destroy(void *appstate, SDL_AppResult result);

    inline const SDL_Window *get_window() const { return window; }
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
};