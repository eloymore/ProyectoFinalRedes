#include "client.h"
#include "Messages.h"
#include <iostream>
#include <algorithm>

#define COLOR(num) static_cast<Uint8>((num >> 24) & 0xff), static_cast<Uint8>((num >> 16) & 0xff), static_cast<Uint8>((num >> 8) & 0xff), static_cast<Uint8>(num & 0xff)

Client::Client(const char* ip, const char* port, std::string nick) : _netSock(ip, port), _nick(nick) {
    _nick[7] = '\0';
    nicks.push_back(_nick);
    scores.push_back(0);
    if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)) { 
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
    }
}

Client::~Client(){
    Quit();
}

bool Client::login(){
    Message msg(_nick, Message::LOGIN);
    _netSock.send(msg, _netSock);

    _window = SDL_CreateWindow(_nick.c_str(), 0, 0, 750, 750, 0);
    _renderer = SDL_CreateRenderer(_window, 0, 0);
    _board = new Texture(_renderer, "./textures/dartboard.png");
    _dart = new Texture(_renderer, "./textures/dart.png");
    _power = new Texture(_renderer, "./textures/power.png");
    //_NESfont = new Font("./fonts/NES-chimera.ttf", 10);
    return true;
}

void Client::logout(){
    Message msg(_nick, Message::LOGOUT);
    _netSock.send(msg, _netSock);
}

void Client::loop_thread(){
    running = true;
    while(running){
        SDL_Event pEvent;
        _powerAmount += 0.05;
        if(_powerAmount > _powerLimit.y) _powerAmount = _powerLimit.x;
        while (SDL_PollEvent(&pEvent))
        {
            if (pEvent.type == SDL_QUIT) 
                running = false;
            else if (pEvent.type == SDL_MOUSEBUTTONDOWN)
            {
                if(state == 1){
                    VelocityMessage msg(_nick, _powerAmount);
                    _netSock.send(msg, _netSock);
                    state = -1;
                }
                else if(state == 0) state = 1;
            }
            else if(pEvent.type == SDL_MOUSEMOTION)
            {
                if(state == 0){
                    MovementMessage msg(_nick, pEvent.motion.x, pEvent.motion.y);
                    _netSock.send(msg, _netSock);
                    _dartX = pEvent.motion.x - 50;
                    _dartY = pEvent.motion.y - 50;
                }
            }
        }
        SDL_SetRenderDrawColor(_renderer, COLOR(0x964B00));
        SDL_RenderClear(_renderer);
        _board->render({125, 0, 500, 500});
        _dart->render({_dartX, _dartY, 100, 100});
        if(state == 1) _power->render({_dartX + 100, _dartY, 100, 100}, {0, 0, (int)(100 * _powerAmount/_powerLimit.y), 100 });
        SDL_RenderPresent(_renderer);
    }
    logout();
}

void Client::net_thread(){
    while(true){
        Message msgB;
        char buffer[Message::MESSAGE_SIZE];
        _netSock.recv(msgB, buffer);
        Message* msg = &msgB;
        
        switch(msg->type){
            case Message::LOGIN:
            {
                if(strcmp(msg->nick.c_str(), _nick.c_str()) == 0) break;

                std::cout << "Login de " << msg->nick << std::endl;

                nicks.push_back(msg->nick);
                scores.push_back(0);

                ScoreMessage sMsg(_nick, scores[0]);    // Se da a conocer por el resto
                _netSock.send(sMsg, _netSock);
                break;
            }
            case Message::CONNREFUSED:
                running = false;
                break;
            case Message::MOVEMENT:
            {
                if(strcmp(msg->nick.c_str(), _nick.c_str()) == 0) break;

                MovementMessage mMsg;
                mMsg.from_bin(buffer);
                std::cout << "Movimiento: " << mMsg.x << "," << mMsg.y << std::endl;

                _dartX = mMsg.x;
                _dartY = mMsg.y;
                break;
            }
            case Message::CLICK:
            {
                ClickMessage cMsg;
                cMsg.from_bin(buffer);
                std::cout << "Click de : " << cMsg.nick << " " << cMsg.i << std::endl;
                break;
            }
            case Message::SCORE:
            {
                ScoreMessage sMsg;
                sMsg.from_bin(buffer);

                std::cout << "Score de " << sMsg.nick << ": " << sMsg.i << std::endl;
                int dist = std::distance(nicks.begin(), std::find(nicks.begin(), nicks.end(), sMsg.nick));
                if(dist < scores.size()) scores[dist] = sMsg.i; // Si existe lo pone
                else{                                           // Si no lo añade y lo pone
                    nicks.push_back(msg->nick);
                    scores.push_back(sMsg.i);
                }

                break;
            }
            case Message::TURN:
            {
                state = 0;
                std::cout << "My turn now >:D" << std::endl;
                break;
            }
            case Message::LOGOUT:
                if(strcmp(msg->nick.c_str(), _nick.c_str()) == 0) break;

                std::cout << "Logout de " << msg->nick << std::endl;

                scores.erase(scores.begin() + std::distance(nicks.begin(), std::find(nicks.begin(), nicks.end(), msg->nick)));
                nicks.erase(std::find(nicks.begin(), nicks.end(), msg->nick));
                break;
            default:
                std::cout << "Mensaje desconocido de " << msg->nick << std::endl;
            break;
        }
    }
}

void Client::Quit(){
    // delete _window;
    // delete _renderer;
    SDL_Quit();
}