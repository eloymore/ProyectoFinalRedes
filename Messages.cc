#include "Messages.h"

#include <string.h>

void Message::to_bin(){
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* bin = _data;

    memcpy(bin, &type, 1);
    bin += 1;

    memcpy(bin, nick.c_str(), 8);
    bin += 8;
}

int Message::from_bin(char * bobj){
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char * bin = _data;

    memcpy(&type, bin, 1);
    bin += 1;

    nick = bin;
    bin += 8;

    return 0;
}

void MovementMessage::to_bin(){
    Message::to_bin();

    char* bin = _data + 9; // Se salta la parte del tipo y el nick

    memcpy(bin, &x, sizeof(float));
    bin += sizeof(float);

    memcpy(bin, &y, sizeof(float));
    bin += sizeof(float);
}

int MovementMessage::from_bin(char * bobj){
    Message::from_bin(bobj);

    //Reconstruir la clase usando el buffer _data
    char * bin = _data + 9; // Se salta la parte del tipo y el nick

    memcpy(&x, bin, sizeof(float));
    bin += sizeof(float);

    memcpy(&y, bin, sizeof(float));
    bin += sizeof(float);

    return 0;
}


void IntMessage::to_bin(){
    Message::to_bin();

    char* bin = _data + 9; // Se salta la parte del tipo y el nick

    memcpy(bin, &i, sizeof(int));
    bin += sizeof(int);
}

int IntMessage::from_bin(char * bobj){
    Message::from_bin(bobj);

    //Reconstruir la clase usando el buffer _data
    char * bin = _data + 9; // Se salta la parte del tipo y el nick

    memcpy(&i, bin, sizeof(int));
    bin += sizeof(int);

    return 0;
}