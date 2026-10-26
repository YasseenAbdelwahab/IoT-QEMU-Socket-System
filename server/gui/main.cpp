#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include "Backend.hpp"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Fusion");

    // usage: server_gui [tcp|udp] [port]
    const QStringList args = app.arguments();
    int  protoIndex = 0;          // 0 = TCP, 1 = UDP
    int  port = 5000;
    bool autoStart = false;

    if (args.size() >= 2) {
        const QString p = args[1].toLower();
        if (p != "tcp" && p != "udp") {
            qWarning("usage: server_gui [tcp|udp] [port]");
            return 1;
        }
        protoIndex = (p == "udp") ? 1 : 0;
        autoStart = true;
    }
    if (args.size() >= 3) {
        bool ok = false;
        const int v = args[2].toInt(&ok);
        if (!ok || v < 1024 || v > 65535) {
            qWarning("port must be a number between 1024 and 65535");
            return 1;
        }
        port = v;
    }

    Backend backend;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("backend", &backend);
    engine.rootContext()->setContextProperty("startProtoIndex", protoIndex);
    engine.rootContext()->setContextProperty("startPort", port);
    engine.rootContext()->setContextProperty("autoStart", autoStart);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, [] { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    engine.loadFromModule("ServerGui", "Main");
    return app.exec();
}
