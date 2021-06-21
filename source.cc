#include "client.h"
#include "server.h"
#include <iostream>
#include <string.h>
#include <thread>

#define usage "USAGE: \nServer: ./ProyectoFinal.exe s <ip> <port>\nClient: ./ProyectoFinal.exe c [nick] <ip> <port>"

int main(int argc, char* argv[]){

    if(argc < 4 || argc > 5){
        std::cerr << usage << std::endl;
        return -1;
    }
    
    if (strcmp(argv[1], "c") == 0){
        // Client
        Client client(argv[3], argv[4], argv[2]);
        client.login();
        std::thread netThread([&client]{
            client.net_thread();
        });
        netThread.detach();
        client.loop_thread();
    } else if(strcmp(argv[1], "s") == 0){
        // Server
        Server server(argv[2], argv[3]);
        std::thread netThread([&server]{
            server.net_thread();
        });
        netThread.detach();
        server.loop_thread();
    } else {
        std::cerr << usage << std::endl;
        return -1;
    }

    return 0;
}