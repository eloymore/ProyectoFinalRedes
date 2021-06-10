#ifndef _NETCLASS_
#define _NETCLASS_

#include <string>

// Clase padre de cliente y servidor
// Contiene informacion de la ip, el puerto y el identificador de socket
class NetClass{
public:
    NetClass(std::string ip, std::string port);
    virtual bool run() = 0;
    virtual bool send(std::)
protected:
    std::string _ip, _port;
    int _sockId;
};

#endif