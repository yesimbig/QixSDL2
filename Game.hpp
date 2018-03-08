#pragma once
#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"

#include "game_object.h"
#include "avancezlib.h"
#include "Shuttle.hpp"
#include "Field.hpp"
#include "Lines.hpp"
#include "Monster.hpp"
#include "Sparx.hpp"

#include <iostream>
#include <set>

class Game : public GameObject{
    
public:
    Game();
    ~Game();
    
    virtual void Init(AvancezLib* system);
    
    virtual void HandleEvents();
    virtual void Update(float dt);
    virtual void Render();
    virtual void Receive(Message m);
    virtual void Clean();
    
private:
    std::set<GameObject*> game_objects;
    
    AvancezLib* _system;
    
    Field* _field;
    Shuttle* _shuttle;
    Monster* _monster;
    
    ObjectPool<Lines> lines_pool;
    ObjectPool<Lines> chasing_lines_pool;
    ObjectPool<Sparx> sparx_pool;
    
    float die_time;
    int gameState;
    int life;
};

