//
//  MySocket.hpp
//  cpp-network-http
//
//  Created by ChrisLam on 09/01/2024.
//

#ifndef MySocket_hpp
#define MySocket_hpp
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "MyUtil.hpp"
#define INVALID_SOCKET -1

using SOCKET = int;


class MySocketAddress : public NonCopyable {
    
public:
    sockaddr addr;
    
    void setPort(uint16_t port){
        auto* p = reinterpret_cast<uint16_t*>(addr.sa_data);
        *p = htons(port);
    }
    
    void setIPv4(uint8_t a, uint8_t b, uint8_t c, uint8_t d){
        addr.sa_family = AF_INET;
        addr.sa_data[2] = a;
        addr.sa_data[3] = b;
        addr.sa_data[4] = c;
        addr.sa_data[5] = d;
    }
    
    uint16_t port(){
        auto* p = reinterpret_cast<uint16_t*>(addr.sa_data);
        return ntohs(*p);
    }
    
    void print() {
        printf("%hu.%hu.%hu.%hu : %hu\n", addr.sa_data[2], addr.sa_data[3], addr.sa_data[4], addr.sa_data[5], port());
    }
};

class MySocket : public NonCopyable {
public:
    SOCKET _sock = INVALID_SOCKET;
    ~MySocket (){ close(); }
    MySocket() = default;
    
    MySocket(MySocket&& rhs){
        *this = std::move(rhs);
    }
    
    void operator=(MySocket&& rhs){
        _sock = rhs._sock;
        rhs._sock = INVALID_SOCKET;
    }
    
    
    
    void close(){
        if (!isValid()) return;
        auto r = ::close(_sock);
        _sock = INVALID_SOCKET;
        if (r < 0) { throw MyError("close"); }
    }
    
    void createUDP() {
        close();
        _sock = ::socket(PF_INET, SOCK_DGRAM, 0);
        if (!isValid()) { throw MyError("createUDP"); }
    };
    
    void createTCP() {
        close();
        _sock = ::socket(PF_INET, SOCK_STREAM, 0);
        if (!isValid()) { throw MyError("createTCP"); }
    };
    
    void connect(MySocketAddress& dst_sa) {
        auto r = ::connect(_sock, &dst_sa.addr, sizeof(dst_sa.addr));
        if (r < 0) { MyError("connect"); }
        printf("connected: "); dst_sa.print(); printf("\n");
    };
    
    void bind(MySocketAddress& dst_sa) {
        auto r = ::bind(_sock, &dst_sa.addr, sizeof(dst_sa.addr));
        if (r < 0) { MyError("bind"); }
        printf("bind: "); dst_sa.print(); printf("\n");
    };
    
    void listen(int backlog) {
        auto n = ::listen(_sock, backlog);
        if (n < 0) { throw MyError("listen"); }
        puts("listen...");
    }
    
    
    
    void accept(MySocket& outSocket) { // not interest in client address
        outSocket.close();
        auto n = ::accept(_sock, nullptr, nullptr);
        if (n < 0) { throw MyError("accept"); }
        outSocket._sock = n;
        puts("accept.");
    };
    
    
    
    size_t nByteToRead(){
        int r = -1;
        if (ioctl(_sock, FIONREAD, &r) < 0
            || r < 0) { throw MyError("nByteToRecv"); }
        return static_cast<size_t>(r);
    }
    
    size_t sendto(uint8_t* data, size_t dataSize, MySocketAddress* dst) {
        if (!data) { throw MyError("sendto, data is nullptr"); }
        auto n = ::sendto(_sock, data, dataSize, 0, &dst->addr, sizeof(dst->addr));
        if (n < 0) { throw MyError("sendto"); }
        return n;
    };
    
    size_t send(uint8_t* data, size_t dataSize) {
        if (!data) { throw MyError("send, data is nullptr"); }
        auto n = ::send(_sock, data, dataSize, 0);
        if (n < 0) { throw MyError("send"); }
        return n;
    };
    
    size_t send(const char* c_str) {
        if (!c_str) { throw MyError("send, c_str is nullptr"); }
        auto n = ::send(_sock, c_str, strlen(c_str), 0);
        printf("[send] %zu byte\n", n);
        if (n < 0) { throw MyError("send"); }
        return n;
    }
    
    size_t recvfrom(uint8_t* outBuff, size_t dataSize, MySocketAddress& outSrc) {
        auto n = ::recvfrom(_sock, outBuff, dataSize, 0, &outSrc.addr, nullptr);
        if (n < 0) { throw MyError("recvfrom"); }
        return n;
    };
    
    size_t recv(std::vector<uint8_t>& outBuff, size_t dataSize) {
        outBuff.resize(dataSize);
        uint8_t* p = outBuff.data();
        return recv(p, dataSize);
    }
    
    size_t recv(uint8_t* outBuff, size_t dataSize) {
        auto n = ::recv(_sock, outBuff, dataSize, 0);
        if (n < 0) { throw MyError("recv"); }
        std::string s;
        s.assign(outBuff, outBuff + dataSize);
        
        printf("[recv] %s", s.c_str());
        return n;
    };
    
    inline bool isValid() { return _sock != INVALID_SOCKET; }
    
    SOCKET sock() const { return _sock; }

};

class MyFdSet {
    
public:
    fd_set readSet;
    fd_set writeSet;
    fd_set errorSet;
    SOCKET max_fd;
    
    MyFdSet() {
        clear();
    }
    
    void clear() {
        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        FD_ZERO(&errorSet);
        max_fd = -1;
    }
    
    bool hasRead (const MySocket& s) { return FD_ISSET(s.sock(), &readSet); }
    bool hasWrite(const MySocket& s) { return FD_ISSET(s.sock(), &writeSet); }
    bool hasError(const MySocket& s) { return FD_ISSET(s.sock(), &errorSet); }
    
    inline void updateMaxFd(const MySocket& s){
        if (s.sock() > max_fd) {
            max_fd = s.sock();
        }
    }
    
    inline void addRead(const MySocket& s) {
        FD_SET(s.sock(), &readSet);
        updateMaxFd(s);
    }
    
    inline void addWrite(const MySocket& s) {
        FD_SET(s.sock(), &writeSet);
        updateMaxFd(s);
    }
    
    inline void addReadWrtie(const MySocket& s) {
        addRead(s);
        addWrite(s);
    }
    
    int select(){
        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        
        int r = ::select(max_fd + 1, &readSet, &writeSet, &errorSet, &tv);
        if (r < 0) {
            throw MyError("select");
        }
        return r;
    }

};

#endif /* MySocket_hpp */
