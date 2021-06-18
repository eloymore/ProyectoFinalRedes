#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "Socket.h"

class Client{
    public:
    Client(const char* ip, const char* port, std::string nick) : _netSock(ip, port), _nick(nick) { _nick[7] = '\0'; };
    void login();
    void logout();
    void loop_thread();
    void net_thread();
    std::string _nick;
    private:
    Socket _netSock;
};

#endif