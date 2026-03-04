#include <QApplication>
#include <QCoreApplication>
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QMessageBox>
#include <memory>

#include "gui/isrmainwindow.h"
#include "config/config_service.h"
#include "config/startup.h"
#include "logger/logger.h"
#include "logger/logging_categories.h"

bool resolveStartupPaths(QApplication& app, StartupPaths& out, QString* errorMsg);

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

    setupLogging();

    StartupPaths paths;
    QString startErr;
    if (!resolveStartupPaths(app, paths, &startErr)) {
        QMessageBox::warning(nullptr, "Startup", startErr);
        qCWarning(logCore) << QString("Ошибка загрузки данных из входных файлов: %1.\nПриложение завершается.").arg(startErr);

        Logger::uninstallQtHandler();
        Logger::instance().stop();

        //чтобы смогли выйти без запуска app.exec()
        QApplication::closeAllWindows();
        QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
        return 1;
    }

    // грузим конфиг
    ConfigLoader::Options opt;
    opt.allowComments = true;
    opt.strict = true; // хочешь строгий формат — true

    ConfigService service{ ConfigLoader{opt} };

    AppConfig cfg;
    ConfigError cfgErr;

    if (!service.load(paths.settingsPath, paths.configPath, cfg, &cfgErr)) {
        QMessageBox::critical(nullptr, "Config error", cfgErr.toString());
        qCWarning(logCore) << QString("Ошибка загрузки данных из входных файлов: %1.\nПриложение завершается.").arg(cfgErr.toString());

        // Корректное завершение логгера
        Logger::uninstallQtHandler();
        Logger::instance().stop();

        //чтобы смогли выйти без запуска app.exec()
        QApplication::closeAllWindows();
        QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
        return 1;
    }

    // раздаём конфиг UI (удобно как shared_ptr<const AppConfig>)
    auto cfgPtr = std::make_shared<const AppConfig>(cfg);

    ISRMainWindow w(cfgPtr);
    w.show();

    qCInfo(logCore) << "Приложение запускается";

    const int rc = app.exec();

    qCInfo(logCore) << "Приложение завершается, код завершения=" << rc;

    // Корректное завершение логгера
    Logger::uninstallQtHandler();
    Logger::instance().stop();

    return rc;
}
