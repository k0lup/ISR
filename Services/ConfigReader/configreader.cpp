#include "configreader.h"
#include <QFile>

#include "logger/logging_categories.h"
#include <QLoggingCategory>

ConfigReader::ConfigReader(QObject* parent) : QObject(parent)
{
    qCInfo(logCore) << "Вызван конструктор ConfigReader";
}

void ConfigReader::onReadFileRequested(const QString& cfg_file_path, const QString& on_file_path,
                                       const QString& service_on_file_path)
{
    qCInfo(logCore) << QString("Получил запрос на чтение входных файлов конфигурации:\n"
                       "'.cfg' - '%1'\n"
                       "'.on' - '%2'\n"
                       "'.on_s284' - '%3'").arg(cfg_file_path).arg(on_file_path).arg(service_on_file_path);
    QFile cfg_file(cfg_file_path);
    QFile on_file(on_file_path);
    QFile service_file(service_on_file_path);

    if (!cfg_file.open(QIODevice::Text | QIODevice::ReadOnly)) {
        QString error_message;
        error_message = QString("Не удалось открыть файл .cfg '%1' - %2").arg(cfg_file_path).arg(cfg_file.errorString());
        qCWarning(logCore) << error_message;
        qCWarning(logCore) << "Отправляем сигнал failed";
        emit failed(error_message);
        return;
    }


    if (!on_file.open(QIODevice::Text | QIODevice::ReadOnly)) {

    }
    if (!service_file.open(QIODevice::Text | QIODevice::ReadOnly)) {

    }

    qCInfo(logCore) << QString("Успещно закончили чтение входных файлов конфигурации");
}
