#ifndef _SERVER_H_
#define _SERVER_H_

#include "Socket.h"
#include <vector>
#include <memory>

class Message;

class Server{
public:
    Server(const char* ip, const char* port) : _netSock(ip, port) { 
        _netSock.bind(); 
    }
    void net_thread();
    void broadcast(Message& msg);
private:
    Socket _netSock;
    std::vector<std::unique_ptr<Socket>> clients;
};

#endif