#include "../../common/ServerChannel.hpp"
#include "../../common/ServerLogic.hpp"
#include "../../common/TCPSocket.hpp"
#include "../../common/UDPSocket.hpp"
#include <iostream>
#include <sstream>

int main(int argc, char** argv) {
    if (argc < 3) { std::cerr << "usage: server_console tcp|udp <port>\n"; return 1; }
    std::string proto = argv[1];
    uint16_t port = static_cast<uint16_t>(std::stoi(argv[2]));

    Socket* sock = (proto == "tcp")
        ? static_cast<Socket*>(new TCPSocket(TCPSocket::Role::Server, "", port))
        : static_cast<Socket*>(new UDPSocket(UDPSocket::Role::Server, "", port));
    ServerChannel channel(sock);
    ServerLogic logic(channel);

    logic.onConnected     = []{ std::cout << "[server] client connected\n"; };
    logic.onTemperature   = [](double t){ std::cout << "[server] temp = " << t << "\n"; };
    logic.onThresholdSent = [](double t){ std::cout << "[server] threshold sent: " << t << "\n"; };
    logic.onDisconnected = []{ std::cout << "[server] client disconnected\n"; };

    std::thread worker([&]{
        try { logic.run(); } catch (const std::exception& e) { std::cerr << "[server] " << e.what() << "\n"; }
    });

    std::string line;                                  // type "t 40" to change the threshold
    while (std::getline(std::cin, line)) {
        std::istringstream in(line);
        std::string cmd; double v;
        if (in >> cmd >> v && cmd == "t") logic.setThreshold(v);
    }
    logic.requestStop();
    channel.stop();
    worker.join();
}