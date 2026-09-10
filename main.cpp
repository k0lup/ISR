#include <QApplication>
#include <QCoreApplication>
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QMessageBox>
#include <QPalette>
#include <QStyleFactory>
#include <memory>


#include <Logger/logger.h>
#include "logging_categories.h"
#include "GUI/mainwindow.h"

static QPalette makeLightPalette()
{
    QPalette p;

    // Active / Normal
    p.setColor(QPalette::Window, Qt::white);
    p.setColor(QPalette::WindowText, Qt::black);
    p.setColor(QPalette::Base, Qt::white);
    p.setColor(QPalette::Text, Qt::black);
    p.setColor(QPalette::Button, QColor(240, 240, 240));
    p.setColor(QPalette::ButtonText, Qt::black);
    p.setColor(QPalette::Highlight, QColor(0, 120, 215));
    p.setColor(QPalette::HighlightedText, Qt::white);

    // Disabled
    p.setColor(QPalette::Disabled, QPalette::WindowText, QColor(160, 160, 160));
    p.setColor(QPalette::Disabled, QPalette::Text, QColor(160, 160, 160));
    p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(150, 150, 150));
    p.setColor(QPalette::Disabled, QPalette::Button, QColor(225, 225, 225));
    p.setColor(QPalette::Disabled, QPalette::Base, QColor(245, 245, 245));

    return p;
}

static void setupLogging()
{
    // 1) Правила фильтрации категорий/уровней
    // В проде обычно debug выключен, но можно включать точечно.
    QLoggingCategory::setFilterRules(R"(
        # Приложение
        app.*.debug=true
        app.*.info=true
        app.*.warning=true
        app.*.critical=true

        # Qt: только warning/critical
        qt.*.debug=false
        qt.*.info=false
        qt.*.warning=true
        qt.*.critical=true
    )");

    // 2) Конфиг логгера
    Logger::Config cfg;
    cfg.logDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/logs";
    cfg.baseFileName = "app.log";

    cfg.rotateBytes = 10 * 1024 * 1024; // 10MB
    cfg.keepDays  = 7;
    cfg.keepFiles = 20;

    cfg.flushIntervalMs = 300;
    cfg.ringBufferLines = 2000;
    cfg.maxQueue = 50000;

    cfg.alsoStdErr = true;            // консоль
    cfg.windowsDebugOutput = true;    // Windows debug output (в debug)

    // 3) Запускаем writer thread
    Logger::instance().start(cfg);

    // 4) Включаем перехват qDebug/qInfo/... (ВАЖНО: после start)
    Logger::installQtHandler();

    qCInfo(logCore) << "Логгер инициализирован. Директория =" << cfg.logDir;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion")); // 1) сначала стиль
    app.setPalette(makeLightPalette());            // 2) потом палитра

    setupLogging();

    qCInfo(logCore) << "Приложение запускается";

    MainWindow mw;
    mw.show();

    const int rc = app.exec();

    qCInfo(logCore) << "Приложение завершается, код завершения=" << rc;

    // Корректное завершение логгера
    Logger::uninstallQtHandler();
    Logger::instance().stop();

    return rc;
}


/*
    // Корректное завершение логгера
    Logger::uninstallQtHandler();
    Logger::instance().stop();

    //чтобы смогли выйти без запуска app.exec()
    QApplication::closeAllWindows();
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
*/
