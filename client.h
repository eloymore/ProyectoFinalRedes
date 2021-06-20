#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "Socket.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include "Texture.h"
#include "Vector2.h"
#include <ctime>
#include <chrono>

#define CLIENTRATE 60

class Client{
public:
    Client(const char* ip, const char* port, std::string nick);
    ~Client();
    bool login();
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
    int state = -1; // -1 = not my turn/dart in air, 0 = moving dart, 1 = selecting strength
    Texture* _board;
    Texture* _dart;
    Texture* _power;
    int _dartX = 325, _dartY = 550;
    Vector2<float> _powerLimit = {1, 5};
    float _powerAmount = 1;
    Font* _NESfont;
    Texture* _text;
    std::chrono::_V2::system_clock::time_point timeSinceLastTick;
private:
    void Quit();
    bool running = true;
};

#endif