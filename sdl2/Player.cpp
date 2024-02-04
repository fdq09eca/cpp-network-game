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
    sendBuff.clear();
    MySerialiser::se_Int(0, sendBuff); // PackSetSize
    MySerialiser::se_Int(static_cast<int>(MyCommand::UPDATE_POS), sendBuff);
    MySerialiser::se_Int(id, sendBuff);
    MySerialiser::se_Int(pos.x, sendBuff);
    MySerialiser::se_Int(pos.y, sendBuff);
    *reinterpret_cast<int*>(sendBuff.data()) = (int) sendBuff.size();
}

void Player::onRecvCommand() {
    uint8_t* p = recvBuff.data();
    uint8_t* e = recvBuff.data() + recvBuff.size();
    
    int packetSize;
    p = MyDeserialiser::de_Int(packetSize, p, e);
    int iCmd;
    p = MyDeserialiser::de_Int(iCmd, p, e);
    
    MyCommand cmd = static_cast<MyCommand>(iCmd);
    switch (cmd) {
        case MyCommand::INIT_PLAYER: {
            MY_ASSERT(isHost == false);
            
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
    
    
    // send connected player hello packet
    Packet::make_PlayerPacket(*this, MyCommand::INIT_PLAYER, sendBuff);
    
    
    auto& players = g.Instance()->players;
    
    // send all players state to new connected player
    for (auto& p: players) {
        if (p.get() == this)
            continue;
        Packet::make_PlayerPacket(*p, MyCommand::UPDATE_PLAYER, sendBuff);
    }
    
}

void Player::toBuffer(std::vector<uint8_t>& buff){
    MySerialiser::se_Player(*this, buff);
}

uint8_t* Player::fromBuffer(uint8_t* src, uint8_t* end){
    return MyDeserialiser::de_Player(*this, src, end);
}

void Player::move(int dx, int dy) {
    
    pos.x += dx * size;
    pos.y += dy * size;
    
    if (pos.y < 0)                    pos.y = Game::SCREEN_H;
    if (pos.y > Game::SCREEN_H)       pos.y = 0;
    
    if (pos.x < 0)                    pos.x = Game::SCREEN_W;
    if (pos.x> Game::SCREEN_W)        pos.x = 0;
    
    onMove();
}

