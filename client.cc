#include "client.h"
#include "Messages.h"
#include <iostream>
#include <SDL.h>

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
        ScoreMessage msg(_nick, 109);
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