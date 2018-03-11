#pragma once
#include "Box2D/Box2D.h"
#include "game_object.h"
#include "avancezlib.h"
#include "component.h"
#include <set>
#include <iostream>

class Circle : public GameObject
{
public:
    b2Body* body;
    virtual ~Circle() { SDL_Log("Circle::~Circle"); }
    b2Body* getBody(){ return body; }
};


class CirclePhysicsComponent : public Component
{
    b2World* world;
    b2Body* body;
    
    float size_r;
    
public:
    
    virtual void Create(AvancezLib* system, b2World* world, Circle * go, std::set<GameObject*> * game_objects, b2BodyType type, float pos_x, float pos_y, float size_r)
    {
        Component::Create(system, go, game_objects);
        
        this->world = world;
        this->size_r = size_r;
        
        b2BodyDef def;
        def.type = type;
        def.position.Set(pos_x + size_r/2, pos_y + size_r/2);
        b2CircleShape shape;
        shape.m_radius = size_r/2;
        
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
        go->width = this->size_r;
        go->height = this->size_r;
        Component::Init();
    }
    
    virtual void Update(float dt)
    {
        
        b2Vec2 position = body->GetPosition() - b2Vec2(size_r/2,size_r/2);
        float32 angle = body->GetAngle();
        
        go->horizontalPosition = position.x;
        go->verticalPosition = position.y;
        go->rotationAngle = angle *(180 / 3.14f);
        
    }
    
    virtual b2Body* getBody()
    {
        return body;
    }
    
    virtual ~CirclePhysicsComponent()
    {
        world->DestroyBody(body);
    }
};




