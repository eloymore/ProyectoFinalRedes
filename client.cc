#include "client.h"
#include "Messages.h"
#include <iostream>
#include <algorithm>

#define COLOR(num) static_cast<Uint8>((num >> 24) & 0xff), static_cast<Uint8>((num >> 16) & 0xff), static_cast<Uint8>((num >> 8) & 0xff), static_cast<Uint8>(num & 0xff)

Client::Client(const char* ip, const char* port, std::string nick) : _netSock(ip, port), _nick(nick) {
    _nick[7] = '\0';
    if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)) { 
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
    }
}

Client::~Client(){
}

void Client::login(){
    Message msg(_nick, Message::LOGIN);
    _netSock.send(msg, _netSock);
    _window = SDL_CreateWindow(_nick.c_str(), 0, 0, 750, 750, 0);
    _renderer = SDL_CreateRenderer(_window, 0, 0);
    _board = new Texture(_renderer, "./textures/dartboard.png");
    _dart = new Texture(_renderer, "./textures/arrow.png");
}

void Client::logout(){
    Message msg(_nick, Message::LOGOUT);
    _netSock.send(msg, _netSock);
    SDL_Quit();
}

void Client::loop_thread(){
    bool running = true;
    while(running){
        SDL_Event pEvent;
        while (SDL_PollEvent(&pEvent))
        {
            if (pEvent.type == SDL_QUIT) 
                running = false;
            else if (pEvent.type == SDL_MOUSEBUTTONDOWN)
            {
                if(state == 1){
                    ClickMessage msg(_nick, pEvent.button.button);
                    _netSock.send(msg, _netSock);
                    state = -1;
                }
                else if(state == 0) state = 1;
            }
            else if(pEvent.type == SDL_MOUSEMOTION)
            {
                if(state == 0){
                    MovementMessage msg(_nick, pEvent.motion.xrel, pEvent.motion.yrel);
                    _netSock.send(msg, _netSock);
                }
            }
        }
        SDL_SetRenderDrawColor(_renderer, COLOR(0x964B00));
        SDL_RenderClear(_renderer);
        _board->render({125, 0, 500, 500});
        _dart->render({_dartX, _dartY, 100, 100});
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
                std::cout << "Login de " << msg->nick << std::endl;
                break;
            case Message::MOVEMENT:
            {
                MovementMessage mMsg;
                mMsg.from_bin(buffer);
                std::cout << "Movimiento: " << mMsg.x << "," << mMsg.y << std::endl;
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
                std::cout << "Score: " << sMsg.i << std::endl;

                scores[std::distance(nicks.begin(), std::find(nicks.begin(), nicks.end(), sMsg.nick))] = sMsg.i;
                break;
            }
            case Message::LOGOUT:
                std::cout << "Logout de " << msg->nick << std::endl;
                break;
            default:
                std::cout << "Mensaje desconocido de " << msg->nick << std::endl;
            break;
        }
    }
}