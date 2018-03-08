#include <cmath>
#include <iostream>
#include "Monster.hpp"
#include "DEFINISIONS.hpp"

void MonsterBehaviourComponent::Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects,Field* field, ObjectPool<Lines>* lines_pool)
{
    Component::Create(system, go, game_objects);
    
    _field = field;
    lines = lines_pool;
}

void MonsterBehaviourComponent::Init()
{
    std::srand(std::time(NULL));
    float angle = std::rand()%360;
    speedH = MONSTER_SPEED * std::sin(angle/180 * M_PI);
    speedV = MONSTER_SPEED * std::cos(angle/180 * M_PI);
}

void MonsterBehaviourComponent::Update(float dt)
{
    Move(dt);
    //std::cout<<go->horizontalPosition<<" "<<go->verticalPosition<<"\n";
    _field->setMonsterPos(go->horizontalPosition, go->verticalPosition);
    
}

bool MonsterBehaviourComponent::Move(float dt)
{
    float moveH = speedH * dt, moveV = speedV * dt;
    float posX1 = go->horizontalPosition;
    float posX2 = go->horizontalPosition + go->width;
    float posY1 = go->verticalPosition;
    float posY2 = go->verticalPosition + go->height;

    if( determineLineCollition(posX1, posX2, posY1, posY2, moveH, moveV)){
        go->Send(HIT);
    }
    
    determineBorderCollition(posX1, posX2, posY1, posY2, moveH, moveV);

    determineGridCollition(posX1, posX2, posY1, posY2, moveH, moveV);
    
    go->horizontalPosition += moveH;
    go->verticalPosition += moveV;
    
    return true;
}

void MonsterBehaviourComponent::determineBorderCollition(float posX1, float posX2, float posY1, float posY2, float &moveH, float &moveV)
{
    if(moveH > 0 && posX2 + moveH >= FIELD_LEFT_OFFSET + FIELD_WIDTH)
    {
        moveH *= -1;
        speedH *= -1;
    }
    else if(moveH < 0 && posX1 + moveH <= FIELD_LEFT_OFFSET)
    {
        moveH *= -1;
        speedH *= -1;
    }
    
    if(moveV > 0 && posY2 + moveV >= FIELD_TOP_OFFSET + FIELD_HEIGHT)
    {
        moveV *= -1;
        speedV *= -1;
    }
    else if(moveV < 0 && posY1 + moveV <= FIELD_TOP_OFFSET)
    {
        moveV *= -1;
        speedV *= -1;
    }
    
}

bool MonsterBehaviourComponent::determineLineCollition(float posX1, float posX2, float posY1, float posY2, float& moveH, float& moveV)
{
    if(moveH > 0 && determineCrossLineH(posX2, posX2 + moveH, posY1 , posY2))
    {
        return true;
    }
    else if(moveH < 0 && determineCrossLineH(posX1, posX1 + moveH, posY1, posY2 ))
    {
        return true;
    }
    
    if(moveV > 0 && determineCrossLineV(posY2, posY2 + moveV, posX1, posX2))
    {
        return true;
    }
    else if(moveV < 0 && determineCrossLineV(posY1, posY1 + moveV, posX1, posX2) )
    {
        return true;
    }
    
    return false;
}

void MonsterBehaviourComponent::determineGridCollition(float posX1, float posX2, float posY1, float posY2, float &moveH, float &moveV)
{
    int * grid = _field->getGrid();
    
    float altPosX1 = posX1 - FIELD_LEFT_OFFSET;
    float altPosX2 = posX2 - FIELD_LEFT_OFFSET;
    float altPosY1 = posY1 - FIELD_TOP_OFFSET;
    float altPosY2 = posY2 - FIELD_TOP_OFFSET;
    
    if(moveH > 0 && altPosX2 + moveH < FIELD_WIDTH)
    {
        for(int i = (int)altPosY1; i<= (int)altPosY2; i++){
            if( grid[ i*FIELD_WIDTH + (int)(altPosX2 + moveH)] == FIELD_STATE_OCCUPIED){
                moveH *= -1;
                speedH *= -1;
                break;
            }
        }
    }
    else if(moveH < 0 && altPosX2 + moveH >= 0)
    {
        for(int i = (int)altPosY1; i<= (int)altPosY2; i++){
            if( grid[ i*FIELD_WIDTH + (int)(altPosX1 + moveH)] == FIELD_STATE_OCCUPIED){
                moveH *= -1;
                speedH *= -1;
                break;
            }
        }
    }
    
    if(moveV > 0 && altPosY2 + moveV < FIELD_HEIGHT)
    {
        for(int i = (int)altPosX1; i<= (int)altPosX2; i++){
            if( grid[ (int)(altPosY2 + moveH)*FIELD_WIDTH + i] == FIELD_STATE_OCCUPIED){
                moveV *= -1;
                speedV *= -1;
                break;
            }
        }
    }
    else if(moveV < 0 && altPosY1 + moveV >= 0)
    {
        for(int i = (int)altPosX1; i<= (int)altPosX2; i++){
            if( grid[ (int)(altPosY1 + moveH)*FIELD_WIDTH + i] == FIELD_STATE_OCCUPIED){
                moveV *= -1;
                speedV *= -1;
                break;
            }
        }
    }
    
}

bool MonsterBehaviourComponent::determineCrossLineH(float preX, float nowX, float Y1, float Y2)
{
    for(int i = 0; i < lines->size(); i++){
        Lines* now = lines->at(i);
        if(now->height>LINE_WIDTH) //vertical line
        {
            if( (Y2 - now->verticalPosition)*(Y1 - now->verticalPosition- now->height) <= 0 &&
                (preX - now->horizontalPosition - now->width/2)*(nowX - now->horizontalPosition - now->width/2) <= 0)
                return true;
        }
    }
     return false;
}

bool MonsterBehaviourComponent::determineCrossLineV(float preY, float nowY, float X1, float X2)
{
    for(int i = 0; i < lines->size(); i++){
        Lines* now = lines->at(i);
        if(now->width>LINE_WIDTH) //horizontal line
        {
            if( (X2 - now->horizontalPosition)*(X1 - now->horizontalPosition- now->width) <= 0 &&
               (preY - now->verticalPosition - now->height/2)*(nowY - now->verticalPosition - now->height/2) <= 0)
                return true;
        }
    }
    return false;
}

void Monster::Init(float h, float v)
{
    this->horizontalPosition = h;
    this->verticalPosition = v;
    this->width = MONSTER_SIZE;
    this->height = MONSTER_SIZE;
    
    SDL_Log("Monster::Init");
    GameObject::Init();
    
}

void Monster::Receive(Message m)
{
    
}

