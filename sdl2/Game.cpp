//
//  Game.cpp
//  sdl2
//
//  Created by ChrisLam on 31/01/2024.
//

#include "Game.hpp"
#include "Player.hpp"

void Game::run(){
    
    
    while (!quit) {
        if (!startTime) {
            // get the time in ms passed from the moment the program started
            startTime = SDL_GetTicks();
        } else {
            delta = endTime - startTime; // how many ms for a frame
        }
        
        
        
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            onEvent(event);
        }
        
        onUpdate();
        onDraw();
        
        // if less than 16ms, delay
        if (delta < timePerFrame) {
            SDL_Delay(timePerFrame - delta); // sleep
        }
        
        // if delta is bigger than 16ms between frames, get the actual fps
        if (delta > timePerFrame) {
            fps = 1000 / delta;
        }
        
        //        printf("FPS is: %i \n", fps);
        
        startTime = endTime;
        endTime = SDL_GetTicks();
    }
    
    // game.onQuit();
    
    if (renderer)   { SDL_DestroyRenderer(renderer);    }
    if (window)     {   SDL_DestroyWindow(window);      }
    SDL_Quit();
}

void Game::startHost(){
    // init host
//    game.onInit();
    auto& p = players.emplace_back(new Player);
    localPlayer = p.get();
}

void Game::startClient(){
    auto& p = players.emplace_back(new Player);
    localPlayer = p.get();
}

void Game::onUpdate(){
    if (localPlayer) {
        localPlayer->onUpdate();
    }
}

void Game::onEvent(SDL_Event& event){
    
    switch (event.type) {
        case SDL_KEYDOWN:
            switch( event.key.keysym.sym ){
                case SDLK_ESCAPE:   quit = SDL_TRUE;            break;
            }
            printf( "Key press detected\n" );                   break;

        case SDL_KEYUP: printf( "Key release detected\n" );     break;
        case SDL_QUIT: quit = SDL_TRUE;                         break;
    }
}

void Game::onDraw(){
    DrawRequest req;
    req.renderer = renderer;
    req.setColor({0, 0, 0, 255});
    SDL_RenderClear(renderer); // <-- clean backBuffer
    
    for (const auto& p : players) {
        p->onDraw(req);
    }
    
    SDL_RenderPresent(renderer); // <--backbuffer to screen
}
