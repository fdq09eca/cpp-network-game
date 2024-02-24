//
//  MySerialiser.hpp
//  sdl2
//
//  Created by ChrisLam on 01/02/2024.
//

#ifndef MySerialiser_hpp
#define MySerialiser_hpp

#include <stdio.h>
#include "MyCommon.hpp"

class Player;
enum class MyCommand;
class Game;

class MySerialiser {
public:
    static void se_Int(int v, std::vector<uint8_t>& buff);
    
    static void se_Position(Point& pos, std::vector<uint8_t>& buff);
    
    static void se_Color(SDL_Color& c, std::vector<uint8_t>& buff);
    
    static void se_Player(Player& p, std::vector<uint8_t>& buff);
    
    static void se_Game(Game& g, std::vector<uint8_t>& buff);
};


class MyDeserialiser {
public:
    static uint8_t* de_Int(int& v, uint8_t* p, uint8_t* e);
    
    static uint8_t* de_Position(Point& pos, uint8_t* p, uint8_t* e);
    
    static uint8_t* de_Color(SDL_Color& c, uint8_t* p, uint8_t* e);
    
    static uint8_t* de_Player(Player& o, uint8_t* p, uint8_t* e);
    
    static uint8_t* de_Game(Game& o, uint8_t* p, uint8_t* e);
    
};

class Packet {
    
public:
    
    static void make_PlayerPacket(Player &p, MyCommand cmd, std::vector<uint8_t> &buff);
    static void make_PosPacket(Point& pos, MyCommand cmd, std::vector<uint8_t> &buff);
    

};



#endif /* MySerialiser_hpp */
