#pragma once
#include "Box2D/Box2D.h"
#include "game_object.h"
#include "avancezlib.h"
#include "component.h"
#include <set>
#include <iostream>

class Box : public GameObject
{
   
public:
    b2Body* body;
    virtual ~Box() { SDL_Log("Box::~Box"); }
    b2Body* getBody(){ return body; }
};


class BoxPhysicsComponent : public Component
{
    b2World* world;
    b2Body* body;
    
    float size_x, size_y;
    
public:
    
    virtual void Create(AvancezLib* system, b2World* world, Box * go, std::set<GameObject*> * game_objects, b2BodyType type, float pos_x, float pos_y, float size_x, float size_y)
    {
        Component::Create(system, go, game_objects);
        
        this->world = world;
        this->size_x = size_x;
        this->size_y = size_y;
        
        b2BodyDef def;
        def.type = type;
        def.position.Set(pos_x + size_x/2, pos_y + size_y/2);
        b2PolygonShape shape;
        shape.SetAsBox(this->size_x/2, this->size_y/2);
        
        
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = 0.01f;
        fixtureDef.friction = 0.f;
        fixtureDef.restitution = 1; //new code
        body = world->CreateBody(&def);
        body->CreateFixture(&fixtureDef);
        go->body = body;
    }
    
    virtual void Init(){
        go->width = this->size_x;
        go->height = this->size_y;
        Component::Init();
    }
    
    virtual void Update(float dt)
    {
        
        b2Vec2 position = body->GetPosition() - b2Vec2(size_x/2,size_y/2);
        float32 angle = body->GetAngle();
        
        go->horizontalPosition = position.x;
        go->verticalPosition = position.y;
        go->rotationAngle = angle *(180 / 3.14f);
        
        if (position.y < 0) // When the box reaches the top of the screen, it disappears.
            go->enabled = false;
    }
    
    virtual void Destroy()
    {
        world->DestroyBody(body);
        Component::Destroy();
    }
    
    virtual b2Body* getBody()
    {
        return body;
    }
    
    virtual ~BoxPhysicsComponent()
    {
        world->DestroyBody(body);
    }
};




