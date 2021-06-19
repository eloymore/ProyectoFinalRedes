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
    void loop_thread();
    void broadcast(Message& msg);
    int getScore(Vector2<> pos);
    bool moveDartInAir();
private:
    Socket _netSock;
    std::vector<std::unique_ptr<Socket>> clients;
    std::vector<std::string> nicks;
    std::vector<int> clientScores;
    Vector2<> targetPos;
    float targetRadius;
    int scores[20] = { 6, 13, 4, 18, 1, 20, 5, 12, 9, 14, 11, 8, 16, 7, 19, 3, 17, 2, 15, 10 };
    int bullseye = 50;
    int clientTurn = 0;
    bool dartInAir = false;
    float targetDepth = 10, dartDepth, dartVelocity;
    Vector2<> dartPos;
};

#endif