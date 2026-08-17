#include "configreader.h"

#include "configschema.h"
#include "configtypes.h"

#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <QDir>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

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

    QStringList error_messages;
    AppConfig cfg = createConfig(params, error_messages);
    if (!error_messages.isEmpty()) {
        QString message = QString("При чтении значений параметров из входных файлов возникли ошибки:\n%1")
                .arg(error_messages.join("\n"));
        qCWarning(logCore) << error_messages;
        qCWarning(logCore) << "Отправляем сигнал failed";
        emit failed(message);
        return;
    }


    qCInfo(logCore) << QString("Успещно закончили чтение входных файлов конфигурации");
    qCInfo(logCore) << QString("Отправляем сигнал succes");
    emit success(cfg);
    return;
}

QStringList ConfigReader::readFile(const QString& file_path, QStringList& error_messages) const
{
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

AppConfig ConfigReader::createConfig(QMap<QString, QStringList> readed_params, QStringList& error_messages) const
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
           if (!schema[key].default_value.isValid()) {
              QString message = QString("Для параметра '%1' не задано значение во входных файлах и нет значения по умолчанию").arg(key);
              qCWarning(logCore) << message;
              error_messages.append(message);
              continue;
           }
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

   if (!validateParams(readed_params, error_messages)) {
       qCWarning(logCore) << QString("Во входных параметра есть проблемы:\n%1").arg(error_messages.join("\n"));
       return AppConfig();
   }

   AppConfig cfg;
   cfg.spo_path = readed_params[app_config_names::spo_path_name][0];
   cfg.settings_path = readed_params[app_config_names::settings_path_name][0];
   cfg.files_paths = readed_params[app_config_names::files_paths_name][0];
   cfg.sections_folder_path = readed_params[app_config_names::sections_folder_path_name][0];

   cfg.prot_path = readed_params[app_config_names::prot_path_name][0];
   cfg.programs_paths = readed_params[app_config_names::programs_paths_name];
   cfg.sections_paths = readed_params[app_config_names::sections_paths_name];

   cfg.nshc_isd = readed_params[app_config_names::nshc_isd_name][0];
   cfg.avar_isd = readed_params[app_config_names::avar_isd_name][0];
   cfg.pri_isd = readed_params[app_config_names::pri_isd_name][0];

   cfg.nshc_rep = readed_params[app_config_names::nshc_rep_name][0];
   cfg.avar_rep = readed_params[app_config_names::avar_rep_name][0];
   cfg.pri_rep = readed_params[app_config_names::pri_rep_name][0];

   cfg.isr_port = readed_params[app_config_names::isr_port_name][0].toInt();
   cfg.ip_server = readed_params[app_config_names::ip_server_name][0];
   cfg.port_server = readed_params[app_config_names::port_server_name][0].toInt();

   qCInfo(logCore) << "Метод createConfig завершен успешно";
   return cfg;
}


bool ConfigReader::validateParams(const QMap<QString, QStringList> &readed_params, QStringList &error_messages) const
{
    qCInfo(logCore) << "Вызван метод validateParams";
    error_messages.clear();
    bool res = true;

    config_reader::ConfigSchema schema = config_reader::createConfigShema();

    for (const auto& key : schema.keys()) {
        qCDebug(logCore) << "Выполняется проверка параметра '" + key + "'";
        QStringList error_messages_for_param;
        bool res_validate{true};
        if (schema[key].type == config_reader::ParamType::StringList) {
            res_validate = validateQStringList(readed_params[key], error_messages_for_param, schema[key].reg_exp);
        } else {
            res_validate = validateQString(readed_params[key], error_messages_for_param, schema[key].reg_exp);
        }

        if (!res_validate) {
            QString message = QString("При проверке параметра '%1' выявлены ошибки:\n%2").arg(key).arg(error_messages_for_param.join("\n"));
            error_messages.append(message);
            qCWarning(logCore) << message;
            res = false;
        }

        qCDebug(logCore) << "Завершена проверка параметра '" + key + "'";
    }
    qCDebug(logCore) << "Завершена проверка всех параметров";

    QString log_message = "Метод validateParams завершен с результатом %1";
    if (res) {
        log_message = log_message.arg("TRUE");
    } else {
        log_message = log_message.arg("FALSE");
    }

    qCInfo(logCore) << log_message;
    return res;
}

bool ConfigReader::validateQStringList(const QStringList &param, QStringList &error_messages, const QString &reg_exp) const
{
    qCInfo(logCore) << "Вызван метод validateQStringList";
    error_messages.clear();
    bool res{true};

    for (const auto& line : param) {
        QStringList error_messages_for_line;
        res &= validateQString(QStringList(line), error_messages_for_line, reg_exp);
        if (!res) {
            QString message = QString("При проверке строки '%1' были выявлены следующие ошибки:\n%2").arg(line).arg(error_messages_for_line.join("\n"));
            qCWarning(logCore) << message;
            error_messages.append(message);
            continue;
        }
    }

    QString log_message = "Метод validateQStringList завершен с результатом %1";
    if (res) {
        log_message = log_message.arg("TRUE");
    } else {
        log_message = log_message.arg("FALSE");
    }

    qCInfo(logCore) << log_message;
    return res;
}

//НЕ УВЕРЕН, ЧТО В МЕТОДЕ ВАЛИДАЦИИ КОРРЕКТНО СОЗДАВАТЬ ПАПКИ, НУ ДА ЛАДНО
bool ConfigReader::validateQString(const QStringList &param, QStringList &error_messages, const QString &reg_exp) const
{
    qCInfo(logCore) << "Вызван метод ValidateQString";
    error_messages.clear();

    if (param.count() != 1) {
        QString message = QString("Параметр должен быть представлен одной строкой, а фактически представлен '%1' строками").arg(param.count());
        qCWarning(logCore) << message;
        error_messages.append(message);
        return false;
    }

    QString param_line = param[0];
    if (param_line.trimmed().isEmpty()) {
        QString message = QString("Параметр не должен быть пустой строкой!");
        qCWarning(logCore) << message;
        error_messages.append(message);
        return false;
    }

    if (reg_exp == config_reader::folder_path_reg_exp_const) {
        qCDebug(logCore) << "Регулярное выражение требует проверки директории";
        QDir dir(param_line);
        if (!dir.exists()) {
            qCInfo(logCore) << QString("Директории '%1' не существует, пробуем создать...").arg(param_line);
            if (!dir.mkpath(".")) {
                QString message = QString("Директории '%1' не существует и ее не удалось создать!").arg(param_line);
                qCWarning(logCore) << message;
                error_messages.append(message);
                return false;
            } else {
                qCInfo(logCore) << QString("Была создана директория '%1'").arg(param_line);
            }
        } else {
            qCDebug(logCore) << QString("Директория '%1' существует").arg(param_line);
        }
    } else if (!reg_exp.isEmpty()) {
        qCDebug(logCore) << "Проверяем параметр по заданному регулярному выражению";
        QRegularExpression reg(reg_exp);
        QRegularExpressionMatch match = reg.match(param_line);
        if (!match.hasMatch()) {
            QString message = QString("Значение '%1' не соотвествует регулярному выражению '%2'")
                    .arg(param_line)
                    .arg(reg_exp);
            qCWarning(logCore) << message;
            error_messages.append(message);
            return false;
        }
    }

    qCInfo(logCore) << "Метод ValidateQString успешно завершен";
    return true;
}


/*
    if (!readed_params.contains(app_config_names::spo_path_name)) {
        QString message_error = QString("Отсутствует параметр '%1'").arg(app_config_names::spo_path_name);
        error_messages.append(message_error);
        qCWarning(logCore) << message_error;
        res = false;
    } else {
        if (readed_params[app_config_names::spo_path_name].count() != 1) {
            QString message_error = QString("Для параметра '%1' допустимо 1 значение. Было передано '%2' значений")
                    .arg(app_config_names::spo_path_name)
                    .arg(readed_params[app_config_names::spo_path_name].count());
            error_messages.append(message_error);
            qCWarning(logCore) << message_error;
            res = false;
        } else if (readed_params[app_config_names::spo_path_name][0].isEmpty()) {
            QString message_error = QString("Для параметра '%1' отсутствует значение (пустое значение)")
                    .arg(app_config_names::spo_path_name);
            error_messages.append(message_error);
            qCWarning(logCore) << message_error;
            res = false;
        }
    }const QString& reg_exp,
    */
