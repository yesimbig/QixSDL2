#pragma once
#include "object_pool.h"
#include "avancezlib.h"
#include "component.h"
#include "game_object.h"

class LinesBehaviourComponent : public Component
{
public:
    ~LinesBehaviourComponent() {}
    
    virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects);
    virtual void Set(int x,int y, int w,int h);
    virtual void Update(float dt);
    
private:
    
};

class Lines : public GameObject
{
public:
    ~Lines(){  }
    
    virtual void Init();
    virtual void Receive(Message m);
    virtual void set(int x1, int y1, int x2, int y2);
    
    LinesBehaviourComponent* controller;
    int firstX, firstY, lastX, lastY;
private:

    
};



