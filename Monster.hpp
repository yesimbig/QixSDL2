#pragma once
#include "object_pool.h"
#include "avancezlib.h"
#include "component.h"
#include "game_object.h"
#include "Lines.hpp"
#include "Field.hpp"
#include "circle.hpp"
#include "Shuttle.hpp"

class MonsterBehaviourComponent : public Component
{
    
public:
    virtual ~MonsterBehaviourComponent() {}
    
    virtual void Create(AvancezLib* system, b2World * world, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<Lines>* lines_pool);
    virtual void Init();
    
    virtual void Update(float dt);
    
    bool determineLineCollition(float posX, float posY);    //check if every the monster collide on the lines or not
    
private:
    
    ObjectPool<Lines>* lines;   //the lines drawn by the shuttle
    Box * _monster;     //main body of the monster
    
};


class Monster: public GameObject
{
    AvancezLib* _system;
    b2World* _world;
    std::set<GameObject*> * _game_objects;
    
    Field* _field;
    ObjectPool<Lines>* _lines_pool;
    Shuttle* _shuttle;
    
    b2Body* _mainBody;          //monster's main body
    b2Body* _tentaclesBody[3];  //three tentacles
    
    
public:
    Circle *_monsterBody;
    ObjectPool<Box> tentacles;
    float wanderTime;
    float rushTime;
    b2Vec2 wanderDir;
    
    virtual ~Monster(){ }
    
    virtual void Create(AvancezLib* system, b2World* world, GameObject * go, std::set<GameObject*> * game_objects,  Field* field, ObjectPool<Lines>* lines_pool, Shuttle* shuttle, float pos_x, float pos_y, float size_r);
    
    virtual void Update(float dt);
    
    virtual void Init(float posX, float posY);
    virtual void Receive(Message m);
    
    Box* generateTentacleSeg(float pos_x, float pos_y, float size_x, float size_y, int r);
    
    float wallDistanceH(float posX, float posV);    //calculate the horizontal distance to the nearest wall
    float wallDistanceV(float posX, float posV); //calculate the vertical distance to the nearest wall
    
};
