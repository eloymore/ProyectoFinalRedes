#include "client.h"
#include "Messages.h"
#include <iostream>

void Client::login(){
    Message msg(_nick, Message::LOGIN);
    _netSock.send(msg, _netSock);
}

void Client::logout(){
    Message msg(_nick, Message::LOGOUT);
    _netSock.send(msg, _netSock);
}

void Client::loop_thread(){
    while(true){
    // Input

    // Render

    }
}

void Client::net_thread(){
    while(true){
        Message* msg;
        _netSock.recv(*msg);

        switch(msg->type){
            case Message::LOGIN:
                std::cout << "Login de " << msg->nick << std::endl;
                break;
            case Message::MOVEMENT:
            {
                MovementMessage* mMsg = static_cast<MovementMessage*>(msg);
                std::cout << "Movimiento: " << mMsg->x << "," << mMsg->y << std::endl;
                break;
            }
            case Message::CLICK:
            {
                ClickMessage* cMsg = static_cast<ClickMessage*>(msg);
                std::cout << "Click: " << cMsg->i << std::endl;
                break;
            }
            case Message::SCORE:
            {
                ScoreMessage* sMsg = static_cast<ScoreMessage*>(msg);
                std::cout << "Score: " << sMsg->i << std::endl;
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