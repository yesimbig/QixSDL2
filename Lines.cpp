#include "Lines.hpp"
#include "DEFINISIONS.hpp"
void Lines::Init()
{
    GameObject::Init();
}

void Lines::Receive(Message m)
{

}

void Lines::set(int x1, int y1, int x2, int y2){
    
    lastX = x2; lastY = y2;
    if(x1<x2){ // left->right
        controller->Set(x1,y1,x2-x1+LINE_WIDTH,LINE_WIDTH);
    }
    else if(x1>x2){ // right->left
        controller->Set(x2,y2,x1-x2,LINE_WIDTH);
    }
    else if(y1<y2){ //up->down
        controller->Set(x1,y1,LINE_WIDTH,y2-y1+LINE_WIDTH);
    }
    else if(y1>y2){ //down->up
        controller->Set(x2,y2,LINE_WIDTH,y1-y2);
    }
}

void LinesBehaviourComponent::Create(AvancezLib *system, GameObject *go, std::set<GameObject *> *game_objects){
    Component::Create(system, go, game_objects);
}

void LinesBehaviourComponent::Set(int x,int y, int w,int h)
{
    go->horizontalPosition = x;
    go->verticalPosition = y;
    go->width = w;
    go->height = h;
}

void LinesBehaviourComponent::Update(float dt)
{
    
}
