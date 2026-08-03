#include "test_game.h"

#include "engine/math.h"

namespace
{
    int scene_index = 0;
    const int scene_index_max = 0;
}


void TestGame::ready(void *appstate)
{

}

void TestGame::update(void *appstate)
{
    if (input.key_pressed(SDLK_1))
    {
        // Prev scene
        scene_index--;
        scene_index = Math::clampi(scene_index, 0, scene_index_max);
    }

    if (input.key_pressed(SDLK_2))
    {
        // Next scene
        scene_index++;
        scene_index = Math::clampi(scene_index, 0, scene_index_max);
    }
    
}

void TestGame::draw(void *appstate)
{

}

void TestGame::quit(void *appstate, SDL_AppResult result)
{

}
