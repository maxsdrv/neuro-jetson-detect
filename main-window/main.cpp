#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "GWindow.h"

int main(int argc, char* argv[]) {

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<GWindow>("custom.window.module", 1, 0, "GWindow");

    const QUrl url(QStringLiteral("qrc:/qml/window.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return QGuiApplication::exec();
}
