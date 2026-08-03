#pragma once
#include "engine/world.h"

class SceneAudio : public Scene
{
public:
    void ready() override;
    void update() override;
    void draw() override;
    void destroy() override;
};

class SceneGl : public Scene
{
public:
    void ready() override;
    void update() override;
    void draw() override;
    void destroy() override;
};
