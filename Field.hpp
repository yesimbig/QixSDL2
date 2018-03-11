#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include <queue>

#include "avancezlib.h"
#include "DEFINISIONS.hpp"
#include "Lines.hpp"
#include "box.hpp"

class Field: public GameObject{
public:
    Field(){}
    ~Field(){ delete[] _pixels; delete[] _grid; SDL_DestroyTexture(_texture); }
    
    virtual void Create(AvancezLib* system, b2World * _world, ObjectPool<Box>* walls, std::set<GameObject *> *game_obj);
    virtual void Init();
    virtual void Draw();
    virtual void Update();
    
    //Uint32* getPixels(){return _pixels;};
    int* getGrid() {return _grid;}
    void setGrid(ObjectPool<Lines>* lines, bool slowing); //color the occupy area by the lines
    int absCoor(int x, int y); //transform the field coordinate into real coordinate
    Box* generateWall(int posX, int posY, int width, int height);
    
    int getRate(){ return (int)(_rate*100);}
    int getScore(){ return _score;}
    void setMonsterPos(float x, float y){monsterX = x; monsterY = y;}

    void setDirField(int x, int y, int* grid,int* dirField, bool turn); //set the direction map that sparx go
    bool isEdge(int x, int y,int* grid); //define the point is edge point or not
    
    int* getDirGrid(){return _dirField;}
    int* getReDirGrid(){return _reDirField;}


private:
    AvancezLib* _system;
    std::set<GameObject*> * game_objects;
    Uint32 * _pixels;   //color pixel
    SDL_Texture * _texture; //the texture on the field
    ObjectPool<Box>* _walls;
    b2World * _world;    //Box2D world
    
    int* _grid;     //grid map
    bool visit[FIELD_WIDTH*FIELD_HEIGHT];   //record the point is visited or not, used in BFS and dirField
    float _rate;    //the occupied ratio
    int _score;     //score of the game
    
    int _dirField[FIELD_WIDTH * FIELD_HEIGHT];  //sparx would move according to this direction
    int _reDirField[FIELD_WIDTH * FIELD_HEIGHT]; //reverse direction
    
    float monsterX, monsterY;   //record monster's position
    
    struct Point{
        int x,y;
        Point(int a,int b): x(a),y(b) {}
    };
    
};
