#pragma once
#include "object_pool.h"
#include "avancezlib.h"
#include "component.h"
#include "game_object.h"
#include "Field.hpp"
#include "Lines.hpp"

#include <set>

class ShuttleBehaviourComponent : public Component
{
    
public:
    virtual ~ShuttleBehaviourComponent() {}
    
    virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects,Field* field, ObjectPool<Lines>* lines_pool, ObjectPool<Lines>* chasingLines);
    virtual void Init();
    
    virtual void Update(float dt);
    
    // move the player left or right
    bool Move(float moveH, float moveV, bool slow);
    int determineShuttleState(float posH, float posV);
    Lines* generateLine(float x,float y, float w, float h, int r, int g, int b, int a = 255);
    bool determineGridCollition(int i,int j,int* _grid);
    void resetLines();
    
private:
    
    Field* _field;
    int preState;
    int nowState;
    ObjectPool<Lines>* lines;
    ObjectPool<Lines>* chasingLines;
    
    int shuttleState;
    bool slowing;
    bool lineChasing;
    float growingLength;
    float chasingDelay;
    float moveStreak; //calculate the moving streak, it is used to determine the shuttle should be stopped while it moves out of the edge
    float stopDelay; // whenever the shuttle stop, it calculate the time of stop

};


// the main player
class Shuttle : public GameObject
{
public:
    
    virtual ~Shuttle(){  }
    
    virtual void Init(float h, float v);
    virtual void Receive(Message m);
    virtual void Update(float dt);
    void resetHit();
    
    
    bool canHit;
    float dieDelay;
    int state;
    bool hit;
    int life;
};

