#pragma once
#include <set>
#include "object_pool.h"
#include "game_object.h"
#include "avancezlib.h"
#include <vector>

class GameObject;
class AvancezLib;
class Sprite;

class Component
{
protected:
	AvancezLib * system;	// used to access the system
	GameObject * go;		// the game object this component is part of
	std::set<GameObject*> * game_objects;	// the global container of game objects

public:
	virtual ~Component() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects);

	virtual void Init() {}
	virtual void Update(float dt) = 0;
	virtual void Receive(int message) {}
	virtual void Destroy() {}
};


class RenderComponent : public Component
{
    std::vector<Sprite*> sprite; // I switch sprite into vector so that the program can handle animation of each object
    int frameSize; //how many frames in the object
    float frameRate; //each frame costs how much time
    int nowFrame; //record current frame
    float nowTime; //record current frame time
public:

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, const char * sprite_name, int frameSize = 1, float frameRate = 0);
    virtual void CreateSingleColor(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, int r,int g, int b, int a);
    virtual void Update(float dt);
	virtual void Destroy();

	//Sprite * GetSprite() { return sprite; }
};


class CollideComponent : public Component
{
	ObjectPool<GameObject> * coll_objects; // collision will be tested with these objects

public:
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects);
	virtual void Update(float dt);
};

