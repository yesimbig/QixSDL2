#include "Game.hpp"
#include "DEFINISIONS.hpp"


Game::Game()
{
    
}

Game::~Game()
{
    
}

void Game::Init(AvancezLib* system)
{
    SDL_Log("Game::Create");
    
    this->_system = system;
    
    // initialize physics system
    b2Vec2 gravity(0.0f, 0.0f);
    world = new b2World(gravity);
    
    //setup game objects
    _monster = new Monster();
    _field = new Field();
    _shuttle = new Shuttle();
    
    //setup monster
    _monster->Create(system, world, _monster, &game_objects, _field, &lines_pool, _shuttle, FIELD_LEFT_OFFSET + FIELD_WIDTH/2 - MONSTER_BODY_SIZE/2, FIELD_TOP_OFFSET + FIELD_HEIGHT/2 - MONSTER_BODY_SIZE/2, MONSTER_BODY_SIZE);
    _monster->AddReceiver(this);
    _monster->Init();
    game_objects.insert(_monster);
    
    //generate two sparx into the game
    sparx_pool.Create(SPARX_MAX_NUM);
    for(int i = 0;i < 2;i++){
        Sparx* sparx = sparx_pool.at(i);
        RenderComponent * sparxRender = new RenderComponent();
        sparxRender->Create(_system, sparx, &game_objects, SPARX_DIRPATH,SPARX_FRAMES,SPARX_FRAME_RATE);
        SparxBehaviourComponent * sparxBehaviour = new SparxBehaviourComponent();
        sparxBehaviour->Create(_system, sparx, &game_objects,_field, _shuttle, i&1);
        
        sparx->Create();
        sparx->AddComponent(sparxRender);
        sparx->AddComponent(sparxBehaviour);
        sparx->AddReceiver(this);
        sparx->Init(FIELD_LEFT_OFFSET + FIELD_WIDTH/2 - SPARX_SIZE/2, FIELD_TOP_OFFSET - SPARX_SIZE/2);
        game_objects.insert(sparx);
    }
    
    //setup field and walls
    walls.CreateEmpty();
    _field->Create(system,world, &walls,  &game_objects);
    _field->Init();
    _field->AddReceiver(this);
    
    //setup shuttle and lines
    lines_pool.CreateEmpty();
    chasing_lines_pool.CreateEmpty();
    
    ShuttleBehaviourComponent * shuttleBehaviour = new ShuttleBehaviourComponent();
    shuttleBehaviour->Create(_system, _shuttle, &game_objects,_field, &lines_pool, &chasing_lines_pool);
    
    RenderComponent * shuttleRender = new RenderComponent();
    shuttleRender->Create(_system, _shuttle, &game_objects, SHUTTLE_FILEPATH);

    _shuttle->Create();
    _shuttle->AddComponent(shuttleBehaviour);
    _shuttle->AddComponent(shuttleRender);
    //_shuttle->AddComponent(shuttle_coll);
    
    _shuttle->AddReceiver(this);
    _shuttle->Init(FIELD_LEFT_OFFSET + FIELD_WIDTH/2 - SHUTTLE_SIZE/2, FIELD_TOP_OFFSET + FIELD_HEIGHT - SHUTTLE_SIZE/2);
    game_objects.insert(_shuttle);
    
    //setup game state
    gameState = STATE_PLAYING;
    die_time = 0;
    life = 3;
    generate_time = 0;
    sparx_num = 2;
}

void Game::HandleEvents()
{
    SDL_Event event;
    SDL_PollEvent(&event);
    
    switch(event.type){
        case SDL_QUIT:
            break;
        
        default:
            break;
    }
    
}

void Game::Update(float dt)
{
    _field->Update();
    _field->Draw();
    
    //playing state
    if(gameState == STATE_PLAYING)
    {
        float32 timeStep = 1.0f / 60.0f;
        int32 velocityIterations = 1;
        int32 positionIterations = 1;
        world->Step(timeStep, velocityIterations, positionIterations);
     
        //generate two sparx into the game
        if(generate_time > SPARX_GENERATE_TIME && sparx_num < SPARX_MAX_NUM){
            generate_time = 0;
            for(int i = sparx_num; i< sparx_num + 2; i++)
            {
                Sparx* sparx = sparx_pool.at(i);
                RenderComponent * sparxRender = new RenderComponent();
                sparxRender->Create(_system, sparx, &game_objects, SPARX_DIRPATH,SPARX_FRAMES,SPARX_FRAME_RATE);
                SparxBehaviourComponent * sparxBehaviour = new SparxBehaviourComponent();
                sparxBehaviour->Create(_system, sparx, &game_objects,_field, _shuttle, i&1);
                
                sparx->Create();
                sparx->AddComponent(sparxRender);
                sparx->AddComponent(sparxBehaviour);
                sparx->AddReceiver(this);
                sparx->Init(FIELD_LEFT_OFFSET + FIELD_WIDTH/2 - SPARX_SIZE/2, FIELD_TOP_OFFSET - SPARX_SIZE/2);
                game_objects.insert(sparx);
            }
            sparx_num += 2;
        }
        generate_time += dt;
        
        //draw unfinished lines
        lines_pool.Update(dt);
        
        //draw chasing lines
        chasing_lines_pool.Update(dt);
        
        for (auto go = game_objects.begin(); go != game_objects.end(); go++)
            (*go)->Update(dt);
    }
    // die or win's time state
    else if(gameState == STATE_DIE || gameState == STATE_WIN_ANI)
    {
        die_time += dt;
        
        if(die_time >= DIE_ANI_TIME){
            _shuttle->resetHit();
            if(lines_pool.size()>0){
                _shuttle->Init(lines_pool.at(0)->firstX - SHUTTLE_SIZE/2 + LINE_WIDTH/2 ,lines_pool.at(0)->firstY - SHUTTLE_SIZE/2 + LINE_WIDTH/2);
                _monster->Init();
                
                lines_pool.Destroy();
                lines_pool.clear();
                chasing_lines_pool.Destroy();
                chasing_lines_pool.clear();
            }
            
            
            if(gameState == STATE_WIN_ANI){
                _field->Init();
                gameState = STATE_WIN;
            }
            else if(life > 0)
                gameState = STATE_PLAYING;
            else{
                _field->Init();
                gameState = STATE_GAME_OVER;
            }
            die_time = 0;
        }
        
        //draw unfinished lines
        lines_pool.Update(0);
        //draw chasing lines
        chasing_lines_pool.Update(0);
        for (auto go = game_objects.begin(); go != game_objects.end(); go++)
            (*go)->Update(0);
        
    }
    
    
}

void Game::Render()
{
    
    char msg[1024];
    
    if(gameState == STATE_GAME_OVER){
        _system->drawText(SCREEN_WIDTH/2 - 203, SCREEN_HEIGHT/2 - 100, "GAME OVER",2);
        _system->drawText(SCREEN_WIDTH/2 - 115, SCREEN_HEIGHT/2, "Final score");
        sprintf(msg, "%5d", _field->getScore());
        _system->drawText(SCREEN_WIDTH/2 - 60 , SCREEN_HEIGHT/2 + 50, msg,1.3);
        return;
    }
    else if(gameState == STATE_WIN){
        _system->drawText(SCREEN_WIDTH/2 - 143, SCREEN_HEIGHT/2 - 100, "YOU WIN!",2);
        _system->drawText(SCREEN_WIDTH/2 - 115, SCREEN_HEIGHT/2, "Final score");
        sprintf(msg, "%5d", _field->getScore());
        _system->drawText(SCREEN_WIDTH/2 - 60 , SCREEN_HEIGHT/2 + 50, msg,1.3);
        return;
    }
    
    _system->drawText(SCREEN_WIDTH/2 - 33, 0, "QIX");
    
    _system->drawText(SCREEN_WIDTH/2 - 238, 50, "Score",0.7);
    _system->drawText(SCREEN_WIDTH/2 - 58, 50, "Claimed",0.7);
    _system->drawText(SCREEN_WIDTH/2 + 153, 50, "Life",0.7);
    
    
    sprintf(msg, "%5d", _field->getScore());
    _system->drawText(SCREEN_WIDTH/2 - 238 , 80, msg,0.7);
    
    _system->drawText(SCREEN_WIDTH/2 + 18 , 80, "75%",0.7);
    sprintf(msg, "%2d%%", _field->getRate());
    _system->drawText(SCREEN_WIDTH/2 - 63 , 80, msg,0.7);
    
    sprintf(msg, "%d", life);
    _system->drawText(SCREEN_WIDTH/2 + 173 , 80, msg,0.7);
}

void Game::Receive(Message m)
{
    if (m == HIT && gameState != STATE_DIE){
        life--;
        SDL_Log("Player::Hit!");
        gameState = STATE_DIE;
    }
    if(m == WIN){
        SDL_Log("Player::Win!");
        gameState = STATE_WIN_ANI;
    }

}

void Game::Clean()
{
    SDL_Log("Game::Destroy");
    for (auto go = game_objects.begin(); go != game_objects.end(); go++)
        (*go)->Destroy();

    delete _shuttle;
    delete _field;
    
    lines_pool.Destroy();
    chasing_lines_pool.Destroy();
    walls.Destroy();
    delete world;
}

