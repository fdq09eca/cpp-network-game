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

class Game;
class MyGameObject : NonCopyable {

public:
    MyGameObject() = default;
    virtual ~MyGameObject(){ printf("[dtor] MyGameObject\n"); }
};



class Player : MyGameObject {
    
    Point _position {0, 0};
    int size = 20;
    SDL_Color color {255, 0, 0, 255};

public:
    
    void setPosition(int x, int y) {
        _position.x = x;
        _position.y = y;
    }
    
    void onDraw(DrawRequest& req)  {
        
        SDL_Rect head {_position.x, _position.y, size, size};
        req.drawRect(head, color);
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
    
    
};

#endif /* Player_hpp */
