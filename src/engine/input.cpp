#include "input.h"
#include "game.h"
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

    memset(mouse_state, 0, MOUSE_STATE_SIZE);
    memset(mouse_state_pressed, 0, MOUSE_STATE_SIZE);
    memset(mouse_state_prev, 0, MOUSE_STATE_SIZE);
}

void Input::update()
{
    for (int i = 0; i < keyboard_state_size; i++)
    {
        keyboard_state_pressed[i] = keyboard_state[i] && !keyboard_state_prev[i];
        keyboard_state_prev[i] = keyboard_state[i];
    }

    for (int i = 0; i < 3; i++)
    {
        mouse_state_pressed[i] = mouse_state[i] && !mouse_state_prev[i];
        mouse_state_prev[i] = mouse_state[i];
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

const bool Input::mouse(MouseButton btn) const
{
    return mouse_state[static_cast<int>(btn)];
}

const bool Input::mouse_pressed(MouseButton btn) const
{
    return mouse_state_pressed[static_cast<int>(btn)];
}

void Input::event_mouse_down(SDL_Event *event)
{
    int btn = event->button.button;
    if (btn >= 0 && btn < MOUSE_STATE_SIZE)
    {
        mouse_state[btn] = true;
    }
}

void Input::event_mouse_up(SDL_Event *event)
{
    // TODO: Maybe don't run this on buttons that were pressed on this frame,
    // such that, if someone presses and releases the mouse in a single frame their input
    // doesn't get dropped.
    int btn = event->button.button;
    if (btn >= 0 && btn < MOUSE_STATE_SIZE)
    {
        mouse_state[btn] = false;
    }
}

void Input::event_mouse_wheel(SDL_Event *event)
{
    mouse_wheel_state += event->wheel.y;
}

void Input::event_mouse_motion(SDL_Event *event, Game *game)
{
    int x = event->motion.x;
    int y = event->motion.y;
    int xRel = event->motion.xrel;
    int yRel = event->motion.yrel;

    float m_scale_x, m_scale_y;
	Vec2f screen_size = game->get_backbuffer_size();
	Vec2f window_size = game->window_size();
	Rect screen_surf_size = game->get_screen_surface_size();

	// Convert mouse position to be scale (0-1) where (0, 0) is top left of window, (1, 1) is bot-right
	m_scale_x = static_cast<float>(x) / static_cast<float>(SDL_max(screen_surf_size.w, 1));
	m_scale_y = static_cast<float>(y) / static_cast<float>(SDL_max(screen_surf_size.h, 1));

	// Fix mouse position for weird aspect ratios
	m_scale_x -= static_cast<float>(screen_surf_size.x) / static_cast<float>(SDL_max(screen_surf_size.w, 1));
	m_scale_y -= static_cast<float>(screen_surf_size.y) / static_cast<float>(SDL_max(screen_surf_size.h, 1));

	// Use screenSize to calculate final mouse position
	mouse_pos = Vec2i(static_cast<int>(screen_size.x * m_scale_x), static_cast<int>(screen_size.y * m_scale_y));
	mouse_pos_relative = Vec2i(xRel, yRel);
}

