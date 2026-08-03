#pragma once
#include "engine/game.h"

class TestGame : public Game
{
protected:
    void ready(void *appstate) override;
    void update(void *appstate) override;
    void draw(void *appstate) override;
    void quit(void *appstate, SDL_AppResult result) override;

public:

};