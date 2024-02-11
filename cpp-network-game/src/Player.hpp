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

class Game;

enum class MyCommand : int {
    INIT_PLAYER = 0,
    UPDATE_PLAYER = 1,
    UPDATE_ID = 2,
    UPDATE_POS = 3,
    UPDATE_COLOR = 4,
    
};

class MyGameObject : NonCopyable {

public:
    MyGameObject() = default;
    virtual ~MyGameObject(){ printf("[dtor] MyGameObject\n"); }
};



class Player : MyGameObject {


public:
    Player()  {
        sendBuff.clear();
        recvBuff.clear();
    }
    
    Player(Player&& rhs) { *this = std::move(rhs); }
    
    void toBuffer(std::vector<uint8_t>& buff);
    
    uint8_t* fromBuffer(uint8_t* src, uint8_t* end);
    
    void operator=(Player&& rhs){
        size = rhs.size;
        color = rhs.color;
        id = rhs.id;
        pos = rhs.pos;
        sock = std::move(rhs.sock);
        remainSize = rhs.remainSize;
        sendBuff = rhs.sendBuff;
        recvBuff = rhs.recvBuff;
        
        rhs.sendBuff.clear();
        rhs.recvBuff.clear();
    }
    
    Point pos {0, 0};
    SDL_Color color {255, 0, 0, 255};
    
    int size = 20;
    int id = -1;
    bool isHost = false;
    
    MySocket sock;
    int remainSize = 0;
    
    std::vector<uint8_t> recvBuff;
    std::vector<uint8_t> sendBuff;
    std::vector<uint8_t> tmpBuff;
    
    void setPosition(int x, int y) {
        pos.x = x;
        pos.y = y;
    }
    
    void onDraw(DrawRequest& req)  {
        
        SDL_Rect head {pos.x, pos.y, size, size};
        req.drawRect(head, color);
    }
    
    void onConnect(Game& g);
    
    
    
    void sendPos();
    
    bool canSend() { return !sendBuff.empty(); }
    bool canRead() { return !canSend(); }
    
    void send(std::vector<uint8_t>& buff){
        MyUtil::appendVector(sendBuff, buff);
    }
    
    void onSend(){
        // should do more.
        size_t n = sock.send(sendBuff.data(), sendBuff.size());
        
        
        fmt::print("\n{} byteSent.", n);
        sendBuff.clear();
    }
    
    void onRecvCommand();
    
    void onRecv() {
        
        auto n = sock.nByteToRead();
        if (n == 0) { // disconnected.
            sock.close();
        }
        
        assert(remainSize >= 0);
        
        if (remainSize == 0)
        {
            auto headerSize = sizeof(uint32_t);
            if (n < headerSize) return; // make sure there are at least 4 byte
            
            std::vector<uint8_t> tmp;
            sock.recv(tmp, headerSize);
            remainSize = *reinterpret_cast<int*>(tmp.data());
            remainSize -= headerSize;
            recvBuff.resize(remainSize);
        }
        else
        {   // auto n = sock.recv(recvBuff.data() + recvOffset, remainSize);
            // or recvAppend?
            auto n = sock.recv(recvBuff.data(), remainSize);
            remainSize -= n;
            if (remainSize <= 0) {
                onRecvCommand();
                remainSize = 0;
                recvBuff.clear();
            }
        }
        
    }
    
    
    void onUpdate() {
        int moveX = 0;
        int moveY = 0;
        
        const Uint8 *keystate = SDL_GetKeyboardState(nullptr);
        
        if (keystate) {
            if ( keystate[SDL_SCANCODE_LEFT] )  moveX -= 1;
            if ( keystate[SDL_SCANCODE_RIGHT] ) moveX += 1;
            if ( keystate[SDL_SCANCODE_UP] )    moveY -= 1;
            if ( keystate[SDL_SCANCODE_DOWN] )  moveY += 1;
        }
        
        move(moveX, moveY);
    }

    void move(int dx, int dy);
    
    inline void onMove() { sendPos();  }
};

#endif /* Player_hpp */
