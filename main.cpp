#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include"GLVideoWidget.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    qDebug()<< qmlRegisterType<GLVideoWidget>("AvCom", 1, 0, "GLVideoWidget");

    engine.loadFromModule("test", "Main");

    return app.exec();
}
