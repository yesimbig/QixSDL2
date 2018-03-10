#include <cmath>
#include <iostream>
#include "Monster.hpp"
#include "DEFINISIONS.hpp"
#include "box.hpp"

void MonsterBehaviourComponent::Create(AvancezLib* system, b2World * world, GameObject * go, std::set<GameObject*> * game_objects,Field* field, ObjectPool<Lines>* lines_pool)
{
    Component::Create(system, go, game_objects);
    
    _field = field;
    lines = lines_pool;
    
}

void MonsterBehaviourComponent::Init()
{
    /*std::srand(std::time(NULL));
    float angle = std::rand()%360;
    speedH = MONSTER_SPEED * std::sin(angle/180 * M_PI);
    speedV = MONSTER_SPEED * std::cos(angle/180 * M_PI);
    */

    
}

void MonsterBehaviourComponent::Update(float dt)
{

    float posX = go->horizontalPosition + go->width/2;
    float posY = go->verticalPosition + go->height/2;
    
    if( determineLineCollition(posX, posY)){
        go->Send(HIT);
    }
    
    _field->setMonsterPos(go->horizontalPosition, go->verticalPosition);
}

bool MonsterBehaviourComponent::determineLineCollition(float posX, float posY)
{
    
    for(int i = 0; i < lines->size(); i++){
        Lines* now = lines->at(i);
        
        //in the line square
        if( now->horizontalPosition - posX <= go->width/2 && posX - now->horizontalPosition - now->width <= go->width/2 && now->verticalPosition - posY <= go->height/2 && posY - now->verticalPosition - now->height <= go->height)
        {
            return true;
        }
    }
    
    return false;
}

float MonsterBehaviourComponent::distance(float posX1, float posY1, float posX2, float posY2)
{
    return std::sqrt((posX1-posX2)*(posX1-posX2) + (posY1-posY2)*(posY1-posY2));
}
               

void Monster::Init()
{
    
    this->width = MONSTER_SIZE;
    this->height = MONSTER_SIZE;
    
    SDL_Log("Monster::Init");
    GameObject::Init();
    
}

void Monster::Receive(Message m)
{
    
}

