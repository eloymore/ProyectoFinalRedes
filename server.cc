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

        switch(msgB.type){
            case Message::LOGIN:
            {
                std::cout << "Login de " << msgB.nick << std::endl;
                auto i = clients.begin();
                while(i != clients.end() && !(*i->get() == *newSD)){ ++i; }
                // Si no se encuentra ya en el vector se pone
                /*if(i == clients.end()){
                    std::unique_ptr<Socket> u_ptr(newSD);
                    clients.push_back(std::move(u_ptr));
                }*/
                //broadcast(msgB);
                break;
            }
            case Message::MOVEMENT:
            {
                MovementMessage mMsg;
                mMsg.from_bin(buffer);
                std::cout << "Movimiento: " << mMsg.x << "," << mMsg.y << std::endl;

                dartPos += {mMsg.x, mMsg.y};

                //broadcast(mMsg);
                delete newSD;
                break;
            }
            case Message::CLICK:
            {
                ClickMessage cMsg;
                cMsg.from_bin(buffer);
                std::cout << "Click de " << cMsg.nick << ": " << cMsg.i << std::endl;
                delete newSD;
                break;
            }
            case Message::SCORE:
            {
                ScoreMessage sMsg;
                sMsg.from_bin(buffer);
                std::cout << "Score de " << sMsg.nick << ": " << sMsg.i << std::endl;
                delete newSD;
                break;
            }
            case Message::LOGOUT:
            {
                std::cout << "Logout de " << msgB.nick << std::endl;
                /*for(auto i = clients.begin(); i != clients.end(); ++i){
                    if(*i->get() == *newSD){
                        clients.erase(i);
                        break;
                    }
                }*/
                delete newSD;
                break;
            }
            default:
                std::cout << "Mensaje desconocido de " << msgB.nick << std::endl;
                delete newSD;
            break;
        }
    }
}

void Server::broadcast(Message& msg){
    for(auto it = clients.begin(); it != clients.end(); ++it){               
        _netSock.send(msg, *it->get());
    }
}

void Server::loop_thread(){
    while(true){
        if(dartInAir){
            if (moveDartInAir()){
                scores[clientTurn] += getScore(dartPos);
                ScoreMessage sm(nicks[clientTurn], scores[clientTurn]);
                _netSock.send(sm, *(clients[clientTurn].get())); // TODO: broadcast
            }
        }
    }
}

bool Server::moveDartInAir(){

    Vector2<> gravity = Vector2<>::down() * 9.8f;

    dartPos += gravity;

    dartDepth += dartVelocity;

    return dartDepth >= targetDepth;
}