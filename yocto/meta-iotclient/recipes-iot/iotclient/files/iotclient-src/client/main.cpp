#include "../common/ClientChannel.hpp"
#include "../common/TCPSocket.hpp"
#include "../common/UDPSocket.hpp"
#include <iostream>
#include <memory>
#include <string>

static const char* led(double temp, double threshold) {
    return temp >= threshold ? "ON" : "OFF";
}

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "usage: client tcp|udp <server-ip> <port>\n";
        return 1;
    }
    std::string proto = argv[1], ip = argv[2];
    uint16_t port = static_cast<uint16_t>(std::stoi(argv[3]));

    Socket* sock = (proto == "tcp")
        ? static_cast<Socket*>(new TCPSocket(TCPSocket::Role::Client, ip, port))
        : static_cast<Socket*>(new UDPSocket(UDPSocket::Role::Client, ip, port));
    ClientChannel channel(sock);

    try {
        std::cout << "Attempting to connect to server...\n";
        channel.start();
        std::cout << "Connected. Awaiting threshold setting...\n";

        double threshold = 0, lastTemp = -1000;   // -1000 = "no reading yet" -> LED OFF

        for (;;) {
            channel.receive();
            std::string cmd = channel.lastMessage();
            if (cmd == "ACK") continue;      // stray handshake reply, ignore
            std::cout << "Received command: " << cmd << "\n";

            if (cmd == "get Temp") {
                std::cout << "Enter temperature: " << std::flush;
                std::string line;
                if (!std::getline(std::cin, line)) break;
                lastTemp = std::stod(line);
                std::cout << "Sending temperature: " << lastTemp << "°C\n";
                channel.send(std::to_string(lastTemp));
                std::cout << "LED Status: " << led(lastTemp, threshold)
          << " (temp " << lastTemp << " / threshold " << threshold << ")\n";
            } else if (cmd == "set threshold") {
                channel.receive();                        // the value follows the command
                threshold = std::stod(channel.lastMessage());
                std::cout << "Received new threshold: " << threshold
                          << "°C - LED Status: " << led(lastTemp, threshold) << "\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    channel.stop();
}