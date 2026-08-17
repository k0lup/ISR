#include "configparser.h"

#include "logger/logging_categories.h"
#include <QLoggingCategory>

ConfigParser::ConfigParser()
{
    qCInfo(logCore) << "Создан объект ConfigParser";
}

QMap<QString, QStringList> ConfigParser::parseData(const QStringList& data, QStringList& error_messages)
{
    qCInfo(logCore) << "Вызван метод parseData";
    QMap<QString, QStringList> result;

    for (int num_line = 0; num_line < data.count(); ++num_line) {
        qCDebug(logCore) << QString("Парсим '%1' строку из переданных '%2'").arg(num_line + 1).arg(data.count());

        QString start_error_message = QString("Строка '%1' ").arg(num_line + 1);
        QString line = data.at(num_line).trimmed();
        qCDebug(logCore) << "Строка: " + line;

        int comment_start_index = line.indexOf("//");
        if (comment_start_index != -1) {
            qCDebug(logCore) << "В строке был удален комментарий";
            line.truncate(comment_start_index);
        }

        if (line.isEmpty()) {
            qCDebug(logCore) << "Строка пустая. Переходим дальше";
            continue;
        }

        int separator_index = line.indexOf('=');

        if (separator_index == -1) {
            QString error_string = start_error_message + "- В строке нет резделителя '='";
            error_messages.append(error_string);
            qCWarning(logCore) << "Получили ошибку при разборе строки (переходим к следующей):\n" + error_string;
            continue;
        }

        QString param_name = line.left(separator_index).trimmed();
        QString all_param_values = line.right(separator_index + 1).trimmed();

        QStringList list_param_values = all_param_values.split(";");
        for (int num_param_value = 0; num_param_value < list_param_values.count(); ++num_param_value) {
            list_param_values[num_param_value] = list_param_values[num_param_value].trimmed();
            if (list_param_values[num_param_value].isEmpty()) {
                list_param_values.removeAt(num_param_value);
                num_param_value -= 1;
            }
        }

        if (param_name.isEmpty()) {
            QString error_string = start_error_message + "- Пустое имя параметра!";
            error_messages.append(error_string);
            qCWarning(logCore) << "Получили ошибку при разборе строки (переходим к следующей):\n" + error_string;
            continue;
        }

        if (list_param_values.isEmpty()) {
            QString error_string = start_error_message + "- Пустое значение параметра!";
            error_messages.append(error_string);
            qCWarning(logCore) << "Получили ошибку при разборе строки (переходим к следующей):\n" + error_string;
            continue;
        }


        qCDebug(logCore) << QString("Прочитали значение '%1' параметра '%2'").arg(list_param_values.join(";")).arg(param_name);

        if (result.contains(param_name)) {
            result[param_name].append(list_param_values);
            qCDebug(logCore) << "Добавили прочитанное значение к уже существующему параметру";
        } else {
            result.insert(param_name, QStringList(list_param_values));
            qCDebug(logCore) << "Создали параметр в словаре и положили прочитанное значение";
        }
    }

    qCInfo(logCore) << "Завершен метод parseData";
    return result;
}
