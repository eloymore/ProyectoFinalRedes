#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "Serializable.h"
#include <unistd.h>
#include <memory>

class Message : public Serializable {
public:
    static const size_t MESSAGE_SIZE = sizeof(char) * 8 + sizeof(uint8_t) + 63 * sizeof(char); // Nick + tipo + 63 bytes extra

    enum MessageType
    {
        LOGIN   = 0,
        CONNREFUSED,
        MOVEMENT,
        CLICK,
        SCORE,
        VELOCITY,
        TURN,
        LOGOUT
    };

    Message(){}

    Message(std::string nick, u_int8_t type) : nick(nick), type(type){}

    virtual void to_bin();

    virtual int from_bin(char * bobj);

    uint8_t type;
    std::string nick;
    std::string msgData;
};

class MovementMessage : public Message{
public:
    MovementMessage(){}

    MovementMessage(std::string nick, float x, float y) : Message(nick, MOVEMENT), x(x), y(y){}

    void to_bin() override;

    int from_bin(char * bobj) override;

    float x, y;
};

class IntMessage : public Message{
public:
    IntMessage(){}

    IntMessage(std::string nick, u_int8_t type, int i): Message(nick, type), i(i){}

    void to_bin() override;

    int from_bin(char * bobj) override;

    int i;
};

class ClickMessage : public IntMessage{
public:
    ClickMessage(){}

    ClickMessage(std::string nick, int click) : IntMessage(nick, CLICK, click){}
};

class ScoreMessage : public IntMessage{
public:
    ScoreMessage(){}

    ScoreMessage(std::string nick, int score) : IntMessage(nick, SCORE, score){}
};

class VelocityMessage: public Message{
    public:
    VelocityMessage(){}

    VelocityMessage(std::string nick, float velocityP) : Message(nick, VELOCITY), velocity(velocityP) {}
    
    void to_bin() override;

    int from_bin(char * bobj) override;

    float velocity;
};

#endif