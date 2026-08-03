#pragma once

#include <SDL3/SDL.h>
#include <memory>

class Game;

class Input
{
    friend Game;
    
private:
    int keyboard_state_size = 0;
    std::unique_ptr<bool[]> keyboard_state = nullptr;
    std::unique_ptr<bool[]> keyboard_state_pressed = nullptr;
    std::unique_ptr<bool[]> keyboard_state_prev = nullptr;

    void init();
    void update();
    
public:
    Input() = default;
    ~Input() = default;

    Input(const Input&) = delete;
    Input(Input&&) = delete;
    Input &operator=(const Input&) = delete;
    Input &operator=(Input&&) = delete;

    const bool key(SDL_Keycode key) const;
    const bool key_pressed(SDL_Keycode key) const;
};