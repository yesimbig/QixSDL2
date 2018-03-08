#include "component.h"
#include <iostream>
void Component::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects)
{
	this->go = go;
	this->system = system;
	this->game_objects = game_objects;
}

void RenderComponent::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, const char * sprite_name, int fs, float fr)
{
    sprite.resize(fs);
	Component::Create(system, go, game_objects);
    
    if(fs == 1)  sprite.at(0) = system->createSprite(sprite_name);
    else
        for(int i = 0;i < fs; i++)
        {
            char str[256];
            sprintf(str,"%s%d.png", sprite_name,i);
            sprite.at(i) = system->createSprite(str);
        }
       
    frameSize = fs;
    frameRate = fr;
    nowFrame = 0;
    nowTime = 0;
}

void RenderComponent::CreateSingleColor(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, int r,int g, int b, int a)
{
    Component::Create(system, go, game_objects);
    
    sprite.resize(1);
    sprite[0] = system->createSpriteByColor(r, g, b, a);
    frameSize = 1;
    frameRate = 0;
    nowFrame = 0;
    nowTime = 0;
}

void RenderComponent::Update(float dt)
{
	if (!go->enabled || !sprite.at(nowFrame))
		return;
    
    nowTime += dt;
    if(frameRate != 0 && nowTime >= frameRate)
    {
        nowTime = 0;
        nowFrame++;
        if(nowFrame >= frameSize)nowFrame = 0;
    }
    
	if (sprite[nowFrame] && !go->width && !go->height)
		sprite[nowFrame] ->draw(int(go->horizontalPosition), int(go->verticalPosition), go->rotationAngle);
    else
        sprite[nowFrame] ->draw(int(go->horizontalPosition), int(go->verticalPosition), go->rotationAngle,go->width,go->height);
}

void RenderComponent::Destroy()
{
    for(int i = 0;i < frameSize; i++)
    {
        if (sprite[i] != NULL)
            sprite[i]->destroy();
        sprite[i] = NULL;
    }
}


void CollideComponent::Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects)
{
	Component::Create(system, go, game_objects);
	this->coll_objects = coll_objects;
}


void CollideComponent::Update(float dt)
{
	for (auto i = 0; i < coll_objects->pool.size(); i++)
	{
		GameObject * go0 = coll_objects->pool[i];
		if (go0->enabled && go0->collisionEnabled)
		{
			if ((go->horizontalPosition + go->width/2 - go0->horizontalPosition >  0) &&
				(go->horizontalPosition - go0->horizontalPosition <  go0->width) &&
				(go->verticalPosition  + go->height/2 - go0->verticalPosition > 0) &&
				(go->verticalPosition - go0->verticalPosition < go0->height))
			{
				go->Receive(HIT);
                return;
			}
		}
	}

}
