#include <cmath>
#include <iostream>
#include "Monster.hpp"
#include "DEFINISIONS.hpp"
#include "box.hpp"
#include <time.h>

void MonsterBehaviourComponent::Create(AvancezLib* system, b2World * world, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<Lines>* lines_pool)
{
    Component::Create(system, go, game_objects);
    lines = lines_pool;
    
}

void MonsterBehaviourComponent::Init()
{
    
}

void MonsterBehaviourComponent::Update(float dt)
{

    float posX = go->horizontalPosition + go->width/2;
    float posY = go->verticalPosition + go->height/2;
    
    if( determineLineCollition(posX, posY)){
        go->Send(HIT);
    }
}

bool MonsterBehaviourComponent::determineLineCollition(float posX, float posY)
{
    for(int i = 0; i < lines->size(); i++){
        Lines* now = lines->at(i);
        
        //in the line square
        if( now->horizontalPosition - posX <= go->width/2 && posX - now->horizontalPosition - now->width <= go->width/2 && now->verticalPosition - posY <= go->height/2 && posY - now->verticalPosition - now->height <= go->height/2)
        {
            return true;
        }
    }
    
    return false;
}

void Monster::Create(AvancezLib* system, b2World* world, GameObject * go, std::set<GameObject*> * game_objects,  Field* field, ObjectPool<Lines>* lines_pool, Shuttle* shuttle, float pos_x, float pos_y, float size_r){
    _system = system;
    _world = world;
    _game_objects = game_objects;
    _field = field;
    _lines_pool = lines_pool;
    _shuttle = shuttle;
    
    //generate the monster's body
    _monsterBody = new Circle();
    RenderComponent * monsterRender = new RenderComponent();
    monsterRender->Create(system, _monsterBody, game_objects, MONSTER_FILEPATH);
    
    MonsterBehaviourComponent * monsterBehaviour = new MonsterBehaviourComponent();
    monsterBehaviour->Create(system, world, _monsterBody, game_objects, lines_pool);
    
    CirclePhysicsComponent * monsterCirclePhysics = new CirclePhysicsComponent();
    monsterCirclePhysics->Create(system, world, _monsterBody, game_objects, b2_dynamicBody, pos_x , pos_y, size_r);
    
    _monsterBody->AddComponent(monsterRender);
    _monsterBody->AddComponent(monsterBehaviour);
    _monsterBody->AddComponent(monsterCirclePhysics);
    _monsterBody->Init();
    _monsterBody->AddReceiver(this);
    
    game_objects->insert(_monsterBody);
    
    
    //generate the tentalcles' segment
    tentacles.CreateEmpty();
    float linkLength = MONSTER_BODY_SIZE/2 - 1;
    //the tentacles' position, three tentalcles are at 90, 210, and 330 degrees to the monster's body
    double linkPoint[3][2] = {{ 0, linkLength}, {linkLength * std::cos( M_PI * 7/6 ), linkLength * std::sin( M_PI * 7/6 )},{linkLength * std::cos(M_PI * 11/6), linkLength * std::sin(M_PI * 11/6)}};
    
    for(int i = 0;i<30;i++)
        tentacles.push_back(generateTentacleSeg( pos_x + (i%10) * linkPoint[i/10][0], pos_y + (i%10) * linkPoint[i/10][1], 10, 5, i%10));
    
    //started to generate three tentacles
    for(int j = 0; j < 3; j++)
    {
        b2Body* link = tentacles.at(10 * j)->getBody();
        
        //use same definitions to create multiple bodies
        for (int i = 1; i < 10; i++) {
            b2Body* newLink = tentacles.at(10 * j + i)->getBody();
            b2RevoluteJointDef revoluteJointDef;
            revoluteJointDef.localAnchorA.Set( -4,0);
            revoluteJointDef.localAnchorB.Set(4,0);
            
            //inside the loop, only need to change the bodies to be joined
            revoluteJointDef.bodyA = link;
            revoluteJointDef.bodyB = newLink;
            _world->CreateJoint( &revoluteJointDef );
            
            link = newLink;//prepare for next iteration
        }
        
        b2RevoluteJointDef revoluteJointDef;
        //a revolute joint to connect the monster's body
        revoluteJointDef.bodyA = _monsterBody->getBody();
        revoluteJointDef.bodyB = tentacles.at(10*j)->getBody();
        revoluteJointDef.localAnchorA.Set(linkPoint[j][0], linkPoint[j][1]);
        revoluteJointDef.localAnchorB.Set(4,0);
        _world->CreateJoint( &revoluteJointDef );
    }
    
    //setup controllers
    _mainBody = _monsterBody->getBody();
    _tentaclesBody[0] = tentacles.at(9)->getBody();
    _tentaclesBody[1] = tentacles.at(19)->getBody();
    _tentaclesBody[2] = tentacles.at(29)->getBody();
    
    GameObject::Create();
}

Box* Monster::generateTentacleSeg( float pos_x, float pos_y, float size_x, float size_y, int r){
    Box* tentacleSeg = new Box();
    //create first link
    RenderComponent * tentacleRender = new RenderComponent();
    tentacleRender->CreateSingleColor(_system, tentacleSeg, _game_objects,   255 ,255 - r*25,0,255);
    MonsterBehaviourComponent * tentacleBehaviour = new MonsterBehaviourComponent();
    tentacleBehaviour->Create(_system, _world, tentacleSeg, _game_objects, _lines_pool);
    
    BoxPhysicsComponent * tentaclePhysics = new BoxPhysicsComponent();
    tentaclePhysics->Create(_system, _world, tentacleSeg, _game_objects, b2_dynamicBody, pos_x , pos_y, size_x, size_y);
    
    tentacleSeg->AddComponent(tentacleRender);
    tentacleSeg->AddComponent(tentacleBehaviour);
    tentacleSeg->AddComponent(tentaclePhysics);
    tentacleSeg->Init();
    tentacleSeg->AddReceiver(this);
    
    _game_objects->insert(tentacleSeg);
    
    return tentacleSeg;
}


void Monster::Init(float pos_x, float pos_y)
{
    SDL_Log("Monster::Init");
    rushTime = 0;
    wanderTime = 0;
    
    _mainBody->SetTransform(b2Vec2(FIELD_LEFT_OFFSET + FIELD_WIDTH/2 - MONSTER_BODY_SIZE/2, FIELD_TOP_OFFSET + FIELD_HEIGHT/2 - MONSTER_BODY_SIZE/2), 0);
    
    float linkLength = MONSTER_BODY_SIZE/2 - 1;
    //the tentacles' position, three tentalcles are at 90, 210, and 330 degrees to the monster's body
    double linkPoint[3][2] = {{ 0, linkLength}, {linkLength * std::cos( M_PI * 7/6 ), linkLength * std::sin( M_PI * 7/6 )},{linkLength * std::cos(M_PI * 11/6), linkLength * std::sin(M_PI * 11/6)}};
    for(int i = 0;i<30;i++)
        tentacles.at(i)->getBody()->SetTransform( b2Vec2(pos_x + (i%10) * linkPoint[i/10][0], pos_y + (i%10) * linkPoint[i/10][1]), 0);

    
    std::srand(time(NULL));
    float angle = std::rand()%360;
    wanderDir = b2Vec2( std::cos(angle/180 * M_PI), std::sin(angle/180 * M_PI));
    
    _mainBody->ApplyLinearImpulse(b2Vec2( wanderDir.x * 40,wanderDir.y * 40) , _mainBody->GetWorldCenter(), false);
    
    
    GameObject::Init();
    
}

void Monster::Update(float dt){
    
    float angle = _mainBody->GetAngle();
    float movingCloseStrength = 20;
    float tentacleCorrStrength = 10;
    float wallResistanceStrength = 1.5;
    float wanderStrength = 15;
    
    //Tentacle's movements
    _world->ClearForces();
    //chase to player
    b2Vec2 near = b2Vec2(_shuttle->horizontalPosition,_shuttle->verticalPosition) - _mainBody->GetPosition();
    near.Normalize();
    _mainBody->ApplyForce(b2Vec2(_mainBody->GetMass() * movingCloseStrength * near.x ,_mainBody->GetMass() * movingCloseStrength * near.y) , _mainBody->GetWorldCenter(), false);
    
    //wandering: change a random direction every 3 seconds
    wanderTime +=dt;
    
    if(wanderTime >= 3){
        wanderTime = 0;
        float angle = std::rand()%360;
        //std::cout<<angle<<"\n";
        wanderDir = b2Vec2( std::cos(angle/180 * M_PI), std::sin(angle/180 * M_PI));
    }
    
    //rush to the wander direction every 0.3 secs
    rushTime += dt;
    if(rushTime >= 0.3){
        rushTime = 0;
        _mainBody->ApplyLinearImpulse(b2Vec2( wanderDir.x * wanderStrength,wanderDir.y * wanderStrength) , _mainBody->GetWorldCenter(), false);
    }
    
    //resistance to the walls when the monster is too close to the walls
    float disX = wallDistanceH(_monsterBody->horizontalPosition + _monsterBody->width/2, _monsterBody->verticalPosition + _monsterBody->height/2);
    float disY = wallDistanceV(_monsterBody->horizontalPosition + _monsterBody->width/2, _monsterBody->verticalPosition + _monsterBody->height/2);
   
    if(disX > 0)
    {
        _mainBody->ApplyForce(b2Vec2( -(100 - disX) * wallResistanceStrength ,0) , _mainBody->GetWorldCenter(), false);
    }
    else if( disX < 0){
        _mainBody->ApplyForce(b2Vec2( (100 - disX) * wallResistanceStrength ,0) , _mainBody->GetWorldCenter(), false);
    }
    
    if(disY > 0)
    {
        _mainBody->ApplyForce(b2Vec2( 0 , -(100 - disY) * wallResistanceStrength) , _mainBody->GetWorldCenter(), false);
    }
    else if(disY < 0)
    {
        _mainBody->ApplyForce(b2Vec2( 0 , (100 - disY) * wallResistanceStrength) , _mainBody->GetWorldCenter(), false);
    }
    
    
    //each tentacle has a force to move in the right angle
    _tentaclesBody[0]->ApplyForce(b2Vec2(_mainBody->GetMass() * tentacleCorrStrength * std::cos(angle + M_PI/2) ,_mainBody->GetMass() * tentacleCorrStrength * std::sin(angle + M_PI/2)),  _tentaclesBody[0]->GetWorldCenter(), false);
    _tentaclesBody[1]->ApplyForce(b2Vec2( _mainBody->GetMass() * tentacleCorrStrength * std::cos(angle +M_PI * 7/6) ,_mainBody->GetMass() * tentacleCorrStrength* std::sin(angle + M_PI* 7/6)),  _tentaclesBody[1]->GetWorldCenter(), false);
    
     _tentaclesBody[2]->ApplyForce(b2Vec2( _mainBody->GetMass() * tentacleCorrStrength * std::cos(angle +M_PI* 11/6) ,_mainBody->GetMass() * tentacleCorrStrength * std::sin(angle * M_PI* 11/6)),  _tentaclesBody[2]->GetWorldCenter(), false);
    
    //setup monster's position to the field
    _field->setMonsterPos(_monsterBody->horizontalPosition, _monsterBody->verticalPosition);
    
    GameObject::Update(dt);
}

float Monster::wallDistanceH(float posX, float posY)
{
    for(int i = MONSTER_BODY_SIZE;i < 100; i++){
        if(posX + i - FIELD_LEFT_OFFSET >= FIELD_WIDTH)return i;
        if(posX + i - FIELD_LEFT_OFFSET < FIELD_WIDTH &&
           _field->getGrid()[(int)(posY - FIELD_TOP_OFFSET) * FIELD_WIDTH + (int)posX + i - FIELD_LEFT_OFFSET] != FIELD_STATE_EMPTY) return i;
        
        if(posX - i - FIELD_LEFT_OFFSET < 0)return -i;
        if(posX - i - FIELD_LEFT_OFFSET >= 0 &&
           _field->getGrid()[(int)(posY - FIELD_TOP_OFFSET) * FIELD_WIDTH + (int)posX - i - FIELD_LEFT_OFFSET] != FIELD_STATE_EMPTY) return -i;
    }
    return 0;
}

float Monster::wallDistanceV(float posX, float posY)
{
    
    for(int i = MONSTER_BODY_SIZE;i < 100; i++){
        if(posY + i - FIELD_TOP_OFFSET >= FIELD_HEIGHT)return i;
        
        if(posY + i - FIELD_TOP_OFFSET < FIELD_HEIGHT &&
           _field->getGrid()[(int)(posY + i - FIELD_TOP_OFFSET) * FIELD_WIDTH + (int)posX - FIELD_LEFT_OFFSET] != FIELD_STATE_EMPTY) return i;
        
        if(posY - i - FIELD_TOP_OFFSET < 0)return -i;
        if(posY - i - FIELD_TOP_OFFSET >= 0 &&
           _field->getGrid()[(int)(posY - i - FIELD_TOP_OFFSET) * FIELD_WIDTH + (int)posX - FIELD_LEFT_OFFSET] != FIELD_STATE_EMPTY) return -i;
    }
    return 0;
}

void Monster::Receive(Message m)
{
    if(m == HIT)
        Send(HIT);
}

