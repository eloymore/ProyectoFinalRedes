#include "server.h"
#include "Messages.h"
#include <string.h>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <unistd.h>

void Server::net_thread(){
    while (true)
    {
        Message msgB; // TODO: Meter los mensajes en una lista, no enviarlos directamente, esperar a que el loop decida que toca y enviarlos de golpe
        Socket* newSD = &_netSock;
        char buffer[Message::MESSAGE_SIZE];
        _netSock.recv(msgB, buffer, newSD);
        switch(msgB.type){
            case Message::LOGIN:
            {
                std::cout << "Login de " << msgB.nick << std::endl;
                
                if(clients.size() >= MAXCLIENTS){
                    std::cout << "Login de " << msgB.nick << " fallido, no cabe" << std::endl;
                    Message connRefused(SERVERNICK, Message::CONNREFUSED);
                    _netSock.send(connRefused, *newSD);
                    delete newSD;
                    break;
                }

                auto i = 0;
                while(i < clients.size() && strcmp(msgB.nick.c_str(), nicks[i].c_str()) != 0 && strcmp(msgB.nick.c_str(), SERVERNICK) != 0) { ++i; } // SERVERNICK se reserva para el servidor
                if(i == clients.size()){    // Si no se encuentra ya en el vector y cabe, se pone
                    std::unique_ptr<Socket> u_ptr(newSD);
                    clients.push_back(std::move(u_ptr));
                    nicks.push_back(msgB.nick);
                    clientScores.push_back(0);

                    broadcast(msgB);

                    if(clients.size() == 1){ // Si es el primer cliente, le dice que es su turno
                        Message turn(SERVERNICK, Message::TURN);
                        _netSock.send(turn, *(clients[0].get()));
                    }

                    for(int i = 0; i < clientScores.size() - 1; ++i){
                        ClientInfoMessage ciMsg(nicks[i], clientScores[i]);
                        _netSock.send(ciMsg, *(clients[clientScores.size() - 1].get()));
                    }
                }
                else {
                    std::cout << "Login de " << msgB.nick << " fallido, ya estaba metido" << std::endl;
                    Message connRefused(SERVERNICK, Message::CONNREFUSED);
                    _netSock.send(connRefused, *newSD);
                    delete newSD;
                }
                break;
            }
            case Message::MOVEMENT:
            {

                MovementMessage mMsg;
                mMsg.from_bin(buffer);
                
                //std::cout << "Movimiento: " << mMsg.x << "," << mMsg.y << std::endl;

                dartPos = {mMsg.x, mMsg.y};

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
            case Message::VELOCITY:
            {
                VelocityMessage vMsg;
                vMsg.from_bin(buffer);
                dartVelocity = vMsg.f;
                dartInAir = true;

                std::cout << "Velocidad de " << vMsg.nick << ": " << vMsg.f << std::endl;

                delete newSD;
                break;
            }
            case Message::LOGOUT:
            {
                int dist = std::distance(nicks.begin(), std::find(nicks.begin(), nicks.end(), msgB.nick));
                if(dist >= clients.size()) break;

                std::cout << "Logout de " << msgB.nick << std::endl;

                clientScores.erase(clientScores.begin() + dist);
                nicks.erase(nicks.begin() + dist);
                clients.erase(clients.begin() + dist);

                broadcast(msgB);
                if(dist == clientTurn && clients.size() > 0){   // Si se ha ido el que ten??a el turno debe pasarse
                    clientTurn = (clientTurn + 1) % clients.size();     // Cambio de turno      
                    Message nt(SERVERNICK, Message::TURN);
                    _netSock.send(nt, *(clients[clientTurn].get()));    // Siguiente turno
                    MovementMessage mm(SERVERNICK, 325, 500);
                    _netSock.send(mm, *(clients[clientTurn].get()));    // Actualizar posici??n del dardo
                    dartInAir = false;
                } else if(dist < clientTurn && clients.size() > 0){ // Si estaba delante suya, debe offsetearse 1 a la iozquierda el turno 
                    clientTurn--;                                   // para que no se le ponga al de su derecha la puntuaci??n por ejemplo
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

void Server::loop_thread(){
    timeSinceLastTick = std::chrono::system_clock::now();
    while(true){
        if(dartInAir){
            if (moveDartInAir()){
                dartDepth = 0;
                clientScores[clientTurn] += getScore(dartPos);
                ScoreMessage sm(nicks[clientTurn], clientScores[clientTurn]);
                broadcast(sm);  
                DepthMessage dm("SERVER", dartDepth/targetDepth);
                broadcast(dm);
                clientTurn = (clientTurn + 1) % clients.size();     // Cambio de turno      
                Message nt(SERVERNICK, Message::TURN);
                _netSock.send(nt, *(clients[clientTurn].get()));    // Siguiente turno
                MovementMessage mm(SERVERNICK, 325, 500);
                _netSock.send(mm, *(clients[clientTurn].get()));    // Actualizar posicion del dardo
                dartInAir = false;
                std::cout << "Turno de " << clientTurn << std::endl;
            }
        }
        std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now()-timeSinceLastTick;
        if(elapsed_seconds.count() < 1.0f / SERVERRATE){
            usleep((1.0f / SERVERRATE - elapsed_seconds.count()) * 1000000);
        }
        timeSinceLastTick = std::chrono::system_clock::now();
    }
}

bool Server::moveDartInAir(){

    Vector2<> gravity = Vector2<>::down() * 9.8f / 60;

    dartPos -= gravity;
    MovementMessage mMsg("SERVER", dartPos.x, dartPos.y);
    broadcast(mMsg);

    DepthMessage dm("SERVER", dartDepth/targetDepth);
    broadcast(dm);

    dartDepth += dartVelocity / 60.0f;

    return dartDepth >= targetDepth;
}

int Server::getScore(Vector2<> pos){
    Vector2<> dirFromCenter = pos - targetPos;
    int score = 0;
    if(dirFromCenter.magnitude() < targetRadius * 0.7f){
        float angleBetween = std::atan2(-dirFromCenter.y, -dirFromCenter.x) * 180 / M_PI;
        angleBetween += 180;

        std::cout << "Angle: " << angleBetween << std::endl;

        if(dirFromCenter.magnitude() > (targetRadius * 0.1f)){

            score = scores[(int)(angleBetween / 18.0f)];
            
            if(dirFromCenter.magnitude() > targetRadius * 0.40f && dirFromCenter.magnitude() < targetRadius * 0.45f){
                score *= 2;
            } else if (dirFromCenter.magnitude() > targetRadius * 0.65f){
                score *= 3;
            }
        }else{
            score = bullseye;
        }

        std::cout << "Score: " << score << std::endl;
    }
    return score;
}