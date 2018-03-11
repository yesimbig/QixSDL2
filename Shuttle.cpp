#include "Shuttle.hpp"
#include "DEFINISIONS.hpp"
#include <iostream>
#include <SDL2/SDL.h>

void ShuttleBehaviourComponent::Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, Field * field, ObjectPool<Lines>* lines_pool, ObjectPool<Lines>* chasing_lines_pool){
    Component::Create(system, go, game_objects);
    _field = field;
    preState = SHUTTLE_STOP;
    nowState = SHUTTLE_STOP;
    
    lines = lines_pool;
    chasingLines = chasing_lines_pool;

}

void ShuttleBehaviourComponent::Init()
{
    
    go->width = SHUTTLE_SIZE;
    go->height = SHUTTLE_SIZE;
    
    preState = SHUTTLE_STOP;
    nowState = SHUTTLE_STOP;
    shuttleState = SHUTTLE_STATE_MOVING_EDGE;
    slowing = false;
    lineChasing = false;
    chasingDelay = 0;
    moveStreak = 0;
    stopDelay = 0;
}

void ShuttleBehaviourComponent::Update(float dt)
{
    if(dt == 0)return;
    
    AvancezLib::KeyStatus keys;
    system->getKeyStatus(keys);
    
    //record the previous location
    float preX = go->horizontalPosition;
    float preY = go->verticalPosition;
    
    //set the current key moving and the shuttle state
    bool moving = defineKeyPress(keys, dt);
    shuttleState = determineShuttleState(go->horizontalPosition, go->verticalPosition);


    //Create a new line seg when shuttle changes its direction or starts to draw
    if( (preState!=nowState && moving && shuttleState == SHUTTLE_STATE_MOVING_MIDDLE) ||
       (shuttleState == SHUTTLE_STATE_MOVING_MIDDLE && lines->size() == 0))
    {
        if(!slowing)
            lines->push_back(generateLine(preX + SHUTTLE_SIZE/2, preY + SHUTTLE_SIZE/2, LINE_WIDTH, LINE_WIDTH, 0, 119, 200));// blue color
        else
            lines->push_back(generateLine(preX + SHUTTLE_SIZE/2, preY + SHUTTLE_SIZE/2, LINE_WIDTH, LINE_WIDTH, 160, 35, 0));// red color
    }
    //set the length of the last line seg while drawing
    else if(preState == nowState && lines->size() > 0 && moving && shuttleState == SHUTTLE_STATE_MOVING_MIDDLE)
    {
        lines->at(lines->size()-1)->set(lines->at(lines->size()-1)->firstX, lines->at(lines->size()-1)->firstY,
                go->horizontalPosition+SHUTTLE_SIZE/2-LINE_WIDTH/2, go->verticalPosition+SHUTTLE_SIZE/2-LINE_WIDTH/2);

    }
    //when the lines are finished
    else if(shuttleState == SHUTTLE_STATE_MOVING_EDGE && lines->size() > 0)
    {
        int nowFieldPosH = (int)go->horizontalPosition + SHUTTLE_SIZE/2 - FIELD_LEFT_OFFSET;
        int nowFieldPosV = (int)go->verticalPosition + SHUTTLE_SIZE/2 - FIELD_TOP_OFFSET;
        //remove the bias of the last line so that it can correctly connect to the walls
        removeBiasOfLastSeg(nowFieldPosH, nowFieldPosV, (int)preX + SHUTTLE_SIZE/2 - FIELD_LEFT_OFFSET, (int)preY  + SHUTTLE_SIZE/2 - FIELD_TOP_OFFSET, nowState);
        
        //fill up the grid and color the field
        _field->setGrid(lines,slowing);
        
        removeBiasOfShuttlesPos(nowFieldPosH, nowFieldPosV);

        lines->Destroy();
        lines->clear();
        chasingLines->Destroy();
        chasingLines->clear();
        
        slowing = false;
    }
    
    //set chasing line
    if( shuttleState == SHUTTLE_STATE_MOVING_MIDDLE){
        if(!moving && chasingDelay < LINE_CHASING_DELAY) //calculate chasing delay time
        {
            chasingDelay += dt;
        }
        else if(!moving) //no moving, start to chase
        {
            setChasingLines(chasingLines->size(), lines->size(), dt);
        }
        else if(moving)
        {
            chasingDelay = 0;
        }
        
    }

    preState = nowState;
}

bool ShuttleBehaviourComponent::defineKeyPress(AvancezLib::KeyStatus keys, float dt)
{
    //set slow only when shuttle is on the edge
    if(shuttleState == SHUTTLE_STATE_MOVING_EDGE){
        slowing = keys.slow;
    }
    
    if ( keys.right && (nowState != SHUTTLE_MOVE_LEFT || shuttleState != SHUTTLE_STATE_MOVING_MIDDLE)){
        go->rotationAngle = 90;
        if(Move(dt * SHUTTLE_MOVING_SPEED, 0, slowing)){
            nowState = SHUTTLE_MOVE_RIGHT;
            moveStreak += dt;
            stopDelay = 0;
            return true;
        }
    }
    else if (keys.left && (nowState != SHUTTLE_MOVE_RIGHT || shuttleState != SHUTTLE_STATE_MOVING_MIDDLE)){
        go->rotationAngle = 90;
        if(Move(-dt * SHUTTLE_MOVING_SPEED, 0, slowing)){
            nowState = SHUTTLE_MOVE_LEFT;
            moveStreak += dt;
            stopDelay = 0;
            return true;
        }
    }
    else if (keys.down && (nowState != SHUTTLE_MOVE_UP || shuttleState != SHUTTLE_STATE_MOVING_MIDDLE)){
        go->rotationAngle = 0;
        if(Move(0, dt * SHUTTLE_MOVING_SPEED, slowing)){
            nowState = SHUTTLE_MOVE_DOWN;
            moveStreak += dt;
            stopDelay = 0;
            return true;
        }
    }
    else if (keys.up && (nowState != SHUTTLE_MOVE_DOWN || shuttleState != SHUTTLE_STATE_MOVING_MIDDLE)){
        go->rotationAngle = 0;
        if(Move(0, -dt * SHUTTLE_MOVING_SPEED, slowing)){
            nowState = SHUTTLE_MOVE_UP;
            moveStreak += dt;
            stopDelay = 0;
            return true;
        }
    }
    
    //reset moveStreak when player release the moving buttons
    if((nowState == SHUTTLE_MOVE_UP && !keys.up) || (nowState == SHUTTLE_MOVE_DOWN && !keys.down) ||
       (nowState == SHUTTLE_MOVE_LEFT && !keys.left) || (nowState == SHUTTLE_MOVE_RIGHT && !keys.right))
        moveStreak = 0;
    
    //calculate the stopDelay time
    stopDelay += dt;
    
    return false;
}

bool ShuttleBehaviourComponent::Move(float moveH, float moveV, bool slow)
{
    float slow_rate = 1.0;
    if(slow){
        slow_rate = SHUTTLE_SLOW_MOVING_RATE;
    }
        
    float afterMoveH =  go->horizontalPosition + moveH * slow_rate;
    float afterMoveV = go->verticalPosition + moveV * slow_rate;
    
    //border detection
    if(afterMoveH < FIELD_LEFT_OFFSET - SHUTTLE_SIZE/2){
        go->horizontalPosition = FIELD_LEFT_OFFSET - SHUTTLE_SIZE/2;
        return false;
    }
    if(afterMoveH > FIELD_LEFT_OFFSET + FIELD_WIDTH - SHUTTLE_SIZE/2){
        go->horizontalPosition = FIELD_LEFT_OFFSET + FIELD_WIDTH - SHUTTLE_SIZE/2;
        return false;
    }
    if(afterMoveV < FIELD_TOP_OFFSET - SHUTTLE_SIZE/2){
        go->verticalPosition = FIELD_TOP_OFFSET - SHUTTLE_SIZE/2;
        return false;
    }
    if(afterMoveV > FIELD_TOP_OFFSET + FIELD_HEIGHT - SHUTTLE_SIZE/2){
        go->verticalPosition = FIELD_TOP_OFFSET + FIELD_HEIGHT - SHUTTLE_SIZE/2;
        return false;
    }
    
    //collision detection
    for (auto i = 0; i < lines->size()-1; i++)
    {
        GameObject* go0 = lines->at(i);
        if(go0->width == LINE_WIDTH) //verticle line
        {
            float y1 = go0->verticalPosition;
            float y2 = go0->verticalPosition + go0->height;
            float lside = go0->horizontalPosition - LINE_WIDTH - 2;
            float rside = go0->horizontalPosition + go0->width + LINE_WIDTH + 1;
            
            if(moveH < 0 && (afterMoveV + go->height/2 - y1) * (afterMoveV + go->height/2 - y2) < 0 &&
               afterMoveH + go->width/2 < rside && go->horizontalPosition + go->width/2 >= rside){
                go->horizontalPosition = rside - go->width/2;
                return false;
            }
            else if(moveH > 0 && (afterMoveV + go->height/2 - y1) * (afterMoveV + go->height/2 - y2) < 0 &&
               afterMoveH + go->width/2 > lside && go->horizontalPosition + go->width/2  <= lside){
                go->horizontalPosition = lside - go->width/2;
                return false;
            }
            
        }
        else if(go0->height == LINE_WIDTH) //horizontal line
        {
            float x1 = go0->horizontalPosition;
            float x2 = go0->horizontalPosition + go0->width;
            float uside = go0->verticalPosition - LINE_WIDTH - 2;
            float dside = go0->verticalPosition + go0->height + LINE_WIDTH + 1;
            
            if(moveV < 0 && (afterMoveH + go->width/2 - x1) * (afterMoveH + go->width/2 - x2) < 0 &&
               afterMoveV + go->height/2 < dside && go->verticalPosition + go->height/2 >= dside){
                go->verticalPosition = dside - go->height/2;
                return false;
            }
            else if(moveV > 0 && (afterMoveH + go->width/2 - x1) * (afterMoveH + go->width/2 - x2) < 0 &&
                    afterMoveV + go->height/2 > uside && go->verticalPosition + go->height/2  <= uside){
                go->verticalPosition = uside - go->height/2;
                return false;
            }
        }
    }
    
    //collision detection to occupied grid
    if(determineGridState((int)afterMoveH + SHUTTLE_SIZE/2 - FIELD_LEFT_OFFSET,(int)afterMoveV + SHUTTLE_SIZE/2 - FIELD_TOP_OFFSET, _field->getGrid()) == FIELD_STATE_OCCUPIED)
    {
         return false;
    }
    
    //set up a temperary stop while the shuttle is about to move out the edge
    if( determineShuttleState(go->horizontalPosition, go->verticalPosition) == SHUTTLE_STATE_MOVING_EDGE &&
       determineShuttleState(afterMoveH,afterMoveV) == SHUTTLE_STATE_MOVING_MIDDLE && moveStreak>0.10 && stopDelay < 0.15)
    {
        return false;
    }
    
    //collision all passed
    go->horizontalPosition = afterMoveH;
    go->verticalPosition = afterMoveV;
    
    
    return true;
}

Lines* ShuttleBehaviourComponent::generateLine(float x, float y, float w, float h, int r, int g, int b, int a)
{
    Lines* line = new Lines();
    
    LinesBehaviourComponent* lineBehaviour = new LinesBehaviourComponent();
    lineBehaviour->Create(system, line, game_objects);
    lineBehaviour->Set(x, y, w, h);
    
    RenderComponent * lineRender = new RenderComponent();
    lineRender->CreateSingleColor(system, line, game_objects, r, g, b, a);
    
    
    line->Init();
    line->AddComponent(lineBehaviour);
    line->AddComponent(lineRender);
    
    line->firstX = x - LINE_WIDTH/2;
    line->firstY = y - LINE_WIDTH/2;
    line->controller = lineBehaviour;
    return line;
}

int ShuttleBehaviourComponent::determineShuttleState(float posH, float posV)
{

    //determine border
    if(posH == FIELD_LEFT_OFFSET - SHUTTLE_SIZE/2 ||
       posH == FIELD_LEFT_OFFSET + FIELD_WIDTH - SHUTTLE_SIZE/2 ||
       posV == FIELD_TOP_OFFSET - SHUTTLE_SIZE/2 ||
       posV == FIELD_TOP_OFFSET + FIELD_HEIGHT - SHUTTLE_SIZE/2){
        return SHUTTLE_STATE_MOVING_EDGE;
    }
    
    //determine finished line
    int absPosH = (int)posH + go->width/2 - FIELD_LEFT_OFFSET;
    int absPosV = (int)posV + go->height/2 - FIELD_TOP_OFFSET;
    
    if( _field->getGrid()[absPosV * FIELD_WIDTH + absPosH] != FIELD_STATE_EMPTY ) return SHUTTLE_STATE_MOVING_EDGE;
    
    return SHUTTLE_STATE_MOVING_MIDDLE;
}

void ShuttleBehaviourComponent::removeBiasOfLastSeg(int x, int y, int preX, int preY, int nowState)
{
    Lines *lastLine = lines->at(lines->size()-1);
    float firstX = lastLine->firstX;
    float firstY = lastLine->firstY;
    
    //go left line
    if(nowState == SHUTTLE_MOVE_LEFT)
    {
        for(int i = x; i <= preX; i++){
            if( i >=0 && determineGridState(i, y, _field->getGrid())==FIELD_STATE_EMPTY ){
                lastLine->set( firstX, firstY, i-LINE_WIDTH/2 + FIELD_LEFT_OFFSET, go->verticalPosition+SHUTTLE_SIZE/2-LINE_WIDTH/2);
                break;
            }
        }
    }
    //go right line
    else if(nowState == SHUTTLE_MOVE_RIGHT)
    {
        for(int i = x; i >= preX; i--){
            if( i < FIELD_WIDTH && determineGridState(i, y, _field->getGrid())==FIELD_STATE_EMPTY ){
                lastLine->set( firstX, firstY, i-LINE_WIDTH/2 + FIELD_LEFT_OFFSET, go->verticalPosition+SHUTTLE_SIZE/2-LINE_WIDTH/2);
                break;
            }
        }
    }
     //go up line
    else if(nowState == SHUTTLE_MOVE_UP)
    {
        for(int i = y; i <= preY; i++){
            if( i >=0 && determineGridState(x, i, _field->getGrid())==FIELD_STATE_EMPTY){
                lastLine->set(firstX, firstY, go->horizontalPosition+SHUTTLE_SIZE/2-LINE_WIDTH/2, i + FIELD_TOP_OFFSET -LINE_WIDTH/2);
                break;
            }
        }
    }
    //go down line
    else if(nowState == SHUTTLE_MOVE_DOWN)
    {
        for(int i = y; i >= preY; i--){
            if( i < FIELD_HEIGHT && determineGridState(x, i, _field->getGrid())==FIELD_STATE_EMPTY){
                lastLine->set(firstX, firstY, go->horizontalPosition+SHUTTLE_SIZE/2-LINE_WIDTH/2, (float)i + FIELD_TOP_OFFSET -LINE_WIDTH/2);
                break;
            }
        }
    }
}

void ShuttleBehaviourComponent::removeBiasOfShuttlesPos(int x, int y)
{
    for(int i = 0;i <= LINE_WIDTH; i++){
        if(determineGridState(x+i ,y , _field->getGrid()) != FIELD_STATE_OCCUPIED) {
            go->horizontalPosition += i;return;
        }
        if(determineGridState(x-i ,y , _field->getGrid()) != FIELD_STATE_OCCUPIED) {
            go->horizontalPosition -= i;return;
        }
        if(determineGridState(x ,y+i , _field->getGrid()) != FIELD_STATE_OCCUPIED) {
            go->verticalPosition += i;return;
        }
        if(determineGridState(x ,y-i , _field->getGrid()) != FIELD_STATE_OCCUPIED) {
            go->verticalPosition -= i;return;
        }
    }
}

int ShuttleBehaviourComponent::determineGridState(int i,int j,int* _grid)
{
    if(j<0) j = 0;
    if(i<0) i = 0;
    if (j>=FIELD_HEIGHT) j = FIELD_HEIGHT-1;
    if (i>=FIELD_WIDTH) i = FIELD_WIDTH-1;
    
    return _grid[j * FIELD_WIDTH + i];
}

void ShuttleBehaviourComponent::setChasingLines(int chasingSize, int lineSize, float dt)
{
    if(chasingSize == 0)
    {
        chasingLines->push_back(generateLine(lines->at(0)->firstX, lines->at(0)->firstY, LINE_WIDTH, LINE_WIDTH, 80, 30, 80));
        growingLength = 0;
    }
    else if( chasingSize <= lineSize){
        float dm = dt * LINE_CHASING_SPEED;
        growingLength += dm;
        Lines* nowAt = chasingLines->at(chasingSize-1), *refAt = lines->at(chasingSize-1);
        if(refAt->height > LINE_WIDTH)
        {
            if(growingLength >= refAt->height){
                nowAt->set( refAt->horizontalPosition, refAt->verticalPosition, refAt->horizontalPosition, refAt->verticalPosition + refAt->height - LINE_WIDTH );
                if(chasingSize < lineSize){
                    chasingLines->push_back(generateLine(lines->at(chasingSize)->firstX, lines->at(chasingSize)->firstY, LINE_WIDTH, LINE_WIDTH, 80, 30, 80));
                    growingLength = 0;
                }
                else{
                    go->Send(HIT);
                }
            }
            else{
                //move up
                if( lines->at(chasingSize-1)->firstY != lines->at(chasingSize-1)->verticalPosition )
                {
                    nowAt->set( refAt->firstX, refAt->firstY, refAt->firstX,refAt->firstY - (int)growingLength );
                }
                else //move down
                {
                    nowAt->set( refAt->firstX, refAt->firstY, refAt->firstX,refAt->firstY + (int)growingLength);
                }
            }
            
        }
        else
        {
            if(growingLength >= refAt->width){
                nowAt->set( refAt->horizontalPosition, refAt->verticalPosition, refAt->horizontalPosition + refAt->width - LINE_WIDTH, refAt->verticalPosition);
                if(chasingSize < lineSize){
                    chasingLines->push_back(generateLine(lines->at(chasingSize)->firstX, lines->at(chasingSize)->firstY, LINE_WIDTH, LINE_WIDTH, 80, 30, 80));
                    growingLength = 0;
                }
                else{
                    go->Send(HIT);
                }
            }
            else{
                //move left
                if( lines->at(chasingSize-1)->firstX != lines->at(chasingSize-1)->horizontalPosition )
                {
                    nowAt->set( refAt->firstX, refAt->firstY, refAt->firstX - (int)growingLength ,refAt->firstY );
                }
                else //move right
                {
                    nowAt->set( refAt->firstX, refAt->firstY, refAt->firstX + (int)growingLength ,refAt->firstY );
                }
            }
        }
    }
}

void Shuttle::Init(float h, float v)
{
    this->horizontalPosition = h;
    this->verticalPosition = v;
    
    SDL_Log("Shuttle::Init");
    GameObject::Init();

}

void Shuttle::Receive(Message m)
{

}

void Shuttle::Update(float dt)
{
    if(dieDelay<SHUTTLE_DIE_DELAY)
        dieDelay += dt;
    else
        canHit = true;
    
    GameObject::Update(dt);
}

void Shuttle::resetHit()
{
    dieDelay = 0;
    canHit = false;
}
