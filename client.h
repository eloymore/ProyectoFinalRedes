#ifndef _CLIENT_
#define _CLIENT_

#include "netclass.h"

class Client : public NetClass{
    public:
    Client(std::string ip, std::string port);
    bool run() override;
};

#endif