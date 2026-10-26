#ifndef SOCKET_HPP
#define SOCKET_HPP
#include <string>
#include <stdexcept>

struct TimeoutError : std::runtime_error {
    using std::runtime_error::runtime_error;
};
struct ConnectionClosed : std::runtime_error {
    using std::runtime_error::runtime_error;
};

class Socket {
public:
    virtual ~Socket() = default;
    virtual void waitForConnect() = 0;
    virtual void connect() = 0;
    virtual void send(const std::string& message) = 0;
    virtual void receive() = 0;
    virtual void shutdown() = 0;
    virtual void setReceiveTimeout(int /*ms*/) {}   // 0 = wait forever; TCP ignores it
    const std::string& lastMessage() const { return lastMessage_; }
protected:
    std::string lastMessage_;
};

#endif