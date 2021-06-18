#include "server.h"
#include "Messages.h"
#include <string.h>
#include <iostream>

void Server::net_thread(){
    while (true)
    {
        Message msgB;
        Socket* newSD;  
        char buffer[Message::MESSAGE_SIZE];
        _netSock.recv(msgB, buffer, newSD);
        Message* msg = &msgB;

        switch(msg->type){
            case Message::LOGIN:
            {
                std::cout << "Login de " << msg->nick << std::endl;
                auto i = clients.begin();
                while(i != clients.end() && !(*i->get() == *newSD)){ ++i; }
                // Si no se encuentra ya en el vector se pone
                if(i == clients.end()){
                    std::unique_ptr<Socket> u_ptr(newSD);
                    clients.push_back(std::move(u_ptr));
                }
                broadcast(*msg);
                break;
            }
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
            {
                std::cout << "Logout de " << msg->nick << std::endl;
                for(auto i = clients.begin(); i != clients.end(); ++i){
                    if(*i->get() == *newSD){
                        clients.erase(i);
                        break;
                    }
                } 
                broadcast(*msg);
                break;
            }
            default:
                std::cout << "Mensaje desconocido de " << msg->nick << std::endl;
            break;
        }
    }
}

void Server::broadcast(Message& msg){
    for(int i = 0; i < clients.size(); ++i){
        _netSock.send(msg, *clients[i].get());
    }
}