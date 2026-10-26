#include "UDPSocket.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <stdexcept>


UDPSocket::UDPSocket(Role role, const std::string& ip, uint16_t port) : role_(role) {
    fd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_ < 0) throw std::runtime_error("socket() failed");

    if (role_ == Role::Server) {
        int yes = 1;
        setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);
        if (::bind(fd_, (sockaddr*)&addr, sizeof(addr)) < 0)
            throw std::runtime_error("bind() failed");
    } else {
        peer_.sin_family = AF_INET;
        peer_.sin_port = htons(port);
        if (inet_pton(AF_INET, ip.c_str(), &peer_.sin_addr) != 1)
            throw std::runtime_error("bad IP address");
    }
}

UDPSocket::~UDPSocket() { shutdown(); }

void UDPSocket::setReceiveTimeout(int ms) {
    timeval tv{};
    tv.tv_sec  = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

void UDPSocket::waitForConnect() {
    for (;;) {
        char buf[256];
        socklen_t len = sizeof(peer_);
        ssize_t n = ::recvfrom(fd_, buf, sizeof(buf), 0, (sockaddr*)&peer_, &len);
        if (n <= 0) throw ConnectionClosed("socket closed");
        if (std::string(buf, n) == "HELLO") break;
    }
    send("ACK");
}

void UDPSocket::connect() {
    setReceiveTimeout(1000);
    for (int attempt = 0; attempt < 60; ++attempt) {      // about 60 s of retries
        send("HELLO");
        try {
            receive();
            if (lastMessage_ == "ACK") { setReceiveTimeout(0); return; }
        } catch (const TimeoutError&) {
            // server not up yet, send HELLO again
        }
    }
    setReceiveTimeout(0);
    throw std::runtime_error("no response from the server");
}

void UDPSocket::send(const std::string& message) {
    if (::sendto(fd_, message.data(), message.size(), 0,
                 (sockaddr*)&peer_, sizeof(peer_)) < 0)
        throw std::runtime_error("sendto() failed");
}

void UDPSocket::receive() {
    char buf[256];
    sockaddr_in from{};
    socklen_t len = sizeof(from);
    ssize_t n = ::recvfrom(fd_, buf, sizeof(buf), 0, (sockaddr*)&from, &len);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) throw TimeoutError("receive timed out");
        throw ConnectionClosed("recvfrom() failed");
    }
    if (n == 0) throw ConnectionClosed("socket closed");
    if (role_ == Role::Server) peer_ = from;     // follow the client if it restarted on a new port
    lastMessage_.assign(buf, n);
}

void UDPSocket::shutdown() {
    if (fd_ >= 0) { ::shutdown(fd_, SHUT_RDWR); ::close(fd_); fd_ = -1; }
}