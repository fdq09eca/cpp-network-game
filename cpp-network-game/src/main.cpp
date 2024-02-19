//
//  main.cpp
//  snake
//
//  Created by ChrisLam on 23/02/2021.
//

#include <iostream>

#include "Game.hpp"
#include "Player.hpp"
#include "MyTest.hpp"

int main(int argc, const char * argv[]) {
    
    Game game;
    if (argc >= 2 && strcmp(argv[1], "--host") == 0) {
        game.startHost();
    }
    else
    {
        game.startClient();
    }
    
    game.onInit();
    
    game.run();
    
    
    
    
    return 0;

}
