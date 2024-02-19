//
//  MyCommon.hpp
//  sdl2
//
//  Created by ChrisLam on 30/01/2024.
//

#ifndef MyCommon_hpp
#define MyCommon_hpp

#include <SDL2/SDL.h>
#include "MyUtil.hpp"


struct Point {
    
    int x = 0; int y = 0;
    Point() = default;
    Point(int x_, int y_): x(x_), y(y_){};
    
    inline bool operator== (const Point& p) const { return p.x == x && p.y == y; }
};

struct MyRect {
    Point pos;
    size_t size = 0;
    
    MyRect() = default;
    MyRect(int x_, int y_, size_t s_): pos(x_, y_), size(s_){ };
    
    
    inline bool operator== (const MyRect& r) const { return pos == r.pos && r.size == size; }
};

struct DrawRequest : public NonCopyable {
    SDL_Renderer* renderer = nullptr;
    
    void setColor(const SDL_Color& color) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    }
    
    void drawRect(const SDL_Rect& rect, const SDL_Color& color){
        setColor(color);
        SDL_RenderFillRect(renderer, &rect);
    }
    

};

#endif /* MyCommon_hpp */
