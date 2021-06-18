#ifndef _SERVER_H_
#define _SERVER_H_

#include "Socket.h"
#include <vector>
#include <memory>
#include "Vector2.h"

class Message;

class Server{
public:
    Server(const char* ip, const char* port) : _netSock(ip, port) { 
        _netSock.bind(); 
    }
    void net_thread();
    void broadcast(Message& msg);
    int getScore(Vector2<> pos);
private:
    Socket _netSock;
    std::vector<std::unique_ptr<Socket>> clients;
    Vector2<> targetPos;
    float targetRadius;
    int scores[20] = { 6, 13, 4, 18, 1, 20, 5, 12, 9, 14, 11, 8, 16, 7, 19, 3, 17, 2, 15, 10 };
    int bullseye = 50;
};

#endif