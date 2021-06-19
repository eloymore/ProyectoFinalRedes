#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "Socket.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Client{
    public:
    Client(const char* ip, const char* port, std::string nick);
    ~Client();
    void login();
    void logout();
    void loop_thread();
    void net_thread();
    std::string _nick;
    private:
    Socket _netSock;
    SDL_Window* _window;
    SDL_Renderer* _renderer;
};

#endif