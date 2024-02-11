//
//  Player.cpp
//  sdl2
//
//  Created by ChrisLam on 31/01/2024.
//

#include "Player.hpp"
#include "Game.hpp"
#include "MySerialiser.hpp"



void Player::sendPos(){
    fmt::print("sendPos()!");
//    sendBuff.clear();
//    MySerialiser::se_Int(0, sendBuff); // PackSetSize
//    MySerialiser::se_Int(static_cast<int>(MyCommand::UPDATE_POS), sendBuff);
//    MySerialiser::se_Int(id, sendBuff);
//    MySerialiser::se_Int(pos.x, sendBuff);
//    MySerialiser::se_Int(pos.y, sendBuff);
//    *reinterpret_cast<int*>(sendBuff.data()) = (int) sendBuff.size();
}

void Player::onRecvCommand() {
    uint8_t* p = recvBuff.data();
    uint8_t* e = recvBuff.data() + recvBuff.size();
    
//    int packetSize;
//    p = MyDeserialiser::de_Int(packetSize, p, e);
    int iCmd;
    p = MyDeserialiser::de_Int(iCmd, p, e);
    
    MyCommand cmd = static_cast<MyCommand>(iCmd);
    
    switch (cmd) {
        case MyCommand::INIT_PLAYER: {
            Player tmp;
            MyDeserialiser::de_Player(tmp, p, e);
            fmt::print("id: {}, pos: ({}, {}), color: ({}, {}, {}, {})",
                       tmp.id, tmp.pos.x, tmp.pos.y,
                       tmp.color.r,
                       tmp.color.g,
                       tmp.color.b,
                       tmp.color.a
                       );
            *this = std::move(tmp);
            
            

        } break;
        
        case MyCommand::UPDATE_PLAYER:{
            Player tmp;
            // local player is host?
            MyDeserialiser::de_Player(tmp, p, e);
            Player* q = Game::Instance()->getPlayerById(tmp.id);
            
            
            if (!q) {
                auto p = std::make_unique<Player>();
                *p = std::move(tmp);
                Game::Instance()->players.emplace_back(std::move(p));
            } else {
                
                if (q->id == id) return;
                
                *q = std::move(tmp);
            }
        }
            
            
        default:
            throw MyError("UNKNOWN CMD number");
            
    }
}


void Player::onConnect(Game& g){
    
    
    
    // set ID
    auto newId = g.getNewPlayerId();
    id = newId;
    
    // set color
    color = {0, 255, 0};
    
    // set pos;
    pos.x = 5;
    pos.y = 5;
    
//     send connected player hello packet
    std::vector<uint8_t> tmp;
    Packet::make_PlayerPacket(*this, MyCommand::INIT_PLAYER, tmp);
    Player q;
    
    int size = -1;
    int iCmd = -1;
    
    uint8_t* src = tmp.data();
    uint8_t* dst = src + tmp.size();
    src = MyDeserialiser::de_Int(size, src, dst);
    src = MyDeserialiser::de_Int(iCmd, src, dst);
    src = MyDeserialiser::de_Player(q, src, dst);
    
    MyCommand cmd = static_cast<MyCommand>(iCmd);
    TEST(cmd == MyCommand::INIT_PLAYER);
    
    TEST(color.r == q.color.r);
    TEST(color.g == q.color.g);
    TEST(color.b == q.color.b);
    TEST(color.a == q.color.a);
    
    TEST(pos.x == q.pos.x);
    TEST(pos.y == q.pos.y);
    
    TEST(src == dst);
    TEST(tmp.size() == 40);
    
    
//    auto& players = g.Instance()->players;
    
    // send all players state to new connected player
//    for (auto& p: players) {
//        if (p.get() == this)
//            continue;
//        Packet::make_PlayerPacket(*p, MyCommand::UPDATE_PLAYER, tmp);
//    }
    
    send(tmp);
}



void Player::move(int dx, int dy) {
    
    pos.x += dx * size;
    pos.y += dy * size;
    
    
    if (pos.y < 0)                    pos.y = Game::SCREEN_H;
    if (pos.y > Game::SCREEN_H)       pos.y = 0;
    
    if (pos.x < 0)                    pos.x = Game::SCREEN_W;
    if (pos.x> Game::SCREEN_W)        pos.x = 0;
    
    if (dx || dy) onMove();
}

