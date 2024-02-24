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
    static Game* _instance; // singleton
    
    std::vector<std::unique_ptr<Player>> players_newAdded;
    std::vector<Player*> players_pendingRemove;

public:
    Game();
    
    ~Game();
    
    MySocket listenSock;
    
    Player* localPlayer = nullptr;
    
    bool isHost();
    
    std::vector<std::unique_ptr<Player>> players;
    
    static const int SCREEN_W = 500;
    static const int SCREEN_H = 500;
    static const int SCREEN_CENTER_X = SCREEN_W / 2;
    static const int SCREEN_CENTER_Y = SCREEN_H / 2;
    Uint32 startTime = 0;
    Uint32 endTime = 0;
    Uint32 delta = 0;
    short fps = 60;
    short timePerFrame = 150; // miliseconds
    
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_bool quit = SDL_FALSE;

    static Game* Instance() { return _instance; }
    
    Player* addPlayer(std::unique_ptr<Player>&& np);
    
    void removePlayer(Player* p);
    
    void onInit();
    
    void onUpdate();

    void onUpdateNetwork();
    
    void boardcast(Player& srcPlayer, std::vector<uint8_t> msg);
    
    void onDraw();
    
    int getNewPlayerId();
    
    Player* getPlayerById(int id);
    
    void run();
    
    void startHost();
    
    void startClient();
    
    void onEvent(SDL_Event& event);
};

#endif /* Game_hpp */
