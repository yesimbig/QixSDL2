#include "Sparx.hpp"
#include <queue>

void SparxBehaviourComponent::Create(AvancezLib *system, GameObject *go, std::set<GameObject *> *game_objects,Field* field, Shuttle* shuttle, bool t){
    Component::Create(system, go, game_objects);
    _field = field;
    _shuttle = shuttle;
    turn = t;
}

void SparxBehaviourComponent::Init()
{
    
}

void SparxBehaviourComponent::Update(float dt)
{
    if(dt == 0) return;
    
    //collision with shuttle
    if(_shuttle->canHit)
    {
        float ShuttleMidX = _shuttle->horizontalPosition + _shuttle->width/2;
        float ShuttleMidY = _shuttle->verticalPosition + _shuttle->height/2;
        float SparxMidX = go->horizontalPosition + go->width/2;
        float SparxMidY = go->verticalPosition + go->height/2;
        if(std::abs(ShuttleMidX-SparxMidX)<=5 && std::abs(ShuttleMidY-SparxMidY)<=5){
            go->Send(HIT);
        }
    }
    
    
    float altPosx = go->horizontalPosition + go->width/2 - FIELD_LEFT_OFFSET;
    float altPosy = go->verticalPosition + go->height/2 - FIELD_TOP_OFFSET;
    int* dirGrid;
    
    //reverse map
    if(!turn) dirGrid = _field->getReDirGrid();
    else dirGrid = _field->getDirGrid();
    
    //find the nearest point on dirField that has the direction data
    if(dirGrid[(int)altPosy*FIELD_WIDTH + (int)altPosx]==0){
        for(int i = 1;i<=LINE_WIDTH;i++){
            if(altPosx - i>=0 && dirGrid[(int)altPosy*FIELD_WIDTH + (int)altPosx - i] != 0){
                altPosx-=(float)i;
                break;
            }
            if(altPosx + i < FIELD_WIDTH && dirGrid[(int)altPosy*FIELD_WIDTH + (int)altPosx + i] != 0){
                altPosx+=(float)i;
                break;
            }
            if(altPosy - i >= 0 && dirGrid[(int)(altPosy-i)*FIELD_WIDTH + (int)altPosx] != 0){
                altPosy-=(float)i;
                break;
            }
            if(altPosy + i < FIELD_HEIGHT && dirGrid[(int)(altPosy+i)*FIELD_WIDTH + (int)altPosx] != 0){
                altPosy+=(float)i;
                break;
            }
        }
    }
    
    float nextPosx = altPosx;
    float nextPosy = altPosy;
   
    //find the nearest point that switch the direction, and follow the path
    if( dirGrid[(int)altPosy*FIELD_WIDTH + (int)altPosx] == FIELD_LEFT){
        
        int i;
        for(i = 1; i< (int)(SPARX_SPEED*dt); i++){
            if(altPosx - i <= 0)break;
            if(!dirGrid[(int)altPosy*FIELD_WIDTH + (int)altPosx - i]){
                i--; break;
            }
            if(dirGrid[(int)altPosy*FIELD_WIDTH + (int)altPosx - i]!= dirGrid[(int)altPosy*FIELD_WIDTH + (int)altPosx]) break;
        }
        if(i == (int)(SPARX_SPEED*dt) +1)
            nextPosx -= SPARX_SPEED*dt;
        else
            nextPosx -= i;
    }
    else if( dirGrid[(int)altPosy*FIELD_WIDTH + (int)altPosx] == FIELD_RIGHT){
        int i;
        for(i = 1; i< (int)(SPARX_SPEED*dt); i++){
            if(altPosx + i >= FIELD_WIDTH-1)break;
            if(!dirGrid[(int)altPosy*FIELD_WIDTH + (int)altPosx + i]){
                i--;break;
            }
            if(dirGrid[(int)altPosy*FIELD_WIDTH + (int)altPosx + i]!= dirGrid[(int)altPosy*FIELD_WIDTH + (int)altPosx]) break;
        }
        if(i == (int)(SPARX_SPEED*dt) +1)
            nextPosx += SPARX_SPEED*dt;
        else
            nextPosx += i;
    }
    else if( dirGrid[(int)altPosy*FIELD_WIDTH + (int)altPosx] == FIELD_UP){
        int i;
        for(i = 1; i< (int)(SPARX_SPEED*dt); i++){
            if(altPosy - i <= 0)break;
            if(!dirGrid[(int)(altPosy-i)*FIELD_WIDTH + (int)altPosx]){
                i--; break;
            }
            if(dirGrid[(int)(altPosy-i)*FIELD_WIDTH + (int)altPosx]!= dirGrid[(int)altPosy*FIELD_WIDTH + (int)altPosx]) break;
        }
        if(i == (int)(SPARX_SPEED*dt) +1)
            nextPosy -= SPARX_SPEED*dt;
        else
            nextPosy -= i;
    }
    else if( dirGrid[(int)altPosy*FIELD_WIDTH + (int)altPosx] == FIELD_DOWN){
        int i;
        for(i = 1; i< (int)(SPARX_SPEED*dt); i++){
            if(altPosy + i >= FIELD_HEIGHT - 1)break;
            if(!dirGrid[(int)(altPosy+i)*FIELD_WIDTH + (int)altPosx]){
                i--; break;
            }
            if(dirGrid[(int)(altPosy+i)*FIELD_WIDTH + (int)altPosx]!= dirGrid[(int)altPosy*FIELD_WIDTH + (int)altPosx]) break;
        }
        if(i == (int)(SPARX_SPEED*dt) +1)
            nextPosy += SPARX_SPEED*dt;
        else
            nextPosy += i;
    }
    
    go->horizontalPosition = nextPosx - go->width/2 + FIELD_LEFT_OFFSET;
    go->verticalPosition = nextPosy - go->height/2 + FIELD_TOP_OFFSET;
}


void Sparx::Init(float h, float v)
{
    this->horizontalPosition = h;
    this->verticalPosition = v;
    this->width = SPARX_SIZE;
    this->height = SPARX_SIZE;
    
    SDL_Log("Sparx::Init");
    GameObject::Init();
}

void Sparx::Receive(Message m)
{
    
}
