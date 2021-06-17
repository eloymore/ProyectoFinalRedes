#ifndef _CLIENT_
#define _CLIENT_

#include "Socket.h"

class Client{
    public:
    Client();
    private:
    Socket _netSock;
};

#endif