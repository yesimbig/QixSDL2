#pragma once
#include "object_pool.h"
#include "avancezlib.h"
#include "component.h"
#include "game_object.h"
#include "Lines.hpp"
#include "Field.hpp"


class MonsterBehaviourComponent : public Component
{
    
public:
    virtual ~MonsterBehaviourComponent() {}
    
    virtual void Create(AvancezLib* system, b2World * world, GameObject * go, std::set<GameObject*> * game_objects,Field* field, ObjectPool<Lines>* lines_pool);
    virtual void Init();
    
    virtual void Update(float dt);
    
    //bool Move(float dt);
    
    bool determineLineCollition(float posX, float posY);
    float distance(float posX1, float posY1, float posX2, float posY2);
    
private:
    
    Field* _field;
    ObjectPool<Lines>* lines;
    Box * _monster;
    
    float speedH, speedV;
};


class Monster: public GameObject
{
public:
    Box * _monster;
    
    virtual ~Monster(){  }
    
    virtual void Init();
    virtual void Receive(Message m);
    
};
