#include <iostream>
#include <string.h>

#define usage "Usage: ./ProyectoFinal.exe <[c,s]> <ip> <port>"

int main(int argc, char* argv[]){

    if(argc < 4 || argc > 4){
        std::cerr << usage << std::endl;
        return -1;
    }
    
    if (strcmp(argv[1], "c") == 0){
        // Client
    } else if(strcmp(argv[1], "s") == 0){
        // Server
    } else {
        std::cerr << usage << std::endl;
        return -1;
    }

    return 0;
}