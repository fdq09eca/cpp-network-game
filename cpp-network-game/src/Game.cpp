//
//  Game.cpp
//  sdl2
//
//  Created by ChrisLam on 31/01/2024.
//

#include "Game.hpp"
#include "Player.hpp"

bool Game::isHost() {
    return localPlayer->isHost;
}

Game* Game::_instance = nullptr;

Game::Game() {
    assert(_instance == nullptr);
    _instance = this;
}

Game::~Game() { _instance = nullptr; }

void Game::onInit(){
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0
        ||SDL_CreateWindowAndRenderer(SCREEN_W, SCREEN_H, 0, &window, &renderer) != 0)
    {
        throw MyError("Game init failed.");
    }
    assert(window && renderer);
    
}

int Game::getNewPlayerId() {
#if 1
    int max_id = players.front()->id;    
    for (const auto& p : players)
        if (p->id > max_id)
            max_id = p->id;
    
    return max_id + 1;
#else
    // try std::max, lambda,
    
    // auto comp =  [](const std::unique_ptr<Player>& p, const std::unique_ptr<Player>& q) { return p->id < q->id; }; // old way
    auto comp =  [](const auto& p, const auto& q) { return p->id < q->id; }; // C++14 generic lambda
    
    // comp returns true if the first argument is **less** than the second.
    
    auto it = std::max_element(players.begin(), players.end(), comp);
    int max_id = (*it)->id;
    return max_id + 1;
    
    
    
#endif

}

void Game::boardcast(Player& srcPlayer, std::vector<uint8_t> msg) {
    if (!isHost()) return;
    fmt::print("\n>> boardcast!!\n");
    for (auto& p : players) {
        if (!p->sock.isValid()) continue;
        if (p.get() == &srcPlayer) continue;
        p->send(msg);
    }
}

Player* Game::addPlayer(std::unique_ptr<Player>&& np) {
    auto& b = players_newAdded.emplace_back(std::move(np));
    return b.get();
}

void Game::removePlayer(Player* p){
    players_pendingRemove.emplace_back(p);
    if (!isHost()) return;
    std::vector<uint8_t> msg;
    Packet::make_PlayerPacket(*p, MyCommand::REMOVE_PLAYER, msg);
    boardcast(*p, msg);
}

Player* Game::getPlayerById(int id) {
    auto pred = [&id](const auto& p) { return p->id == id; };
    auto it = std::find_if(players.begin(), players.end(), pred);
    if (it == players.end()) return nullptr;
    return (*it).get();
}

void Game::onUpdateNetwork(){
    MyFdSet fdSet;
    if (listenSock.isValid()) {
        fdSet.addRead(listenSock);
    }
    
    // add and remove players
    for (auto& np : players_newAdded) {
        players.emplace_back(std::move(np));
    }
    
    for (auto* rp : players_pendingRemove) {
        players.erase(std::remove_if(players.begin(),
                                    players.end(),
                                     [rp](const auto& q) { return q.get() == rp; }),
                     players.end());
    }
    
    
    players_newAdded.clear();
    players_pendingRemove.clear();
    
    
    for (auto& p : players) {
        if (!p)
            continue;
        if (!p->sock.isValid())
            continue;
        
        
        if (p->canSend()) {
            fdSet.addWrite(p->sock);
        }
        
        if (p->canRead()) {
            fdSet.addRead(p->sock);
        }
    }
    
    
    int n = fdSet.select();
    if (n <= 0) return;
    
    if (fdSet.hasRead(listenSock)) {
        auto p = std::make_unique<Player>();
        listenSock.accept(p->sock);
        auto& b = players.emplace_back(std::move(p));
        
        b->onConnect(*this);
        
        std::vector<uint8_t> msg;
        Packet::make_PlayerPacket(*b, MyCommand::UPDATE_PLAYER, msg);
        boardcast(*b, msg);
    }
    
    for (auto& p : players) {
        if (!p) continue;
        if (!p->sock.isValid()) continue;
        
        if (fdSet.hasRead(p->sock)) {
            p->onRecv();
        }
        
        if (fdSet.hasWrite(p->sock)) {
            fmt::print("\nplayer {} onSend.\n", fmt::ptr(p.get()));
            fmt::print("p->sendBuff.size() = {}\n", p->sendBuff.size());
            
            for (auto& c : p->sendBuff) {
                printf("%c", c);
            }
            
            p->onSend();
        }
    }
    
}

void Game::run(){
    
    
    while (!quit) {
        if (!startTime) {
            // get the time in ms passed from the moment the program started
            startTime = SDL_GetTicks();
        } else {
            delta = endTime - startTime; // how many ms for a frame
        }
        
        
        // host;
        onUpdateNetwork();
    
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            onEvent(event);
        }
        
        onUpdate();
        onDraw();
        
        // if less than 16ms, delay
        if (delta < timePerFrame) {
            SDL_Delay(timePerFrame - delta); // sleep
        }
        
        // if delta is bigger than 16ms between frames, get the actual fps
        if (delta > timePerFrame) {
            fps = 1000 / delta;
        }
        
        //        printf("FPS is: %i \n", fps);
        
        startTime = endTime;
        endTime = SDL_GetTicks();
    }
    
    
    
    if (renderer)   {   SDL_DestroyRenderer(renderer);    }
    if (window)     {   SDL_DestroyWindow(window);      }
    SDL_Quit();
}

void Game::startHost(){
    auto& p = players.emplace_back(new Player);
    localPlayer = p.get();
    localPlayer->isHost = true;
    
    listenSock.createTCP();
    MySocketAddress sa;
    sa.setPort(5000);
    sa.setIPv4(0, 0, 0, 0);
    listenSock.bind(sa);
    listenSock.listen(64);
}

void Game::startClient(){
    auto& p = players.emplace_back(new Player);
    localPlayer = p.get();
    auto& cs = localPlayer->sock;
    cs.createTCP();
    MySocketAddress sa;
    sa.setPort(5000);
    sa.setIPv4(127, 0, 0, 1);
    cs.connect(sa);
}

void Game::onUpdate(){
    if (localPlayer) {
        localPlayer->onUpdate();
    }
}

void Game::onEvent(SDL_Event& event){
    
    switch (event.type) {
        case SDL_KEYDOWN:
            switch( event.key.keysym.sym ){
                case SDLK_ESCAPE:   quit = SDL_TRUE;            break;
            }
            printf( "Key press detected\n" );                   break;
            
        case SDL_KEYUP: printf( "Key release detected\n" );     break;
        case SDL_QUIT: quit = SDL_TRUE;                         break;
    }
}

void Game::onDraw(){
    DrawRequest req;
    req.renderer = renderer;
    req.setColor({0, 0, 0, 255});
    SDL_RenderClear(renderer); // clean backBuffer
    
    for (const auto& p : players) {
        p->onDraw(req);
    }
    
    SDL_RenderPresent(renderer); // backBuffer to screen
}


