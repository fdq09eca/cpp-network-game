//
//  Game.hpp
//  sdl2
//
//  Created by ChrisLam on 31/01/2024.
//

#ifndef Game_hpp
#define Game_hpp

#include <stdio.h>
#include "myCommon.hpp"

class Player;

class Game {
    
public:

    
    Player* localPlayer = nullptr;
    
    std::vector<std::unique_ptr<Player>> players; // <==
    
    static const int SCREEN_W = 1280;
    static const int SCREEN_H = 720;
    static const int SCREEN_CENTER_X = SCREEN_W/2;
    static const int SCREEN_CENTER_Y = SCREEN_H/2;
    Uint32 startTime = 0;
    Uint32 endTime = 0;
    Uint32 delta = 0;
    short fps = 60;
    short timePerFrame = 150; // miliseconds
    
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_bool quit = SDL_FALSE;
    
    void setLocalPlayer(std::unique_ptr<Player>&& p);
    
    void onInit() {
        if (SDL_Init(SDL_INIT_VIDEO) != 0
          ||SDL_CreateWindowAndRenderer(SCREEN_W, SCREEN_H, 0, &window, &renderer) != 0)
        {
             throw MyError("Game init failed.");
        }
        assert(window && renderer);
    }
    
    
    
    

    void onUpdate();

    
    void onDraw();
    
    
    
    
    
    void run();
    void startHost();
    void startClient();
    
    void onEvent(SDL_Event& event);
};

#endif /* Game_hpp */
