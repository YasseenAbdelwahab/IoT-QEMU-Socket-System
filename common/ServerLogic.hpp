#ifndef SERVERLOGIC_HPP
#define SERVERLOGIC_HPP
#include "Channel.hpp"
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <string>
#include <thread>


class ServerLogic {
public:
    explicit ServerLogic(Channel& ch) : ch_(ch) {}

    void setThreshold(double t) {                 // thread-safe
        std::lock_guard<std::mutex> lk(m_);
        pending_ = t;
        changed_ = true;
    }
    void requestStop() { running_ = false; }

    void run() {                                  // blocking: run it in its own thread
        while (running_) {
            try {
                ch_.setReceiveTimeout(0);         // waiting for a client may take forever
                ch_.start();
                if (!running_) break;
                ch_.setReceiveTimeout(kReplyTimeoutMs);
                resendThreshold();                // every new client gets the current threshold first
                if (onConnected) onConnected();
                session();
            } catch (const ConnectionClosed&) {
                if (!running_) break;
                if (onDisconnected) onDisconnected();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

    std::function<void()> onConnected;
    std::function<void()> onDisconnected;
    std::function<void(double)> onTemperature;
    std::function<void(double)> onThresholdSent;

private:
    static constexpr int kReplyTimeoutMs = 30000;   // UDP: how long to wait for a temperature reply

    void resendThreshold() {
        std::lock_guard<std::mutex> lk(m_);
        changed_ = true;                          // pending_ still holds the last value
    }

    void session() {
        while (running_) {
            auto next = std::chrono::steady_clock::now() + std::chrono::seconds(1);

            bool send = false;
            double t = 0;
            {
                std::lock_guard<std::mutex> lk(m_);
                if (changed_) { send = true; t = pending_; changed_ = false; }
            }

            if (send) {
                ch_.send("set threshold");
                ch_.send(std::to_string(t));
                if (onThresholdSent) onThresholdSent(t);
            } else {
                ch_.send("get Temp");
                try {
                    ch_.receive();                // blocks while the user types a value
                } catch (const TimeoutError&) {
                    continue;                     // reply lost, try again
                }
                const std::string msg = ch_.lastMessage();
                if (msg == "HELLO") {             // a UDP client (re)started
                    ch_.send("ACK");
                    resendThreshold();
                } else {
                    try {
                        double v = std::stod(msg);
                        if (onTemperature) onTemperature(v);
                    } catch (const std::exception&) {
                        // ignore a malformed reply
                    }
                }
            }
            std::this_thread::sleep_until(next);
        }
    }

    Channel& ch_;
    std::atomic<bool> running_{true};
    std::mutex m_;
    double pending_ = 30.0;
    bool changed_ = true;
};

#endif