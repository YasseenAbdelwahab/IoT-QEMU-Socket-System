#ifndef SERVERCHANNEL_HPP
#define SERVERCHANNEL_HPP
#include "Channel.hpp"

class ServerChannel : public Channel {
public:
    using Channel::Channel;
    void start() override   { channelSocket->waitForConnect(); }
    void stop() override    { channelSocket->shutdown(); }
    void send(const std::string& m) override { channelSocket->send(m); }
    void receive() override { channelSocket->receive(); }
};

#endif