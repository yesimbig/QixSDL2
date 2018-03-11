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
    
    bool defineKeyPress(AvancezLib::KeyStatus keys, float dt); //handle the key's statement, and return if the shuttle moves or not
    bool Move(float moveH, float moveV, bool slow); //handle shuttle's movement, and return if the next step is correct or not
    int determineShuttleState(float posH, float posV); //determine the position's statement (edge or middle)
    
    Lines* generateLine(float x,float y, float w, float h, int r, int g, int b, int a = 255); //generate a line segment
    
    void removeBiasOfLastSeg(int x, int y, int preX, int preY, int nowState); //if the last seg of lines doesn't connect on the edge or the wall, it remove the bias
    void removeBiasOfShuttlesPos(int x, int y); //if the shuttle isn't on the edge or the wall, it remove the bias
    
    int determineGridState(int i,int j,int* _grid); //return gird's statement
    
    void setChasingLines(int chasingSize, int lineSize, float dt); //setup the chasing line's movement
    
private:
    
    Field* _field;
    ObjectPool<Lines>* lines;
    ObjectPool<Lines>* chasingLines;
    
    int preState;   //previous moving direction
    int nowState;   //current moving direction
    int shuttleState; //shuttle statement: on edge or on middle
    bool slowing;   //slow mode
    bool lineChasing;   //the current line is chasing
    float growingLength; //current length of the chasing line on the seg
    float chasingDelay;  //caclate the chasing line's delay time
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
    
    
    bool canHit;    //shuttle can be hit or not
    float dieDelay; //after it revives, it has time that cannot be hit
};

