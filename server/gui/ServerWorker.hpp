#pragma once
#include <QObject>
#include <QString>
#include <atomic>
#include <memory>
#include <thread>
#include "../../common/ServerChannel.hpp"
#include "../../common/ServerLogic.hpp"
#include "../../common/TCPSocket.hpp"
#include "../../common/UDPSocket.hpp"

class ServerWorker : public QObject {
    Q_OBJECT
public:
    explicit ServerWorker(QObject* parent = nullptr) : QObject(parent) {}
    ~ServerWorker() override { stop(); }

    void start(bool tcp, quint16 port, double initialThreshold) {
        stop();
        Socket* s = tcp
            ? static_cast<Socket*>(new TCPSocket(TCPSocket::Role::Server, "", port))
            : static_cast<Socket*>(new UDPSocket(UDPSocket::Role::Server, "", port));
        channel_ = std::make_unique<ServerChannel>(s);
        logic_   = std::make_unique<ServerLogic>(*channel_);
        logic_->setThreshold(initialThreshold);
        logic_->onConnected     = [this]            { emit connected(); };
        logic_->onTemperature   = [this](double t)  { emit temperature(t); };
        logic_->onThresholdSent = [this](double t)  { emit thresholdSent(t); };
        logic_->onDisconnected = [this] { emit disconnected(); };

        thread_ = std::thread([this] {
            try { logic_->run(); }
            catch (const std::exception& e) {
                if (!stopping_) emit error(QString::fromUtf8(e.what()));
            }
        });
    }

    void setThreshold(double t) { if (logic_) logic_->setThreshold(t); }

    void stop() {
        stopping_ = true;
        if (logic_)   logic_->requestStop();
        if (channel_) channel_->stop();           // wakes any blocked accept/recv
        if (thread_.joinable()) thread_.join();   // can take up to ~1 s
        logic_.reset();
        channel_.reset();
        stopping_ = false;
    }

signals:
    void connected();
    void temperature(double t);
    void thresholdSent(double t);
    void disconnected();
    void error(const QString& message);

private:
    std::unique_ptr<ServerChannel> channel_;
    std::unique_ptr<ServerLogic>   logic_;
    std::thread thread_;
    std::atomic<bool> stopping_{false};
};