#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QFontDatabase>
#include <QFont>
#include <QLoggingCategory>
#include <cstdio>
#include <memory>
#include <dlt/dlt.h>

#include "backend/VehicleSimulator.h"
#include "backend/VehicleBackend.h"
#include "backend/ClimateBackend.h"
#include "backend/MediaBackend.h"
#include "backend/NavigationBackend.h"
#include "backend/PhoneBackend.h"
#include "backend/SystemBackend.h"
#include <QtWebEngineQuick/qtwebenginequickglobal.h>

DLT_DECLARE_CONTEXT(g_dltContext);

namespace {

void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    Q_UNUSED(context);

    const QByteArray utf8Message = message.toUtf8();
    const char *level = "DEBUG";
    switch (type) {
    case QtDebugMsg:
        level = "DEBUG";
        DLT_LOG(g_dltContext, DLT_LOG_DEBUG, DLT_STRING(utf8Message.constData()));
        break;
    case QtInfoMsg:
        level = "INFO";
        DLT_LOG(g_dltContext, DLT_LOG_INFO, DLT_STRING(utf8Message.constData()));
        break;
    case QtWarningMsg:
        level = "WARN";
        DLT_LOG(g_dltContext, DLT_LOG_WARN, DLT_STRING(utf8Message.constData()));
        break;
    case QtCriticalMsg:
        level = "ERROR";
        DLT_LOG(g_dltContext, DLT_LOG_ERROR, DLT_STRING(utf8Message.constData()));
        break;
    case QtFatalMsg:
        level = "FATAL";
        DLT_LOG(g_dltContext, DLT_LOG_FATAL, DLT_STRING(utf8Message.constData()));
        break;
    }
    std::fprintf(stderr, "[Qt][%s] %s\n", level, utf8Message.constData());
    std::fflush(stderr);
}

}

int main(int argc, char *argv[])
{
    DLT_REGISTER_APP("IVI", "Head Vision IVI");
    DLT_REGISTER_CONTEXT(g_dltContext, "QT", "Qt application logging");
    qInstallMessageHandler(qtMessageHandler);
    qInfo("Head Vision IVI starting");

    // High DPI and performance flags for Raspberry Pi / desktop
    QGuiApplication::setApplicationName("Otras IVI");
    QGuiApplication::setOrganizationName("Dextris");
    QGuiApplication::setOrganizationDomain("Dextris.in");

    QtWebEngineQuick::initialize();
    qInfo("Qt WebEngine initialized");

    QGuiApplication app(argc, argv);
    qInfo("Qt application created; DISPLAY=%s", qgetenv("DISPLAY").constData());

    // Dark style for Controls
    QQuickStyle::setStyle("Basic");

    // Load bundled Inter fonts into Qt application font database
    QFontDatabase::addApplicationFont(":/qt/qml/HeadVision/qml/assets/fonts/Inter-Regular.ttf");
    QFontDatabase::addApplicationFont(":/qt/qml/HeadVision/qml/assets/fonts/Inter-Medium.ttf");
    QFontDatabase::addApplicationFont(":/qt/qml/HeadVision/qml/assets/fonts/Inter-SemiBold.ttf");
    QFontDatabase::addApplicationFont(":/qt/qml/HeadVision/qml/assets/fonts/Inter-Bold.ttf");
    QFontDatabase::addApplicationFont(":/HeadVision/qml/assets/fonts/Inter-Regular.ttf");
    QFontDatabase::addApplicationFont(":/HeadVision/qml/assets/fonts/Inter-Medium.ttf");
    QFontDatabase::addApplicationFont(":/HeadVision/qml/assets/fonts/Inter-SemiBold.ttf");
    QFontDatabase::addApplicationFont(":/HeadVision/qml/assets/fonts/Inter-Bold.ttf");

    // Set Inter as global UI default typeface
    QFont interFont("Inter", 16);
    interFont.setStyleStrategy(QFont::PreferAntialias);
    QGuiApplication::setFont(interFont);

    // Instantiate simulation and IVI backend services
    auto simulator = std::make_unique<VehicleSimulator>();
    auto vehicleBackend = std::make_unique<VehicleBackend>(simulator.get());
    auto climateBackend = std::make_unique<ClimateBackend>();
    auto mediaBackend = std::make_unique<MediaBackend>(simulator.get());
    auto navigationBackend = std::make_unique<NavigationBackend>(simulator.get());
    auto phoneBackend = std::make_unique<PhoneBackend>();
    auto systemBackend = std::make_unique<SystemBackend>();

    QQmlApplicationEngine engine;

    // Register backend services into QML context
    QQmlContext *rootContext = engine.rootContext();
    rootContext->setContextProperty("VehicleBackend", vehicleBackend.get());
    rootContext->setContextProperty("ClimateBackend", climateBackend.get());
    rootContext->setContextProperty("MediaBackend", mediaBackend.get());
    rootContext->setContextProperty("NavigationBackend", navigationBackend.get());
    rootContext->setContextProperty("PhoneBackend", phoneBackend.get());
    rootContext->setContextProperty("SystemBackend", systemBackend.get());

    const QUrl url(QStringLiteral("qrc:/HeadVision/qml/Main.qml"));
    
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) {
                qCritical("Failed to load the root QML document: %s", qPrintable(url.toString()));
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection);

    engine.load(url);
    qInfo("Root QML load requested: %s", qPrintable(url.toString()));

    const int exitCode = app.exec();
    qInfo("Head Vision IVI stopped with exit code %d", exitCode);
    DLT_UNREGISTER_CONTEXT(g_dltContext);
    DLT_UNREGISTER_APP();
    return exitCode;
}
