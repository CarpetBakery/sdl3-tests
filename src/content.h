#pragma once
#include <SDL3/SDL.h>

class Game;

namespace Content
{
    void load(Game *game);
    void unload();
}