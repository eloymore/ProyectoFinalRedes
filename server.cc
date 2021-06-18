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
                if(i == clients.end()){
                    std::unique_ptr<Socket> u_ptr(newSD);
                    clients.push_back(std::move(u_ptr));
                }
                break;
            }
            case Message::MOVEMENT:
            {
                MovementMessage mMsg;
                mMsg.from_bin(buffer);
                std::cout << "Movimiento: " << mMsg.x << "," << mMsg.y << std::endl;
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
                for(auto i = clients.begin(); i != clients.end(); ++i){
                    if(*i->get() == *newSD){
                        clients.erase(i);
                        break;
                    }
                } 
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

int Server::getScore(Vector2<> pos){
    Vector2<> dirFromCenter = pos - targetPos;
    int score = 0;
    if(dirFromCenter.magnitude() < targetRadius){
        float angleBetweenR = dirFromCenter.angleDegrees(Vector2<>::right());
        float angleBetweenL = dirFromCenter.angleDegrees(Vector2<>::left());
        angleBetweenR -= 6, angleBetweenL -= 6, angleBetweenL += 180;

        if(dirFromCenter.magnitude() > targetRadius * 0.1f){
            if(pos.y > targetPos.y)
                score = scores[(int)angleBetweenR / 13];
            else
                score = scores[(int)angleBetweenL / 13];
            }

            if(dirFromCenter.magnitude() > targetRadius * 0.475f && dirFromCenter.magnitude() < targetRadius * 0.525f){
                score *= 2;
            } else if (dirFromCenter.magnitude() > targetRadius * 0.95f){
                score *= 3;
            }
        else
            score = bullseye;
    }
    return score;
}