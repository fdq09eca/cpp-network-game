//
//  MySerialiser.cpp
//  sdl2
//
//  Created by ChrisLam on 01/02/2024.
//

#include "MySerialiser.hpp"
#include "Player.hpp"
#include "Game.hpp"

void MySerialiser::se_Int(int v, std::vector<uint8_t>& buff) {
    auto  n = buff.size();
    auto sv = sizeof(v);
    buff.resize(n + sv);
    auto p = (int*) &buff[n]; // write at the last one
    *p = v;
}

uint8_t* MyDeserialiser::de_Int(int& v, uint8_t* p, uint8_t* e) {
    assert(p + sizeof(v) <= e);
    v = *((int*) p);
    p += sizeof(int);
    return p;
}



void MySerialiser::se_Color(SDL_Color& c, std::vector<uint8_t>& buff){
    se_Int(c.r, buff);
    se_Int(c.g, buff);
    se_Int(c.b, buff);
    se_Int(c.a, buff);
}

uint8_t* MyDeserialiser::de_Color(SDL_Color& c, uint8_t* p, uint8_t* e) {
    int r, g, b, a;
    p = de_Int(r, p, e);
    p = de_Int(g, p, e);
    p = de_Int(b, p, e);
    p = de_Int(a, p, e);
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return p;
}

void MySerialiser::se_Position(Point& pos, std::vector<uint8_t>& buff){
    se_Int(pos.x, buff);
    se_Int(pos.y, buff);
}

uint8_t* MyDeserialiser::de_Position(Point& pos, uint8_t* p, uint8_t* e) {
    p = de_Int(pos.x, p, e);
    p = de_Int(pos.y, p, e);
    return p;
}

void MySerialiser::se_Player(Player& p, std::vector<uint8_t>& buff) {
    MySerialiser::se_Int(p.id, buff);
    MySerialiser::se_Position(p.pos, buff);
    MySerialiser::se_Int(p.size, buff);
    MySerialiser::se_Color(p.color, buff);
}


uint8_t* MyDeserialiser::de_Player(Player& o, uint8_t* p, uint8_t* e) {
    
    p = MyDeserialiser::de_Int(o.id,  p, e);
    p = MyDeserialiser::de_Position(o.pos, p, e);
    p = MyDeserialiser::de_Int(o.size, p, e);
    p = MyDeserialiser::de_Color(o.color, p, e);
    return p;
}

void Packet::make_PlayerPacket(Player &p, MyCommand cmd, std::vector<uint8_t> &buff) {
       MySerialiser::se_Int(0, buff);
       MySerialiser::se_Int((int) cmd, buff);
       MySerialiser::se_Player(p, buff);
       *reinterpret_cast<int*>(buff.data()) = static_cast<int>(buff.size());
}


