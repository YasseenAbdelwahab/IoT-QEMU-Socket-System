#include "TCPSocket.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>


TCPSocket::TCPSocket(Role role, const std::string& ip, uint16_t port)
    : role_(role), ip_(ip), port_(port) {
    if (role_ == Role::Server) {
        listenFd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (listenFd_ < 0) throw std::runtime_error("socket() failed");
        int yes = 1;
        setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port_);
        if (::bind(listenFd_, (sockaddr*)&addr, sizeof(addr)) < 0)
            throw std::runtime_error("bind() failed");
        if (::listen(listenFd_, 1) < 0)
            throw std::runtime_error("listen() failed");
    }
}

TCPSocket::~TCPSocket() { shutdown(); }

void TCPSocket::waitForConnect() {
    if (fd_ >= 0) { ::close(fd_); fd_ = -1; buf_.clear(); }   // drop the previous client
    fd_ = ::accept(listenFd_, nullptr, nullptr);
    if (fd_ < 0) throw ConnectionClosed("accept() failed");
}

void TCPSocket::connect() {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    if (inet_pton(AF_INET, ip_.c_str(), &addr.sin_addr) != 1)
        throw std::runtime_error("bad IP address");

    for (int attempt = 0; attempt < 60; ++attempt) {   // keep trying while the server boots
        fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ < 0) throw std::runtime_error("socket() failed");
        if (::connect(fd_, (sockaddr*)&addr, sizeof(addr)) == 0) return;
        ::close(fd_);
        fd_ = -1;
        ::sleep(1);
    }
    throw std::runtime_error("could not reach the server");
}

void TCPSocket::send(const std::string& message) {
    std::string data = message + "\n";
    size_t sent = 0;
    while (sent < data.size()) {
        ssize_t n = ::send(fd_, data.data() + sent, data.size() - sent, MSG_NOSIGNAL);
        if (n <= 0) throw ConnectionClosed("send() failed");
        sent += n;
    }
}

void TCPSocket::receive() {
    size_t pos;
    while ((pos = buf_.find('\n')) == std::string::npos) {
        char tmp[256];
        ssize_t n = ::recv(fd_, tmp, sizeof(tmp), 0);
        if (n <= 0) throw ConnectionClosed("connection closed");
        buf_.append(tmp, n);
    }
    lastMessage_ = buf_.substr(0, pos);
    buf_.erase(0, pos + 1);
}

void TCPSocket::shutdown() {
    if (fd_ >= 0)       { ::shutdown(fd_, SHUT_RDWR);       ::close(fd_);       fd_ = -1; }
    if (listenFd_ >= 0) { ::shutdown(listenFd_, SHUT_RDWR); ::close(listenFd_); listenFd_ = -1; }
}