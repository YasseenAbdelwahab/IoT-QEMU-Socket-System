#ifndef UDPSOCKET_HPP
#define UDPSOCKET_HPP
#include "Socket.hpp"
#include <cstdint>
#include <netinet/in.h>

class UDPSocket : public Socket {
public:
    enum class Role { Server, Client };
    UDPSocket(Role role, const std::string& ip, uint16_t port);
    ~UDPSocket() override;
    void waitForConnect() override;   // server: wait for client's HELLO
    void connect() override;          // client: send HELLO, wait for ACK
    void send(const std::string& message) override;
    void receive() override;
    void shutdown() override;
    void setReceiveTimeout(int ms) override;
private:
    Role role_;
    int fd_ = -1;
    sockaddr_in peer_{};              // server learns it, client sets it
};

#endif