#include "test_game.h"

#include "scenes.h"
#include "engine/math.h"

namespace
{
    int scene_index = 0;
    const int scene_index_max = 2;
}


void TestGame::ready(void *appstate)
{

}

void TestGame::update(void *appstate)
{
    int before_index = scene_index;
    if (input.key_pressed(SDLK_1))
    {
        // Prev scene
        scene_index--;
        scene_index = Math::clampi(scene_index, 0, scene_index_max);
        change_scene<SceneAudio>();
    }
    
    if (input.key_pressed(SDLK_2))
    {
        // Next scene
        scene_index++;
        scene_index = Math::clampi(scene_index, 0, scene_index_max);
        change_scene<SceneGpu>();
    }

    if (before_index != scene_index)
    {
        before_index = scene_index;

        switch (scene_index)
        {
        case 0: change_scene<SceneAudio>(); break;
        case 1: change_scene<SceneGpu>(); break;
        case 2: change_scene<SceneGpuBatcher>(); break;
        }
    }
}

void TestGame::draw(void *appstate)
{

}

void TestGame::quit(void *appstate, SDL_AppResult result)
{

}
