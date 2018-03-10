#pragma once
#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"
#include "Box2D/Box2D.h"
#include "box.hpp"

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
    
    Field* _field;      //color the whole field
    Shuttle* _shuttle;  //player's shuttle
    Monster* _monster;  //temperary monster
    
    b2World * world;    //Box2D world
    
    Box * static_box;
    Box * dynamic_box;
    
    ObjectPool<Lines> lines_pool;   //the lines drawn by shuttle
    ObjectPool<Lines> chasing_lines_pool;   //chasing lines that caused by the hesitation of the player
    ObjectPool<Sparx> sparx_pool;   //sparxes
    
    ObjectPool<Box> walls;  //field's walls
    
    float die_time;
    int gameState;
    int life;
    float generate_time;    //caculate generate time_delay
    int sparx_num;
};

