#include "input.h"
#include "debug.h"

void Input::init()
{
    const bool *key_state = SDL_GetKeyboardState(&keyboard_state_size);

    // Allocate keyboard states
    keyboard_state = std::make_unique<bool[]>(keyboard_state_size);
    keyboard_state_pressed = std::make_unique<bool[]>(keyboard_state_size);
    keyboard_state_prev = std::make_unique<bool[]>(keyboard_state_size);

    memset(keyboard_state.get(), 0, keyboard_state_size);
    memset(keyboard_state_pressed.get(), 0, keyboard_state_size);
    memset(keyboard_state_prev.get(), 0, keyboard_state_size);
}

void Input::update()
{
    for (int i = 0; i < keyboard_state_size; i++)
    {
        keyboard_state_pressed[i] = keyboard_state[i] && !keyboard_state_prev[i];
        keyboard_state_prev[i] = keyboard_state[i];
    }
}

const bool Input::key(SDL_Keycode key) const
{
    SDL_Scancode scancode = SDL_GetScancodeFromKey(key, NULL);
    LB_ASSERT(scancode >= 0 && scancode < keyboard_state_size, "Keycode out of range.");
    return keyboard_state[scancode];
}

const bool Input::key_pressed(SDL_Keycode key) const
{
    SDL_Scancode scancode = SDL_GetScancodeFromKey(key, NULL);
    LB_ASSERT(scancode >= 0 && scancode < keyboard_state_size, "Keycode out of range.");
    return keyboard_state_pressed[scancode];
}

    