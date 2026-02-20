#include "startup.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

static QString pickFile(QWidget* parent, const QString& title)
{
    // Можно настроить фильтры под твои расширения
    return QFileDialog::getOpenFileName(
        parent,
        title,
        QString(),
        "Text files (*.txt *.cfg *.ini);;All files (*.*)"
    );
}

static bool ensureFileExists(const QString& path)
{
    return !path.isEmpty() && QFileInfo::exists(path) && QFileInfo(path).isFile();
}

bool resolveStartupPaths(QApplication& app, StartupPaths& out, QString* errorMsg)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("App with settings/config files");
    parser.addHelpOption();

    // аргументы: --settings / --config
    QCommandLineOption settingsOpt({"s", "settings"}, "Путь к файлу настроек", "path");
    QCommandLineOption configOpt({"c", "config"}, "Путь к файлу конфигурации", "path");

    parser.addOption(settingsOpt);
    parser.addOption(configOpt);
    parser.process(app);

    out.settingsPath = parser.value(settingsOpt);
    out.configPath   = parser.value(configOpt);

    // Если путь не передан — попросим через диалог
    if (out.settingsPath.isEmpty()) {
        out.settingsPath = pickFile(nullptr, "Выберите файл настроек");
        if (out.settingsPath.isEmpty()) {
            if (errorMsg) *errorMsg = "Файл настроек не выбран.";
            return false;
        }
    }

    if (out.configPath.isEmpty()) {
        out.configPath = pickFile(nullptr, "Выберите файл конфигурации");
        if (out.configPath.isEmpty()) {
            if (errorMsg) *errorMsg = "Файл конфигурации не выбран.";
            return false;
        }
    }

    // Проверим существование
    if (!ensureFileExists(out.settingsPath)) {
        if (errorMsg) *errorMsg = QString("Файл настроек не найден: %1").arg(out.settingsPath);
        return false;
    }

    if (!ensureFileExists(out.configPath)) {
        if (errorMsg) *errorMsg = QString("Файл конфигурации не найден: %1").arg(out.configPath);
        return false;
    }

    return true;
}
