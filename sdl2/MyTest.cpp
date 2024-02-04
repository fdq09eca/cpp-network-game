//
//  MyTest.cpp
//  sdl2
//
//  Created by ChrisLam on 02/02/2024.
//

#include "MyTest.hpp"
#include "Game.hpp"
#include "Player.hpp"

void MyTest::testGetPlayerById() {
    Game g;
    auto& p = g.players.emplace_back(new Player);
    p->id = 1;
    
    auto q = g.getPlayerById(1);
    TEST(q != nullptr);
    TEST(p.get() == q);
    TEST(p->id == q->id);
    
    q = g.getPlayerById(0);
    
    TEST(q == nullptr);
    
    
}

void MyTest::testPlayer(){
    Player p1;
    p1.id = 1;
    p1.size = 20;
    p1.pos.x = 0;
    p1.pos.y = 1;
    p1.color.r = 255;
    p1.color.g = 180;
    p1.color.b = 0;
    
    Player p2;
    p2.id = 2;
    p2.size = 40;
    p2.pos.x = 1;
    p2.pos.y = 2;
    p2.color.r = 111;
    p2.color.g = 222;
    p2.color.b = 101;
    
    std::vector<uint8_t> buff;
    
    MySerialiser::se_Player(p1, buff);
    MySerialiser::se_Player(p2, buff);
    
    Player q1;
    auto e   = buff.data()+buff.size();
    auto ptr1 = MyDeserialiser::de_Player(q1, buff.data(), e);
    
    TEST(p1.id      == q1.id);
    TEST(p1.size    == q1.size);
    TEST(p1.pos.x   == q1.pos.x);
    TEST(p1.pos.y   == q1.pos.y);
    TEST(p1.color.r == q1.color.r);
    TEST(p1.color.g == q1.color.g);
    TEST(p1.color.b == q1.color.b);
    
    Player q2;
    auto ptr2 = MyDeserialiser::de_Player(q2, ptr1, e);
    
    
    TEST(p2.id      == q2.id);
    TEST(p2.size    == q2.size);
    TEST(p2.pos.x   == q2.pos.x);
    TEST(p2.pos.y   == q2.pos.y);
    TEST(p2.color.r == q2.color.r);
    TEST(p2.color.g == q2.color.g);
    TEST(p2.color.b == q2.color.b);
    TEST(ptr2 = e);
}

void MyTest::testInt(){
    int v = 50;
    std::vector<uint8_t> buff;
    
    {
        MySerialiser::se_Int(v, buff);
    }
    
    int u = -1;
    {
        MyDeserialiser::de_Int(u, buff.data(), buff.data() + buff.size());
    }
    
    TEST(u == v);
    TEST(buff.size() > 0);
}

void MyTest::testGameMaxId(){
    Game g;
    g.players.emplace_back(new Player);
    g.players.back()->id = -1;
    
    int newId = g.getNewPlayerId();
    TEST(newId == 0);
    
    g.players.emplace_back(new Player);
    g.players.back()->id = newId;
    newId = g.getNewPlayerId();
    TEST(newId == 1);
    
    g.players.emplace_back(new Player);
    g.players.back()->id = -2;
    
    newId = g.getNewPlayerId();
    TEST(newId == 1);
    
    g.players.emplace_back(new Player);
    g.players.back()->id = 2;
    
    newId = g.getNewPlayerId();
    TEST(newId == 3);
    

}
