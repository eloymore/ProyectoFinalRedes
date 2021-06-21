#ifndef _SERVER_H_
#define _SERVER_H_

#include "Socket.h"
#include <vector>
#include <memory>
#include "Vector2.h"
#include <chrono>
#include <ctime>

class Message;

#define SERVERNICK "SERVER"
#define SERVERRATE 60
#define MAXCLIENTS 1

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
    Vector2<> targetPos = { 375, 250 };
    float targetRadius = 250;
    int scores[20] = { 10, 12, 2, 17, 3, 19, 7, 16, 8, 11, 14, 9, 12, 5, 20, 1, 18, 4, 13, 6 };
    int bullseye = 50;
    int clientTurn = 0;
    bool dartInAir = false;
    float targetDepth = 10, dartDepth, dartVelocity;
    Vector2<> dartPos;
    std::chrono::_V2::system_clock::time_point timeSinceLastTick;
};

#endif