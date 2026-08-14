#include "configreader.h"

#include "configschema.h"
#include "configtypes.h"

#include <QFile>
#include <QTextStream>
#include <QByteArray>

#include "Services/EncodingDetector/encodingdetector.h"

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


    QStringList files_paths = {cfg_file_path, on_file_path, service_on_file_path};
    QMap<QString, QStringList> params;

    for (const auto& file_path : files_paths) {
        qCDebug(logCore) << QString("Читаем файл '%1'").arg(file_path);
        QStringList cur_file_error_messages;
        QStringList lines = readFile(file_path, cur_file_error_messages);
        if (!cur_file_error_messages.isEmpty()) {
            QString error_message = QString("Возникли ошибки при чтении файла '%1':\n%2").arg(file_path).arg(cur_file_error_messages.join("\n"));
            qCWarning(logCore) << error_message;
            qCWarning(logCore) << "Отправляем сигнал failed";
            emit failed(error_message);
            return;
        }
        qCDebug(logCore) << QString("Прочитали строки из файла '%1'").arg(file_path);

        QMap<QString, QStringList> cur_file_params = parser_.parseData(lines, cur_file_error_messages);
        if (!cur_file_error_messages.isEmpty()) {
            QString error_message = QString("Возникли ошибки при чтении файла '%1':\n%2").arg(file_path).arg(cur_file_error_messages.join("\n"));
            qCWarning(logCore) << error_message;
            qCWarning(logCore) << "Отправляем сигнал failed";
            emit failed(error_message);
            return;
        }

        qCDebug(logCore) << QString("Распрасили файл '%1'").arg(file_path);

        for (auto it = cur_file_params.constBegin(); it != cur_file_params.constEnd(); ++it) {
            params[it.key()].append(it.value());
        }
    }

    config_reader::ConfigSchema schema = config_reader::createConfigShema();

    qCDebug(logCore) << "Проверяем наличие обязательных параметров";
    for (const auto& key : schema.keys()) {
        if (schema[key].required && !params.keys().contains(key)) {
            QString error_message = QString("Нет обязательного параметра '%1' во входных файлах").arg(key);
            qCWarning(logCore) << error_message;
            qCWarning(logCore) << "Отправляем сигнал failed";
            emit failed(error_message);
            return;
        }
    }
    qCDebug(logCore) << "Проверка обязательных параметров завершена";


    qCInfo(logCore) << QString("Успещно закончили чтение входных файлов конфигурации");
}

QStringList ConfigReader::readFile(const QString& file_path, QStringList& error_messages) {
    qCInfo(logCore) << "Вызван метод readFile\n"
                       "Читаем файл : '" + file_path + "'";
    error_messages.clear();
    QStringList result;

    QFile file(file_path);

    if (!file.open(QIODevice::Text | QIODevice::ReadOnly)) {
        QString message = QString("Не удалось открыть файл '%1' - %2").arg(file_path).arg(file.errorString());
        qCWarning(logCore) << message;
        error_messages.append(message);
        return QStringList();
    }

    QByteArray data = file.readAll();
    qCDebug(logCore) << "Успешно прочитали файл";

    const EncodingDetector::Encoding encoding = EncodingDetector::detect(data);

    if (encoding == EncodingDetector::Encoding::ERROR) {
        QString message = QString("Ошибка при определении кодировки файла '%1'").arg(file_path);
        qCWarning(logCore) << message;
        error_messages.append(message);
        return QStringList();
    }

    qCDebug(logCore) << "Определена кодировка: '" + QString(EncodingDetector::codecName(encoding)) + "'";

    QTextStream stream(&data, QIODevice::ReadOnly);
    stream.setCodec(EncodingDetector::codecName(encoding));

    while (!stream.atEnd()) {
        result.append(stream.readLine());
    }

    file.close();

    qCInfo(logCore) << "Метод readFile успешно завершен.";
    return result;
}

AppConfig ConfigReader::createConfig(QMap<QString, QStringList> readed_params, QStringList& error_messages)
{
   qCInfo(logCore) << "Вызван метод createConfig";

   error_messages.clear();
   config_reader::ConfigSchema schema = config_reader::createConfigShema();

   qCDebug(logCore) << "Выполняем проверку параметров из входных файлов по схеме";

   for (const QString& key : schema.keys()) {
       qCDebug(logCore) << "Проверяем параметр '" + key + "'";
       if (schema[key].required && !readed_params.contains(key)) {
           QString message = QString("Нет обязательного параметра '%1' во входных файлах").arg(key);
           qCWarning(logCore) << message;
           error_messages.append(message);
           continue;
       }
       if (!readed_params.contains(key)) {
           qCInfo(logCore) << QString("В прочитанных параметрах не найден параметр - '%1'."
                                      "Ему будет проставлено значение по умолчанию: '%2'").arg(key).arg(schema[key].default_value.toString());
           QStringList value;
           if (schema[key].type == config_reader::ParamType::StringList) {
               value = schema[key].default_value.toStringList();
           } else {
               value = QStringList(schema[key].default_value.toString());
           }
       }
   }

   qCDebug(logCore) << "Завершили проверху входных параметров по схеме";

   if (!error_messages.isEmpty()) {
       qCWarning(logCore) << QString("Во входных параметра есть проблемы:\n%1").arg(error_messages.join("\n"));
       return AppConfig();
   }

   AppConfig cfg;
   cfg.spo_path = readed_params[app_config_names::spo_path_name]

   qCInfo(logCore) << "Метод createConfig завершен успешно";
}
