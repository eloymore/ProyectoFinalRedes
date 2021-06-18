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
        char alpha;
        std::cin >> alpha;
        MovementMessage msg(_nick, 10, -17);
        _netSock.send(msg, _netSock);
        // Render

    }
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
                MovementMessage* mMsg = static_cast<MovementMessage*>(msg);
                mMsg->from_bin(buffer);
                std::cout << "Movimiento: " << mMsg->x << "," << mMsg->y << std::endl;
                break;
            }
            case Message::CLICK:
            {
                ClickMessage* cMsg = static_cast<ClickMessage*>(msg);
                cMsg->from_bin(buffer);
                std::cout << "Click: " << cMsg->i << std::endl;
                break;
            }
            case Message::SCORE:
            {
                ScoreMessage* sMsg = static_cast<ScoreMessage*>(msg);
                sMsg->from_bin(buffer);
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