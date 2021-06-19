#include "server.h"
#include "Messages.h"
#include <string.h>
#include <iostream>
#include <math.h>

void Server::net_thread(){
    while (true)
    {
        Message msgB;
        Socket* newSD = &_netSock;
        char buffer[Message::MESSAGE_SIZE];
        _netSock.recv(msgB, buffer, newSD);
        switch(msgB.type){
            case Message::LOGIN:
            {
                std::cout << "Login de " << msgB.nick << std::endl;

                auto i = 0;
                while(i < clients.size() && strcmp(msgB.nick.c_str(), nicks[i].c_str()) != 0 && strcmp(msgB.nick.c_str(), SERVERNICK) != 0) { ++i; } // SERVERNICK se reserva para el servidor
                if(i == clients.size()){    // Si no se encuentra ya en el vector se pone
                    Message connRefused(SERVERNICK, Message::CONNACCEPTED); // TCP de palo para informar al cliente si debe cerrarse
                    _netSock.send(connRefused, *newSD);

                    std::unique_ptr<Socket> u_ptr(newSD);
                    clients.push_back(std::move(u_ptr));
                    nicks.push_back(msgB.nick);
                    clientScores.push_back(0);
                }
                else {
                    std::cout << "Login de " << msgB.nick << " fallido, ya estaba metido" << std::endl;
                    Message connRefused(SERVERNICK, Message::CONNREFUSED);
                    _netSock.send(connRefused, *newSD);
                }
                broadcast(msgB);

                if(clients.size() == 1){ // Si es el primer cliente, le dice que es su turno
                    Message turn(SERVERNICK, Message::TURN);
                    _netSock.send(turn, *(clients[0].get()));
                }
                break;
            }
            case Message::MOVEMENT:
            {
                MovementMessage mMsg;
                mMsg.from_bin(buffer);
                //std::cout << "Movimiento: " << mMsg.x << "," << mMsg.y << std::endl;

                dartPos = {mMsg.x, mMsg.y};
                //std::cout << dartPos.x << " | " << dartPos.y << "\n";

                broadcast(mMsg);
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
            case Message::VELOCITY:
            {
                VelocityMessage vMsg;
                vMsg.from_bin(buffer);
                dartVelocity = vMsg.velocity;
                dartInAir = true;
                std::cout << "Velocidad de " << vMsg.nick << ": " << vMsg.velocity << std::endl;
                delete newSD;
                break;
            }
            case Message::LOGOUT:
            {
                std::cout << "Logout de " << msgB.nick << std::endl;
                broadcast(msgB);
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
                clientScores[clientTurn] += getScore(dartPos);
                ScoreMessage sm(nicks[clientTurn], clientScores[clientTurn]);
                _netSock.send(sm, *(clients[clientTurn].get()));    // TODO: broadcast
                clientTurn = (clientTurn + 1) % clients.size();     // Cambio de turno      
                Message nt(SERVERNICK, Message::TURN);
                _netSock.send(nt, *(clients[clientTurn].get()));    // Siguiente turno
                MovementMessage mm(SERVERNICK, 325, 500);
                _netSock.send(mm, *(clients[clientTurn].get()));    // Actualizar posición del dardo
                dartInAir = false;
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

int Server::getScore(Vector2<> pos){
    Vector2<> dirFromCenter = pos - targetPos;
    int score = 0;
    if(dirFromCenter.magnitude() < targetRadius){
        float angleBetween = std::atan2(-dirFromCenter.y, -dirFromCenter.x) * 180 / M_PI;
        angleBetween += 180;

        std::cout << "Angle: " << angleBetween << std::endl;

        if(dirFromCenter.magnitude() > (targetRadius * 0.1f)){

            score = scores[(int)(angleBetween / 18.0f)];
            
            if(dirFromCenter.magnitude() > targetRadius * 0.40f && dirFromCenter.magnitude() < targetRadius * 0.45f){
                score *= 2;
            } else if (dirFromCenter.magnitude() > targetRadius * 0.65f && dirFromCenter.magnitude() < targetRadius * 0.7f){
                score *= 3;
            }
        }else{
            score = bullseye;
        }

        std::cout << "Score: " << score << std::endl;
    }
    return score;
}