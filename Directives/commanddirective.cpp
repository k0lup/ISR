#include "commanddirective.h"
#include <QLoggingCategory>
#include "logger/logging_categories.h"

CommandDirective::CommandDirective(const Command directive, QObject *parent) :
    Direct(directive, parent)
{

}

bool CommandDirective::isValid(QStringList &errors) const {
    bool res {true};
    errors.clear();

    if (m_directive_.number < 0) {
        errors.append("Недопустимый номер директивы!");
        res = false;
    }

    if (m_directive_.command_lines.count() < 1) {
        errors.append("В директиве нет строк. КРИТИЧЕСКАЯ ОШИБКА В ПАРСЕРЕ ФАЙЛА СТРУКТУРЫ РАЗДЕЛА");
        res = false;
    }

    if (m_directive_.command_lines.at(0).type.trimmed() != "К") {
        errors.append("Неверно указан тип директивы");
        res = false;
    }

    if (m_directive_.command_lines.at(0).command.trimmed().isEmpty()) {
        errors.append("Не указана команда в 1 строке директивы!");
        res = false;
    }

    return res;
}

void CommandDirective::start() {
    qCDebug(logCore) << "Начали выполнять директиву КОМАНДА";
    command_.clear();
    ResultDirective result;

    QStringList errors;
    if (!isValid(errors)) {
        result.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result.message = errors.join(" ");
        qCDebug(logCore) << "закончили выполнять директиву КОМАНДА с ошибкой";
        emit finished(result);
        return;
    }

    for (int num_line = 0; num_line < m_directive_.command_lines.count(); ++num_line) {
        QString line = m_directive_.command_lines[num_line].command.trimmed();
        if (!line.isEmpty()) {
            command_.append(" " + line);
        }
    }

    if (command_.isEmpty()) {
        result.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result.message = "Пустая операция для передачи в ПРИС";
        qCDebug(logCore) << "закончили выполнять директиву КОМАНДА с ошибкой";
        emit finished(result);
        return;
    }

    WidgetInfo widget_info;
    widget_info.title = "Директива для ПРИС";
    widget_info.information.append(command_);

    qCDebug(logCore) << "отправили запрос директивы КОМАНДА на отображение окна в GUI";
    emit showWindow(WidgetTypes::INFO, widget_info);
}

void CommandDirective::onWindowResult(const WindowResultData& result) {
    ResultDirective result_directive;
    if (!result.status) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::NOT_COMPLETED;
        emit finished(result_directive);
        return;
    }

    if (command_.isEmpty()) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = "Отсуствует операция после вывода сообщения перед передачей в ПРИС";
        emit finished(result_directive);
        return;
    }

    PRISMessage pris_message;
    pris_message.message = command_;
    emit sendMessageToPris(pris_message);
}

void CommandDirective::onPRISResult(const PRISResultData& result) {
    ResultDirective result_direcitve;
    if (!result.status) {
        result_direcitve.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_direcitve.message = "Ошибка в ответе от ПРИСа";
        emit finished(result_direcitve);
        return;
    }

    result_direcitve.type = RESULT_DIRECTIVE_TYPES::OK;
    result_direcitve.message.clear();
    emit finished(result_direcitve);
}

CommandDirective::~CommandDirective() {

}
