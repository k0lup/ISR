#include "configloader.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

namespace {
QString cleanValue(QString value)
{
    value = value.trimmed();
    if (value.startsWith('"') && value.endsWith('"') && value.size() >= 2) {
        value = value.mid(1, value.size() - 2);
    }
    return value.trimmed();
}
}

bool ConfigLoader::loadFromArgs(const QStringList &args, AppConfig &config, QString &error) const
{
    if (args.size() < 3) {
        error = QStringLiteral("Использование: ISR <user.on> <config.cfg> [checkpoints]");
        return false;
    }

    config.userOnPath = args.at(1);
    config.prisCfgPath = args.at(2);
    config.checkpointsEnabled = args.size() > 3;

    if (!QFileInfo::exists(config.userOnPath)) {
        error = QStringLiteral("Не найден файл .on: %1").arg(config.userOnPath);
        return false;
    }
    if (!QFileInfo::exists(config.prisCfgPath)) {
        error = QStringLiteral("Не найден файл cfg: %1").arg(config.prisCfgPath);
        return false;
    }

    if (!loadPrisConfig(config.prisCfgPath, config, error)) {
        return false;
    }
    if (!loadUserOn(config.userOnPath, config, error)) {
        return false;
    }

    return config.isValid(&error);
}

bool ConfigLoader::loadPrisConfig(const QString &path, AppConfig &config, QString &error) const
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error = QStringLiteral("Не удалось открыть cfg: %1").arg(path);
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (!line.contains('=')) {
            continue;
        }

        const QString key = line.section('=', 0, 0).trimmed().toUpper();
        const QString value = cleanValue(line.section('=', 1));

        if (key == QStringLiteral("РАЗДЕЛ")) {
            config.razdel = value;
        } else if (key == QStringLiteral("ПФКС")) {
            config.pfksPath = value;
        }
    }

    return true;
}

bool ConfigLoader::loadUserOn(const QString &path, AppConfig &config, QString &error) const
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error = QStringLiteral("Не удалось открыть .on: %1").arg(path);
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (!line.contains('=')) {
            continue;
        }

        const QString key = line.section('=', 0, 0).trimmed().toUpper();
        const QString value = cleanValue(line.section('=', 1));

        if (key == QStringLiteral("РАЗМЕЩЕНИЕ_СПО")) config.spoRoot = value;
        else if (key == QStringLiteral("РАЗМЕЩЕНИЕ_НАСТРОЕК")) config.settingsRoot = value;
        else if (key == QStringLiteral("РАЗМЕЩЕНИЕ_ФАЙЛОВ")) config.filesRoot = value;
        else if (key == QStringLiteral("РАЗМЕЩЕНИЕ_РАЗДЕЛОВ")) config.sectionsRoot = value;
        else if (key == QStringLiteral("ПРОТОКОЛ")) config.protocolPath = value;
        else if (key == QStringLiteral("ПРОГРАММЫ")) config.programs.append(splitMultiValue(value));
        else if (key == QStringLiteral("РАЗДЕЛЫ")) config.sections.append(splitMultiValue(value));
        else if (key == QStringLiteral("НШС_ИЗД") || key == QStringLiteral("АВАР_ИЗД") ||
                 key == QStringLiteral("ПРИ_ИЗД") || key == QStringLiteral("НШС_РЭП") ||
                 key == QStringLiteral("АВАР_РЭП") || key == QStringLiteral("ПРИ_РЭП")) {
            config.edDocs.insert(key, value);
        }
    }

    config.programs.removeDuplicates();
    config.sections.removeDuplicates();
    return true;
}

QStringList ConfigLoader::splitMultiValue(const QString &value)
{
    QStringList result;
    for (const QString &part : value.split(';', Qt::SkipEmptyParts)) {
        const QString trimmed = part.trimmed();
        if (!trimmed.isEmpty()) {
            result << trimmed;
        }
    }
    if (result.isEmpty() && !value.trimmed().isEmpty()) {
        result << value.trimmed();
    }
    return result;
}
