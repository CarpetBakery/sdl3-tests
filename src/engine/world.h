#pragma once
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
    Scene(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene &operator=(const Scene&) = delete;
    Scene &operator=(Scene&&) = delete;
    
    Scene() = default;
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