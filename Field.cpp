#include "Field.hpp"

void Field::Create(AvancezLib* system, b2World* world, ObjectPool<Box>* walls, std::set<GameObject *> *game_obj)
{
    _system = system;
    _world = world;
    _walls = walls;
    _pixels = new Uint32[(FIELD_WIDTH + 2*FIELD_THICK) * (FIELD_HEIGHT + 2*FIELD_THICK) ];
    game_objects = game_obj;
}

void Field::Init()
{
    
    memset(_pixels, 0, (FIELD_WIDTH + 2*FIELD_THICK) * (FIELD_HEIGHT + 2*FIELD_THICK) * sizeof(Uint32));

    //set horizontal line of the field
    for(int j = 0;j < FIELD_THICK; j++)
        for(int i=0;i<FIELD_WIDTH + 2 * FIELD_THICK; i++){
            _pixels[ i + j * (FIELD_WIDTH + 2 * FIELD_THICK)] = 0xffffff;
            _pixels[ i + (FIELD_HEIGHT + 2 * FIELD_THICK - j - 1) * (FIELD_WIDTH + 2 * FIELD_THICK)] = 0xffffff;
        }
    
    //set vertical line of the field
    for(int j = 0;j < FIELD_THICK; j++)
        for(int i = 0; i < FIELD_HEIGHT + 2 * FIELD_THICK; i++){
            _pixels[ j + i * (FIELD_WIDTH + 2 * FIELD_THICK)] = 0xffffff;
            _pixels[ FIELD_WIDTH + 2 * FIELD_THICK - j - 1 + i * (FIELD_WIDTH + 2 * FIELD_THICK)] = 0xffffff;
        }
    
    //set walls
    _walls->Destroy();
    _walls->clear();
    
    //top wall
    _walls->push_back(generateWall(FIELD_LEFT_OFFSET - FIELD_THICK,FIELD_TOP_OFFSET - FIELD_THICK, FIELD_WIDTH + 2* FIELD_THICK, FIELD_THICK));
    //down wall
    _walls->push_back(generateWall(FIELD_LEFT_OFFSET - FIELD_THICK,FIELD_TOP_OFFSET + FIELD_HEIGHT, FIELD_WIDTH + 2* FIELD_THICK, FIELD_THICK));
    
    //left wall
    _walls->push_back(generateWall(FIELD_LEFT_OFFSET - FIELD_THICK,FIELD_TOP_OFFSET - FIELD_THICK, FIELD_THICK, FIELD_HEIGHT + 2*FIELD_THICK));
    
    //right wall
    _walls->push_back(generateWall(FIELD_LEFT_OFFSET + FIELD_WIDTH,FIELD_TOP_OFFSET - FIELD_THICK, FIELD_THICK, FIELD_HEIGHT + 2*FIELD_THICK));
    
    
    _texture = SDL_CreateTexture(_system->getRenderer(),
                    SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, FIELD_WIDTH + 2*FIELD_THICK, FIELD_HEIGHT + 2*FIELD_THICK);
    
    _grid = new int[FIELD_WIDTH * FIELD_HEIGHT];
    
    memset(_grid,FIELD_STATE_EMPTY,sizeof(int) * FIELD_WIDTH * FIELD_HEIGHT);
    
    memset(_dirField, 0, sizeof(_dirField));
    memset(_reDirField, 0, sizeof(_reDirField));
    
    setDirField(FIELD_WIDTH/2, FIELD_HEIGHT/2, _grid, _dirField, false);
    setDirField(FIELD_WIDTH/2, FIELD_HEIGHT/2, _grid, _reDirField, true);
    _rate = 0;
}

void Field::setGrid(ObjectPool<Lines>* lines, bool slowing)
{
    //fill in the lines' position
    for (auto i = 0; i < lines->size(); i++)
    {
        GameObject* go0 = lines->at(i);
        
        for(int j = std::max(0 , (int)go0->horizontalPosition - FIELD_LEFT_OFFSET); j <= (int)go0->horizontalPosition + go0->width - FIELD_LEFT_OFFSET; j++ )
            for(int k = std::max(0 , (int)go0->verticalPosition - FIELD_TOP_OFFSET); k<= (int)go0->verticalPosition + go0->height - FIELD_TOP_OFFSET; k++){
                    _grid[k * FIELD_WIDTH + j] = FIELD_STATE_EDGE;
                    _pixels[absCoor(j,k)] = 0xFFFFFF;
                }
        //set box2D wall
        _walls->push_back(generateWall((int)go0->horizontalPosition,(int)go0->verticalPosition, (int)go0->width + 1, (int) go0->height + 1));
    }
    
    //start to set grid map
    int mY = (int)monsterY - FIELD_TOP_OFFSET;
    int mX = (int)monsterX - FIELD_LEFT_OFFSET;
    
    //BFS
    memset(visit,false,sizeof(visit));
    std::queue<Point> Q;
    Q.push(Point(mY, mX));
    visit[mX * FIELD_WIDTH + mY] = true;
    
    while(!Q.empty()){
        Point p = Q.front();
        Q.pop();
        
        if( !visit[ (p.x + 1) * FIELD_WIDTH + p.y] && p.x < FIELD_HEIGHT-1 && _grid[ (p.x + 1) *FIELD_WIDTH + p.y] == FIELD_STATE_EMPTY){
            visit[(p.x + 1)  * FIELD_WIDTH + p.y] = true;
            Q.push(Point(p.x+1,p.y));
        }
        
        if( !visit[ (p.x - 1) * FIELD_WIDTH + p.y] && p.x > 0 && _grid[ (p.x - 1) *FIELD_WIDTH + p.y] == FIELD_STATE_EMPTY){
            visit[(p.x - 1)  * FIELD_WIDTH + p.y] = true;
            Q.push(Point(p.x-1,p.y));
        }
        
        if( !visit[ p.x * FIELD_WIDTH + p.y+1] && p.y < FIELD_WIDTH-1 && _grid[ p.x *FIELD_WIDTH + p.y + 1] == FIELD_STATE_EMPTY){
            visit[p.x * FIELD_WIDTH + p.y+1] = true;
            Q.push(Point(p.x,p.y+1));
        }
        if( !visit[ p.x * FIELD_WIDTH + p.y-1] && p.y > 0 && _grid[ p.x *FIELD_WIDTH + p.y - 1] == FIELD_STATE_EMPTY){
            visit[p.x * FIELD_WIDTH + p.y-1] = true;
            Q.push(Point(p.x,p.y-1));
        }
    }
    
    int fillSum = 0;
    //fill in
    for(int i = 0;i<FIELD_HEIGHT;i++){
        for(int j = 0;j<FIELD_WIDTH;j++){
            if(!visit[i* FIELD_WIDTH + j] && _grid[i* FIELD_WIDTH + j] == FIELD_STATE_EMPTY) {
                if(slowing) //red
                    _pixels[absCoor(j,i)] = 0xA02300;
                else //blue
                    _pixels[absCoor(j,i)] = 0x0077C8;
                _grid[i* FIELD_WIDTH + j] = FIELD_STATE_OCCUPIED;
                fillSum++;
                //fill the block
                for(int k = -2; k<= 2; k++)
                    for(int l = -2;l <= 2; l++)
                        if( i+k >= 0 && i+k < FIELD_HEIGHT && j+l >=0 && j+l < FIELD_WIDTH && _grid[(i+k)* FIELD_WIDTH + (j+l)] == FIELD_STATE_EDGE ){
                            _grid[(i+k)* FIELD_WIDTH + (j+l)] = FIELD_STATE_OCCUPIED;
                            fillSum++;
                            
                        }
            }
        }
    }
    
    setDirField(mX, mY, _grid, _dirField, false);
    setDirField(mX, mY, _grid, _reDirField, true);
    
    _rate += (float)fillSum / (float)(FIELD_WIDTH * FIELD_HEIGHT);
    _score += (int)(10000 * (float)fillSum / (float)(FIELD_WIDTH * FIELD_HEIGHT)) * (1 + (int)slowing);
    
    if(getRate() >= 75)
    {
        _score += (getRate()-75) * 1000;
        if(getRate()>= 99)_score += 25000;
        Send(WIN);
    }
}

void Field::setDirField(int x, int y, int *grid, int *dirField, bool turn)//turn means reverse
{
    //find the rightest line of the field
    while( x>0 && !isEdge(x,y,grid)) x--;
   /*
    for(int i = 0;i<FIELD_WIDTH;i++)
        for(int j = 0;j<FIELD_HEIGHT;j++)
            if(_pixels[absCoor(i,j)] == 0xFFFF00 || _pixels[absCoor(i,j)] == 0x0000FF ||
               _pixels[absCoor(i,j)] == 0xFF00FF || _pixels[absCoor(i,j)] == 0x00FF00)
                _pixels[absCoor(i,j)] = 0xFFFFFF;
    */
    memset(visit,false,sizeof(visit));
    
    if(turn) dirField[y*FIELD_WIDTH + x] = FIELD_DOWN;
    else dirField[y*FIELD_WIDTH + x] = FIELD_UP;
    
    visit[y*FIELD_WIDTH + x] = true;
    
    int nowX = x, nowY = y;
    
    if(turn)nowY++;
    else nowY--;
    
    int r = 0;
    while(nowX != x || nowY != y){
       // std::cout<<nowX<<" "<<nowY<<std::endl;
        visit[nowY*FIELD_WIDTH + nowX] = true;
        
        //go top
        if(nowY>0 && !visit[(nowY-1)*FIELD_WIDTH + nowX] && isEdge(nowX,nowY-1,grid)){
            dirField[nowY*FIELD_WIDTH + nowX] = FIELD_UP;
            //_pixels[absCoor(nowX,nowY)] = 0xFFFF00;
            nowY--;
        }//go down
        else if(nowY<FIELD_HEIGHT && !visit[(nowY+1)*FIELD_WIDTH + nowX] && isEdge(nowX,nowY+1,grid)){
            dirField[nowY*FIELD_WIDTH + nowX] = FIELD_DOWN;
            //_pixels[absCoor(nowX,nowY)] = 0x0000FF;
            nowY++;
        }//go left
        else if(nowX>0 && !visit[nowY*FIELD_WIDTH + nowX-1] && isEdge(nowX-1,nowY,grid)){
            dirField[nowY*FIELD_WIDTH + nowX] = FIELD_LEFT;
            //_pixels[absCoor(nowX,nowY)] = 0xFF00FF;
            nowX--;
        }//go right
        else if(nowX<FIELD_WIDTH && !visit[nowY*FIELD_WIDTH + nowX+1] && isEdge(nowX+1,nowY,grid)){
            dirField[nowY*FIELD_WIDTH + nowX] = FIELD_RIGHT;
            //_pixels[absCoor(nowX,nowY)] = 0x00FF00;
            nowX++;
        }
        else{
            //std::cout<<"GG\n";
            break;
        }
        
        //make start point into unvisit
        r++;
        if(r == 2)visit[y*FIELD_WIDTH + x] = false;
    }
    
    //SDL_Log("done");
    
}

bool Field::isEdge(int x, int y, int *grid)
{
    if(x<0 || x>=FIELD_WIDTH)return false;
    if(y<0 || y>=FIELD_HEIGHT)return false;
    
    if(grid[y*FIELD_WIDTH + x] == FIELD_STATE_OCCUPIED)return false;
    
    for(int i = -1;i<=1;i++)
        for(int j = -1; j<=1 ;j++)
            if(y+i<0 || y+i==FIELD_HEIGHT || x+j<0 || x+j==FIELD_WIDTH ||  grid[(y+i)*FIELD_WIDTH + x+j] == FIELD_STATE_OCCUPIED)
                return true;
    return false;
}


int Field::absCoor(int x, int y)
{
    return  FIELD_THICK + x + (y + FIELD_THICK) * (FIELD_WIDTH + 2 * FIELD_THICK);
}

Box* Field::generateWall(int posX, int posY, int width, int height){
    Box* static_box = new Box();
    //RenderComponent * static_box_render = new RenderComponent();
    //static_box_render->CreateSingleColor(_system, static_box, game_objects, 255,0,0,255);
    
    BoxPhysicsComponent * static_box_physics = new BoxPhysicsComponent();
    static_box_physics->Create(_system, _world, static_box, game_objects, b2_staticBody, posX, posY, width, height);
    
    static_box->Create();
    //static_box->AddComponent(static_box_render);
    static_box->AddComponent(static_box_physics);
    static_box->AddReceiver(this);
    static_box->Init();
    game_objects->insert(static_box);
    
    
    return static_box;
}

void Field::Draw()
{
    SDL_Rect rect;
    
    rect.x = FIELD_LEFT_OFFSET - FIELD_THICK;
    rect.y = FIELD_TOP_OFFSET - FIELD_THICK;
    
    SDL_QueryTexture(_texture, NULL, NULL, &(rect.w), &(rect.h));
    SDL_Point center;
    center.x = (int)(rect.w / 2.f);
    center.y = (int)(rect.h / 2.f);
    
    //Render texture to screen
    //    SDL_RenderCopy(renderer, texture, NULL, &rect);
    
    SDL_RenderCopyEx(_system->getRenderer(),
                     _texture,
                     NULL,
                     &rect,
                     NULL,
                     &center,
                     SDL_FLIP_NONE);
}



void Field::Update()
{
    SDL_UpdateTexture(_texture, NULL, _pixels, (FIELD_WIDTH + 2*FIELD_THICK) * sizeof(Uint32));
}


