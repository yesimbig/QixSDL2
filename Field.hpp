#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include <queue>

#include "avancezlib.h"
#include "DEFINISIONS.hpp"
#include "Lines.hpp"

class Field: public GameObject{
public:
    Field(AvancezLib* system): _system(system){}
    ~Field(){ delete[] _pixels; delete[] _grid; SDL_DestroyTexture(_texture); }
    
    virtual void Init();
    virtual void Draw();
    virtual void Update();
    
    //Uint32* getPixels(){return _pixels;};
    int* getGrid() {return _grid;}
    void setGrid(ObjectPool<Lines>* lines, bool slowing);
    int absCoor(int x, int y);
    int getRate(){ return (int)(_rate*100);}
    int getScore(){ return _score;}
    
    void setMonsterPos(float x, float y){monsterX = x; monsterY = y;}

    void setDirField(int x, int y, int* grid,int* dirField, bool turn);
    bool isEdge(int x, int y,int* grid);
    
    int* getDirGrid(){return _dirField;}
    int* getReDirGrid(){return _reDirField;}


private:
    AvancezLib* _system;
    Uint32 * _pixels;
    SDL_Texture * _texture;
    int* _grid;
    bool visit[FIELD_WIDTH*FIELD_HEIGHT];
    float _rate;
    int _score;
    
    int _dirField[FIELD_WIDTH * FIELD_HEIGHT];
    int _reDirField[FIELD_WIDTH * FIELD_HEIGHT];
    
    float monsterX, monsterY;
    
    struct vLines{
        float x,y1,y2;
        vLines(float a,float b = 0, float c = 0): x(a),y1(b),y2(c) {}
        bool operator < (const vLines& v) const
        {
            return (x < v.x);
        }
    };
    
    struct Point{
        int x,y;
        Point(int a,int b): x(a),y(b) {}
    };
    
};
