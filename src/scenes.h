#pragma once
#include "engine/world.h"

class SceneAudio : public Scene
{
public:
    SceneAudio() : Scene(RendererType::Sdl) {}

    void ready() override;
    void update() override;
    void draw() override;
    void destroy() override;
};

class SceneGpu : public Scene
{
public:
    SceneGpu() : Scene(RendererType::Gpu) {}

    void ready() override;
    void update() override;
    void draw() override;
    void destroy() override;
};

class SceneGpuBatcher : public Scene
{
public:
    SceneGpuBatcher() : Scene(RendererType::Gpu) {}

    void ready() override;
    void update() override;
    void draw() override;
    void destroy() override;
};
