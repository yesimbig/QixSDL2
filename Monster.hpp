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
    
    virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects,Field* field, ObjectPool<Lines>* lines_pool);
    virtual void Init();
    
    virtual void Update(float dt);
    
    bool Move(float dt);
    
    void determineBorderCollition(float posX1, float posX2, float posY1, float posY2, float& moveH, float& moveV);
    bool determineLineCollition(float posX1, float posX2, float posY1, float posY2, float& moveH, float& moveV);
    void determineGridCollition(float posX1, float posX2, float posY1, float posY2, float& moveH, float& moveV);
    
    bool determineCrossLineH(float preX, float nowX, float Y1, float Y2);
    bool determineCrossLineV(float preV, float nowV, float X1, float X2);
private:
    
    Field* _field;
    ObjectPool<Lines>* lines;
    
    float speedH, speedV;
};


class Monster: public GameObject
{
public:
    
    virtual ~Monster(){  }
    
    virtual void Init(float h, float v);
    virtual void Receive(Message m);
    
};
