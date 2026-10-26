#pragma once
#include <QObject>
#include <QString>
#include "ServerWorker.hpp"

class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(double  temperature READ temperature NOTIFY temperatureChanged)
    Q_PROPERTY(double  threshold   READ threshold   NOTIFY thresholdChanged)
    Q_PROPERTY(bool    ledOn       READ ledOn       NOTIFY ledChanged)
    Q_PROPERTY(bool    running     READ running     NOTIFY runningChanged)
    Q_PROPERTY(QString status      READ status      NOTIFY statusChanged)

public:
    explicit Backend(QObject* parent = nullptr) : QObject(parent) {
        connect(&worker_, &ServerWorker::connected, this,
                [this] { setStatus("Client connected"); });
        connect(&worker_, &ServerWorker::temperature, this, [this](double t) {
            temp_ = t;  haveTemp_ = true;
            emit temperatureChanged();
            emit sample(t);                       // QML appends it to the graph
            updateLed();
        });
        connect(&worker_, &ServerWorker::thresholdSent, this, [this](double t) {
            threshold_ = t;
            emit thresholdChanged();
            setStatus(QString("Threshold sent: %1 °C").arg(t));
            updateLed();
        });
        connect(&worker_, &ServerWorker::error, this, [this](const QString& m) {
            running_ = false;  emit runningChanged();
            setStatus("Error: " + m);
        });
          connect(&worker_, &ServerWorker::disconnected, this,
          [this] { setStatus("Client disconnected, waiting for a new one..."); });
    }

    double  temperature() const { return temp_; }
    double  threshold()   const { return threshold_; }
    bool    ledOn()       const { return ledOn_; }
    bool    running()     const { return running_; }
    QString status()      const { return status_; }

    Q_INVOKABLE void start(bool tcp, int port, double initialThreshold) {
        haveTemp_ = false;
        threshold_ = initialThreshold;  emit thresholdChanged();
        updateLed();
        worker_.start(tcp, static_cast<quint16>(port), initialThreshold);
        running_ = true;  emit runningChanged();
        setStatus("Waiting for client...");
    }
    Q_INVOKABLE void stop() {
        worker_.stop();
        running_ = false;  emit runningChanged();
        setStatus("Stopped");
    }
    Q_INVOKABLE void applyThreshold(double t) { worker_.setThreshold(t); }

signals:
    void temperatureChanged();
    void thresholdChanged();
    void ledChanged();
    void runningChanged();
    void statusChanged();
    void sample(double value);

private:
    void updateLed() {
        // LED rule: change this ONE line if your instructor wants the opposite
        ledOn_ = haveTemp_ && temp_ >= threshold_;
        emit ledChanged();
    }
    void setStatus(const QString& s) { status_ = s; emit statusChanged(); }

    ServerWorker worker_;
    double temp_ = 0, threshold_ = 30;
    bool haveTemp_ = false, ledOn_ = false, running_ = false;
    QString status_ = "Idle";
};