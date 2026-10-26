#ifndef CLIENTCHANNEL_HPP
#define CLIENTCHANNEL_HPP
#include "Channel.hpp"

class ClientChannel : public Channel {
public:
    using Channel::Channel;
    void start() override   { channelSocket->connect(); }
    void stop() override    { channelSocket->shutdown(); }
    void send(const std::string& m) override { channelSocket->send(m); }
    void receive() override { channelSocket->receive(); }
};

#endif