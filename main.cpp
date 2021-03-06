#include <iostream>
#include "avancezlib.h"

#include "Shuttle.hpp"
#include "Field.hpp"
#include "Lines.hpp"
#include "Monster.hpp"

#include "Game.hpp"
#include "DEFINISIONS.hpp"

#include "Box2D/Box2D.h"

int main(int argc, const char * argv[]) {
    AvancezLib system;
    system.init(SCREEN_WIDTH, SCREEN_HEIGHT);
    Game game;
    game.Create(&system);
    game.Init();
    
    float lastTime = system.getElapsedTime();
    while (system.update())
    {
        float newTime = system.getElapsedTime();
        float dt = newTime - lastTime;
        dt = dt * GAME_SPEED;
        lastTime = newTime;
        
        game.Update(dt);
        
        game.Render();
    }
    
    // clean up
    game.Destroy();
    system.destroy();

    
    return 0;
}
