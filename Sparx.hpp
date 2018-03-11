#pragma once
#include "object_pool.h"
#include "avancezlib.h"
#include "component.h"
#include "game_object.h"
#include "Lines.hpp"
#include "Field.hpp"
#include "Shuttle.hpp"

class SparxBehaviourComponent : public Component
{
    
public:
    virtual ~SparxBehaviourComponent() {}
    
    virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects,Field* field,Shuttle* shuttle, bool t);
    virtual void Init();
    
    virtual void Update(float dt);
private:
    
    Field* _field;
    Shuttle* _shuttle;
    
    bool turn;
    struct Point{
        int x,y;
        Point(int a,int b): x(a),y(b) {}
    };
};


class Sparx: public GameObject
{
public:
    
    virtual ~Sparx(){  }
    
    virtual void Init(float h, float v);
    virtual void Receive(Message m);


};
