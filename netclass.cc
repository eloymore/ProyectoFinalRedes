#include "netclass.h"

#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <ostream>

NetClass::NetClass(std::string ip, std::string port) : _ip(ip), _port(port)
{
    struct addrinfo hints;
    struct addrinfo * res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(_ip.c_str(), _port.c_str(), &hints, &res);

    if (rc != 0)
    {
        std::cerr << "[getaddrinfo] " << gai_strerror(rc) << "\n";
    }

    //Con el resultado inicializar los miembros sd, sa y sa_len de la clase

    _sockId = socket(res->ai_family, res->ai_socktype, 0);

    if(_sockId == -1)
    {
        std::cerr << "[socket] " << strerror(errno) << "\n";
    }

    struct sockaddr sa = *res->ai_addr;
    socklen_t sa_len = res->ai_addrlen;

    bind(_sockId, (const struct sockaddr *) &sa, sa_len);

    freeaddrinfo(res);
}