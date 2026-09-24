#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QFile>
#include <QDirIterator>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;



    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    qDebug() << "QM 1:"
             << QFile::exists(":/qt/qml/QML_Sig_Slot/i18n/qml_bn_BD.qm");

    qDebug() << "QM 2:"
             << QFile::exists(":/translations/qml_bn_BD.qm");

    qDebug() << "QM 3:"
             << QFile::exists(":/qml_bn_BD.qm");

    QDirIterator it(":/", QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString path = it.next();

        if (path.endsWith(".qm"))
            qDebug() << "QM RESOURCE:" << path;
    }
    engine.loadFromModule("QML_Sig_Slot", "Main");

    return QGuiApplication::exec();
}
