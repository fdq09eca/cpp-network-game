//
//  Player.cpp
//  sdl2
//
//  Created by ChrisLam on 31/01/2024.
//

#include "Player.hpp"
#include "Game.hpp"
#include "MySerialiser.hpp"

Player::Player(Player&& rhs) {
    *this = std::move(rhs);
}

SDL_Color Player::possible_colors[] = {
    SDL_GREEN  ,
    SDL_BLUE   ,
    SDL_ORANGE ,
    SDL_YELLOW ,
    SDL_SKYBLUE,
    SDL_PURPLE ,
    SDL_RED    ,
};

bool Player::canSend() { return !sendBuff.empty(); }

bool Player::canRead() { return !canSend(); }

void Player::setColorById() {
    if (id < 0 || isHost ) { color = SDL_WHITE; }
    color = possible_colors[id % 7];
}

void Player::operator=(Player&& rhs){
    size = rhs.size;
    color = rhs.color;
    id = rhs.id;
    pos = rhs.pos;
    sock = std::move(rhs.sock);
    remainPacketSize = rhs.remainPacketSize;
    sendBuff = rhs.sendBuff;
    recvBuff = rhs.recvBuff;
    
    rhs.sendBuff.clear();
    rhs.recvBuff.clear();
}

void Player::sendPos(){
    fmt::print("===sendPos()=====\n");
    std::vector<uint8_t> tmp;
    Packet::make_PlayerPacket(*this, MyCommand::UPDATE_PLAYER_POS, tmp);
    if (isHost) {
        Game::Instance()->boardcast(*this, tmp);
    } else {
        send(tmp);
    }

}

void Player::onRecvCommand() {
    uint8_t* p = recvBuff.data();
    uint8_t* e = recvBuff.data() + recvBuff.size();

    int iCmd;
    p = MyDeserialiser::de_Int(iCmd, p, e);
    
    MyCommand cmd = static_cast<MyCommand>(iCmd);
    
    switch (cmd) {
        case MyCommand::INIT_PLAYER: {
            
            Player tmp;
            tmp.sock = std::move(sock);
            if (!tmp.sock.isValid()) {
                MY_ASSERT(false);
            }
            MyDeserialiser::de_Player(tmp, p, e);
            *this = std::move(tmp);
        } break;
        
        case MyCommand::UPDATE_PLAYER:{
            
            Player tmp;
            MyDeserialiser::de_Player(tmp, p, e);
            Player* q = Game::Instance()->getPlayerById(tmp.id);
            
            if (!q) {
                auto np = std::make_unique<Player>();
                *np = std::move(tmp);
                q = Game::Instance()->addPlayer(std::move(np));
            } else {

                size = tmp.size;
                color = tmp.color;
                pos = tmp.pos;
                isHost = tmp.isHost;
                
            }
            
            MY_ASSERT(q != nullptr);
            std::vector<uint8_t> msg;
            Packet::make_PlayerPacket(*q, MyCommand::UPDATE_PLAYER, msg);
            Game::Instance()->boardcast(*q, msg);
            
        
        } break;
            
        case MyCommand::UPDATE_PLAYER_POS:{
            Player tmp;
            MyDeserialiser::de_Player(tmp, p, e);
            Player* q = Game::Instance()->getPlayerById(tmp.id);
            
            if (q == nullptr) {
                auto np = std::make_unique<Player>();
                *np = std::move(tmp);
                q = Game::Instance()->addPlayer(std::move(np));
            } else {
                (*q).pos = tmp.pos;
            }
            
            MY_ASSERT(q != nullptr);
            
            std::vector<uint8_t> msg;
            Packet::make_PlayerPacket(*q, MyCommand::UPDATE_PLAYER_POS, msg);
            Game::Instance()->boardcast(*q, msg);
        } break;
            
        case MyCommand::REMOVE_PLAYER:{
//            rp_id =
//            Player* rp = Game::Instance()->getPlayerById(rp_id);
//            Game::Instance()->removePlayer(rp);
            MY_ASSERT("\nNOT YET IMPLEMENTED.\n");
        } break;
            
            
        default:
            throw MyError("UNKNOWN CMD number");
            
    }
}


void Player::onConnect(Game& g){
    // set ID
    auto newId = g.getNewPlayerId();
    id = newId;
    setColorById();
    
    // set pos;
    pos.x = 5 * size;
    pos.y = 5 * size;
    
    // send connected player hello packet
    std::vector<uint8_t> tmp;
    Packet::make_PlayerPacket(*this, MyCommand::INIT_PLAYER, tmp);
    send(tmp); // [80 CMD DATA ]
    
    auto& players = g.Instance()->players;
    
    // send all players to newly connected player
    for (auto& p: players) {
        if (p->id == id)
            continue;
        std::vector<uint8_t> tmp2;
        tmp2.clear();
        Packet::make_PlayerPacket(*p, MyCommand::UPDATE_PLAYER, tmp2);
        send(tmp2);
        fmt::print("!! send player {}", fmt::ptr(p.get()));
    }
    
        
}



void Player::move(int dx, int dy) {
    
    pos.x += dx * size;
    pos.y += dy * size;
    
    
    if (pos.y < 0)                    pos.y = Game::SCREEN_H;
    if (pos.y > Game::SCREEN_H)       pos.y = 0;
    
    if (pos.x < 0)                    pos.x = Game::SCREEN_W;
    if (pos.x > Game::SCREEN_W)        pos.x = 0;
    
    if (dx || dy) onMove();
}

void Player::onUpdate(){
    int dx = 0;
    int dy = 0;
    
    const Uint8 *keystate = SDL_GetKeyboardState(nullptr);
    
    if (keystate) {
        if ( keystate[SDL_SCANCODE_LEFT] )  dx -= 1;
        if ( keystate[SDL_SCANCODE_RIGHT] ) dx += 1;
        if ( keystate[SDL_SCANCODE_UP] )    dy -= 1;
        if ( keystate[SDL_SCANCODE_DOWN] )  dy += 1;
    }
    
    move(dx, dy);
}

void Player::onSend() {
    // TODO: redo.
    // send with offset
    // should do more.
    
    size_t n = sock.send(sendBuff.data(), sendBuff.size());
    fmt::print("\n{} byteSent.\n", n);
    sendBuff.clear();
}

void Player::onRecv() {
    
    size_t n = sock.nByteToRead();
    if (n == 0) { // disconnected.
        fmt::print("\n >> disconnected!!\n");
        Game::Instance()->removePlayer(this);
        sock.close();
    }
    
    assert(remainPacketSize >= 0);
    
    if (remainPacketSize == 0)
    {
        auto headerSize = sizeof(uint32_t);
        if (n < headerSize) return; // make sure there are at least 4 byte
        
        std::vector<uint8_t> tmp;
        sock.recv(tmp, headerSize);
        remainPacketSize = *reinterpret_cast<int*>(tmp.data());
        remainPacketSize -= headerSize;
        recvBuff.resize(remainPacketSize);
    }
    else
    {
        MY_ASSERT(remainPacketSize > 0);
        auto r = sock.recv(recvBuff.data() + recvOffset, remainPacketSize);
        remainPacketSize -= r;
        recvOffset       += r;
        
        if (remainPacketSize <= 0) {
            onRecvCommand();
            
            recvOffset = 0;
            remainPacketSize = 0;
            recvBuff.clear();
        }
    }
    
}

void Player::send(std::vector<uint8_t>& buff){
    MyUtil::appendVector(sendBuff, buff);
}

void Player::onDraw(DrawRequest &req){
    SDL_Rect head {pos.x, pos.y, size, size};
    req.drawRect(head, color);
    
    if (isHost) {
        // indicate which window is host.
        SDL_Rect r {100, 0, size, size};
        req.drawRect(r, SDL_WHITE);
    }
}


void Player::setPosition(int x, int y) {
    pos.x = x;
    pos.y = y;
}


void Player::onMove() { sendPos();  }


