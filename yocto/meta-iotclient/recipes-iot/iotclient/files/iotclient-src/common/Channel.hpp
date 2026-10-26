#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include "Socket.hpp"
#include <string>


class Channel {
public:
    explicit Channel(Socket* s) : channelSocket(s) {}
    virtual ~Channel() { delete channelSocket; }   // Channel owns the socket

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void send(const std::string& message) = 0;
    virtual void receive() = 0;

    const std::string& lastMessage() const { return channelSocket->lastMessage(); }
    void setReceiveTimeout(int ms) { channelSocket->setReceiveTimeout(ms); }

protected:
    Socket* channelSocket;
};

#endif