#include "client.h"
#include "Messages.h"
#include <iostream>
#include <algorithm>

#define COLOR(num) static_cast<Uint8>((num >> 24) & 0xff), static_cast<Uint8>((num >> 16) & 0xff), static_cast<Uint8>((num >> 8) & 0xff), static_cast<Uint8>(num & 0xff)

Client::Client(const char* ip, const char* port, std::string nick) : _netSock(ip, port), _nick(nick) {
    //nick[7] = '\0';  // Provoca que se corte al mostrar la puntuación propia
    std::string realNick(nick, 0, 8);
    _nick = realNick;
    nicks.push_back(_nick);
    scores.push_back(0);
    if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)) { 
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
    }
    if((TTF_Init()==-1)) { 
        printf("Could not initialize TTF: %s.\n", TTF_GetError());
        exit(-1);
    };
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
    _NESfont = new Font("./fonts/NES-Chimera.ttf", 10);
    _text = new Texture(_renderer, "./textures/power.png");
    return true;
}

void Client::logout(){
    Message msg(_nick, Message::LOGOUT);
    _netSock.send(msg, _netSock);
}

void Client::loop_thread(){
    running = true;
    timeSinceLastTick = std::chrono::system_clock::now();

    while(running){

        // Input
        SDL_Event pEvent;
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
                else if(state == 0) {
                    state = 1;
                    _powerAmount = 0.0;
                }
            }
            else if(pEvent.type == SDL_MOUSEMOTION)
            {
                if(state == 0){
                    MovementMessage msg(_nick, pEvent.motion.x, pEvent.motion.y);
                    _netSock.send(msg, _netSock);
                    _dartX = pEvent.motion.x;
                    _dartY = pEvent.motion.y;
                }
            }
        }

        // Update
        std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now()-timeSinceLastTick;
        if(elapsed_seconds.count() > 1.0f / CLIENTRATE)
        {
            _powerAmount += 0.125;
            if(_powerAmount > _powerLimit.y) _powerAmount = _powerLimit.x;
            timeSinceLastTick = std::chrono::system_clock::now();
        }

        // Render
        SDL_SetRenderDrawColor(_renderer, COLOR(0x964B00));
        SDL_RenderClear(_renderer);
        _board->render({125, 0, 500, 500});
        int depthDim = (int)(100 - (_cDepth * (_sizeVar.y - _sizeVar.x)));
        _dart->render({_dartX - depthDim/2, _dartY - depthDim/2, depthDim, depthDim});
        if(state == 1) _power->render({_dartX + 100, _dartY, 100, 100}, {0, 0, (int)(100 * _powerAmount/_powerLimit.y), 100 });
        for(int i = 0; i < scores.size(); ++i){
            _text->loadFromText(_renderer, nicks[i] + ": " + std::to_string(scores[i]), _NESfont);
            _text->render({10 + (80 * i), 700, 70, 50});
        }
        SDL_RenderPresent(_renderer);
    }
    
    if(connAccepted)
    {
        logout();
    }
    else std::cout << "Conexion rechazada: numero de clientes superado o nick incorrecto (SERVER esta reservado)\n";
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
                break;
            }
            case Message::CONNREFUSED:
                running = false;
                connAccepted = false;
                break;
            case Message::MOVEMENT:
            {
                if(strcmp(msg->nick.c_str(), _nick.c_str()) == 0) break;

                MovementMessage mMsg;
                mMsg.from_bin(buffer);

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
                scores[dist] = sMsg.i; // Si existe lo pone

                break;
            }
            case Message::DEPTH:
            {
                DepthMessage dMsg;
                dMsg.from_bin(buffer);

                _cDepth = dMsg.f;

                break;
            }
            case Message::CLIENTINFO:
            {
                ClientInfoMessage ciMsg;
                ciMsg.from_bin(buffer);

                std::cout << "Registrado " << ciMsg.nick << " con score " << ciMsg.i << std::endl;
                nicks.push_back(ciMsg.nick);
                scores.push_back(ciMsg.i);
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
    SDL_DestroyWindow(_window);
    SDL_DestroyRenderer(_renderer);
    SDL_Quit();
    TTF_Quit();
}