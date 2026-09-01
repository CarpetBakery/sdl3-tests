#pragma once
#include "spatial.h"

#include <SDL3/SDL.h>
#include <memory>

class Game;

enum MouseButton
{
    Left = 1,
    Middle = 2,
    Right = 3
};

class Input
{
    friend Game;

private:
    int keyboard_state_size = 0;
    std::unique_ptr<bool[]> keyboard_state = nullptr;
    std::unique_ptr<bool[]> keyboard_state_pressed = nullptr;
    std::unique_ptr<bool[]> keyboard_state_prev = nullptr;

    static constexpr int MOUSE_STATE_SIZE = 3;
    bool mouse_state[MOUSE_STATE_SIZE];
    bool mouse_state_pressed[MOUSE_STATE_SIZE];
    bool mouse_state_prev[MOUSE_STATE_SIZE];
    int mouse_wheel_state = 0;
    Vec2i mouse_pos;
    Vec2i mouse_pos_relative;

    void init();
    void update();

    void event_mouse_down(SDL_Event *event);
    void event_mouse_up(SDL_Event *event);
    void event_mouse_wheel(SDL_Event *event);
    void event_mouse_motion(SDL_Event *event, Game *game);
public:
    Input() = default;
    ~Input() = default;

    Input(const Input&) = delete;
    Input(Input&&) = delete;
    Input &operator=(const Input&) = delete;
    Input &operator=(Input&&) = delete;

    const bool key(SDL_Keycode key) const;
    const bool key_pressed(SDL_Keycode key) const;

    const bool mouse(MouseButton btn) const;
    const bool mouse_pressed(MouseButton btn) const;
    inline const int mouse_wheel() const { return mouse_wheel_state; }
    inline const Vec2i get_mouse_pos() const { return mouse_pos; }
};
