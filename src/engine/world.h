#pragma once
#include "graphics.h"

#include <vector>

class Scene;
class Entity;
class Component;
class Game;


class Scene
{
    friend Game;

private:

protected:
    Game *game = nullptr;
    
public:
    RendererType renderer_type = RendererType::Sdl;

    Scene(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene &operator=(const Scene&) = delete;
    Scene &operator=(Scene&&) = delete;
    
    Scene() = default;
    Scene(RendererType _renderer_type) : renderer_type(_renderer_type) {}
    virtual ~Scene() = default;

    virtual void ready() {}
    virtual void update() {}
    virtual void draw() {}
    virtual void destroy() {}
};


class Entity
{
private:
    
public:

};

class Component
{
private:
    
public:

};