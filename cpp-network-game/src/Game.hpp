//
//  Game.hpp
//  sdl2
//
//  Created by ChrisLam on 31/01/2024.
//

#ifndef Game_hpp
#define Game_hpp

#include <stdio.h>
#include "MyCommon.hpp"
#include "MySocket.hpp"
class Player;

class Game {
private:
    static Game* _instance;
    
public:
    Game() {
        assert(_instance == nullptr);
        _instance = this;
    }
    
    ~Game() {
        _instance = nullptr;
    }
    
    MySocket listenSock;
    
    
    
    Player* localPlayer = nullptr;
    
    std::vector<std::unique_ptr<Player>> players;
    
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
    
    
    static Game* Instance() { return _instance; }
    
    void onInit();
    
    void onUpdate();

    void onUpdateNetwork();
    
    void boardcast(Player& p);
    
    void onDraw();
    
    int getNewPlayerId();
    
    Player* getPlayerById(int id); // not sure about raw ptr..?
    
    void run();
    
    void startHost();
    
    void startClient();
    
    void onEvent(SDL_Event& event);
};

#endif /* Game_hpp */
