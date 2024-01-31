//
//  Player.cpp
//  sdl2
//
//  Created by ChrisLam on 31/01/2024.
//

#include "Player.hpp"
#include "Game.hpp"



void Player::move(int dx, int dy) {
    
    _position.x += dx * size;
    _position.y += dy * size;
    
    if (_position.y < 0)                    _position.y = Game::SCREEN_H;
    if (_position.y > Game::SCREEN_H)       _position.y = 0;
    
    if (_position.x < 0)                    _position.x = Game::SCREEN_W;
    if (_position.x> Game::SCREEN_W)        _position.x = 0;
    
}
