//
//  Player.hpp
//  sdl2
//
//  Created by ChrisLam on 31/01/2024.
//

#ifndef Player_hpp
#define Player_hpp

#include <stdio.h>
#include "MyCommon.hpp"
#include "MySocket.hpp"
#include "MySerialiser.hpp"

#define SDL_WHITE   SDL_Color{255, 255, 255, 255}
#define SDL_RED     SDL_Color{255, 0  ,   0, 255}
#define SDL_ORANGE  SDL_Color{255, 128,   0, 255}
#define SDL_YELLOW  SDL_Color{255, 255,   0, 255}
#define SDL_GREEN   SDL_Color{  0, 255,   0, 255}
#define SDL_SKYBLUE SDL_Color{  0, 255, 255, 255}
#define SDL_BLUE    SDL_Color{  0,   0, 255, 255}
#define SDL_PURPLE  SDL_Color{128,   0, 255, 255}

class Game;

enum class MyCommand {
    INIT_PLAYER         = 0,
    UPDATE_PLAYER       = 1,
    UPDATE_PLAYER_POS   = 2,
    REMOVE_PLAYER       = 3,
};

class Player : NonCopyable {
    
    static SDL_Color possible_colors[];

public:
    Point pos {0, 0};
    SDL_Color color {255, 0, 0, 255};
    
    int size = 20;
    int id = -1;
    bool isHost = false;
    
    MySocket sock;
    std::vector<uint8_t> recvBuff;
    int remainPacketSize = 0;
    int recvOffset = 0;
    std::vector<uint8_t> sendBuff;
    std::vector<uint8_t> tmpBuff;
    
    Player() = default;
    Player(Player&& rhs);
    
    void operator=(Player&& rhs);
    
    void setPosition(int x, int y);
    
    void onDraw(DrawRequest& req);
    
    void setColorById();
    
    void onConnect(Game& g);

    void sendPos();
    
    bool canSend();
    
    bool canRead();
    
    void send(std::vector<uint8_t>& buff);
    
    void onSend();
    
    void onRecvCommand();
    
    void onRecv();

    void onUpdate();

    void move(int dx, int dy);
    
    void onMove();
    
    
};

#endif /* Player_hpp */
