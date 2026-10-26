#ifndef TCPSOCKET_HPP
#define TCPSOCKET_HPP
#include "Socket.hpp"
#include <cstdint>

class TCPSocket : public Socket {
public:
    enum class Role { Server, Client };
    TCPSocket(Role role, const std::string& ip, uint16_t port);
    ~TCPSocket() override;
    void waitForConnect() override;
    void connect() override;
    void send(const std::string& message) override;
    void receive() override;
    void shutdown() override;
private:
    Role role_;
    std::string ip_;
    uint16_t port_;
    int listenFd_ = -1;
    int fd_ = -1;
    std::string buf_;   // TCP is a stream, so we frame messages with '\n'
};

#endif