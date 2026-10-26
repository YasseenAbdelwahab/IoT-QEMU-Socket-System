#include "../common/TCPSocket.hpp"
#include "../common/UDPSocket.hpp"
#include <iostream>
#include <memory>

int main(int argc, char** argv) {
    if (argc < 3) { std::cerr << "usage: echo_test server|client tcp|udp\n"; return 1; }
    bool server = std::string(argv[1]) == "server";
    bool tcp    = std::string(argv[2]) == "tcp";
    std::unique_ptr<Socket> s;   // polymorphism: same code for both protocols
    if (tcp) s = std::make_unique<TCPSocket>(server ? TCPSocket::Role::Server : TCPSocket::Role::Client, "127.0.0.1", 5000);
    else     s = std::make_unique<UDPSocket>(server ? UDPSocket::Role::Server : UDPSocket::Role::Client, "127.0.0.1", 5000);

    if (server) { s->waitForConnect(); s->send("set threshold"); s->receive(); }
    else        { s->connect(); s->receive(); std::cout << "got: " << s->lastMessage() << "\n"; s->send("ok"); }
    if (server) std::cout << "client said: " << s->lastMessage() << "\n";
}