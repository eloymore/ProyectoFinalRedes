#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "Socket.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include "Texture.h"

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
    std::vector<std::string> nicks;
    std::vector<int> scores;
    int state = 0; // -1 = not my turn/dart in air, 0 = moving dart, 1 = selecting strength
    Texture* _board;
    Texture*_dart;
    int _dartX = 325, _dartY = 550;
};

#endif