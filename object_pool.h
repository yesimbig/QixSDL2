#pragma once
#include "SDL2/SDL.h"
#include <vector>

template <class T>
class ObjectPool
{
public:

	void Create(unsigned int num_objects)
	{
		Deallocate();

		for (unsigned int i = 0; i < num_objects; i++)
		{
			T * t = new T();
			pool.push_back(t);
		}
	}

    void CreateEmpty()
    {
        Deallocate();
    }
    
    void push_back(T* t){
        pool.push_back(t);
    }
    
    T* at(int i){
        return pool.at(i);
    }
    
    int size(){
        return pool.size();
    }
    
	void Destroy()
	{
		for (auto it = pool.begin(); it != pool.end(); it++)
			(*it)->Destroy();
	}

	void Deallocate()
	{
		SDL_Log("ObjectPool::Deallocating ");
        for (typename std::vector<T*>::iterator it = pool.begin(); it != pool.end(); it++)
			delete *it;
	}

    void clear(){
        pool.clear();
    }
    
	~ObjectPool()
	{
		SDL_Log("ObjectPool::~ObjectPool");
		Deallocate();
	}

	T* FirstAvailable()
	{
        for (typename std::vector<T*>::iterator it = pool.begin(); it != pool.end(); it++)
			if (!(**it).enabled)
				return (*it);

		// if it reaches this point, there is no available object in the pool
		return NULL;
	}

	// sleect a random, enabled element in the object pool
	T* SelectRandom()
	{
		int offset = (rand() / (float)RAND_MAX) * pool.size();

		for (int i = 0; i < pool.size(); i++)
		{
			int index = (i + offset) % pool.size();

			if (pool[index]->enabled)
				return pool[index];
		}

		return NULL;
	}

    void Update(float dt){
        for(auto it = pool.begin(); it<pool.end();it++)
                (*it)->Update(dt);
    }
    
	std::vector<T*> pool;
private:
};
