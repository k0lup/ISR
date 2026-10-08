#include "commanddirective.h"
#include <QLoggingCategory>
#include <Logger/logger.h>
#include "logging_categories.h"

CommandDirective::CommandDirective(const DirectiveStruct& directive, QObject *parent) :
    Direct(directive, parent)
{
    qCInfo(logCore) << "Вызван конструктор CommandDirective";
    qCInfo(logCore) << "Устанавливаем состояние директивы в Created";
    directive_state_ = DirectiveState::Created;
    QObject::connect(this, &Direct::failed, this, &CommandDirective::onFiledDetected);
    QObject::connect(this, &Direct::finished, this, &CommandDirective::onFinishedDetected);
    if (isValid()) {
        parseDirective();
        qCInfo(logCore) << "Устанавливаем состояние директивы в Ready";
        directive_state_ = DirectiveState::Ready;
    } else {
        qCInfo(logCore) << "Устанавливаем состояние директивы в Failed";
        directive_state_ = DirectiveState::Failed;
    }
    qCInfo(logCore) << "Завершен конструктор CommandDirective";
}

void CommandDirective::parseDirective()
{
    if (directive_state_ != DirectiveState::Created) {
        QString error_message = "Парсер директивы вызван в недопустимом состоянии директивы: " + directStateToString();
        qCCritical(logCore) << error_message;
        qCInfo(logCore) << "Отправляем сигнал failed()";
        emit failed(error_message);
        return;
    }

    for (int num_line = 0; num_line < m_directive_.lines.count(); ++num_line) {
        QString line = m_directive_.lines[num_line].command_col.trimmed();
        if (!line.isEmpty()) {
            command_.append(" " + line);
        }
    }

    qCInfo(logCore) << "Завершен метод parseDirective()";
}

bool CommandDirective::isValid()
{
    qCInfo(logCore) << "Вызван метод isValid()";
    bool is_valid = true;

    is_valid = true;
    errors_valid_.clear();

    if (m_directive_.number < 0) {
        errors_valid_.append("Недопустимый номер директивы!");
        is_valid = false;
    }

    if (m_directive_.lines.count() < 1) {
        errors_valid_.append("В директиве нет строк. КРИТИЧЕСКАЯ ОШИБКА В ПАРСЕРЕ ФАЙЛА СТРУКТУРЫ РАЗДЕЛА");
        is_valid = false;
    } else {
        if (m_directive_.lines.at(0).type_col.trimmed() != "К") {
            errors_valid_.append("Неверно указан тип директивы");
            is_valid = false;
        }

        if (m_directive_.lines.at(0).command_col.trimmed().isEmpty()) {
            errors_valid_.append("Не указана команда в 1 строке директивы!");
            is_valid = false;
        }
    }

    if (!is_valid) {
        qCWarning(logCore) << "При проверке директивы были выявлены следующие проблемы:\n" + errors_valid_.join("\n");
        qCInfo(logCore) << "Переводим состояние директивы в Failed";
        directive_state_ = DirectiveState::Failed;
    }

    qCInfo(logCore) << "Завершен метод isValid()";
    return is_valid;
}

void CommandDirective::run()
{
    if (directive_state_ != DirectiveState::Ready &&
            directive_state_ != DirectiveState::Finished) {
        QString error_message = "run() вызван в недопустимом состоянии директивы: " + directStateToString();
        qCCritical(logCore) << error_message;
        qCInfo(logCore) << "Отправляем сигнал failed()";
        emit failed(error_message);
        return;
    }

    qCInfo(logCore) << "Вызван метод run()";
    ResultDirective result;

    if (command_.isEmpty()) {
        result.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result.message = "Пустая операция для передачи в ПРИС";
        qCCritical(logCore) << "закончили выполнять директиву КОМАНДА с ошибкой:\n" + result.message;
        qCInfo(logCore) << "Отправляем сигнал failed";
        emit finished(result);
        return;
    }

    WidgetInfo widget_info;
    widget_info.title = "Директива для ПРИС";
    widget_info.information.append(command_);

    qCInfo(logCore) << "Устанавливаем состояние директивы в WaitingForGui";
    directive_state_ = DirectiveState::WaitingForGui;
    qCInfo(logCore) << "Отправляем сигнал showWindow()";
    emit showWindow(WidgetTypes::INFO, widget_info);
    qCInfo(logCore) << "Завершен метод run()";
}

void CommandDirective::onWindowResult(const WindowResultData& result)
{
    if (directive_state_ != DirectiveState::WaitingForGui) {
        QString error_message ="onWindowResult() вызван в недопустимом состоянии директивы: " + directStateToString();
        qCCritical(logCore) << error_message;
        qCInfo(logCore) << "Отправляем сигнал failed()";
        emit failed(error_message);
        return;
    }

    qCInfo(logCore) << "Вызван метод onWindowResult()";
    ResultDirective result_directive;
    if (!result.status) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::NOT_COMPLETED;
        qCInfo(logCore) << "Пользователь отказался от выполнения директивы";
        qCInfo(logCore) << "Отправлен сигнал finished";
        emit finished(result_directive);
        return;
    }

    if (command_.isEmpty()) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = "Отсуствует операция после вывода сообщения перед передачей в ПРИС";
        qCInfo(logCore) << "Отправлен сигнал finished";
        emit finished(result_directive);
        return;
    }

    PRISMessage pris_message;
    pris_message.message = command_;
    qCInfo(logCore) << "Устанавливаем состояние директивы в WaitingForPris";
    directive_state_ = DirectiveState::WaitingForPris;
    qCInfo(logCore) << "Отправлен сигнал sendMessageToPris";
    emit sendMessageToPris(pris_message);
    qCInfo(logCore) << "Завершен метод onWindowResult()";
}

void CommandDirective::onPRISResult(const PRISResultData& result)
{
    if (directive_state_ != DirectiveState::WaitingForPris) {
        QString error_message = "onPRISResult() вызван в недопустимом состоянии директивы: " + directStateToString();
        qCCritical(logCore) << error_message;
        qCInfo(logCore) << "Отправляем сигнал failed()";
        emit failed(error_message);
        return;
    }

    qCInfo(logCore) << "Вызван метод onPRISResult()";
    ResultDirective result_direcitve;
    if (!result.status) {
        result_direcitve.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_direcitve.message = "Ошибка в ответе от ПРИСа";
        qCWarning(logCore) << result_direcitve.message;
        qCInfo(logCore) << "Отправлен сигнал finished";
        emit finished(result_direcitve);
        return;
    }

    result_direcitve.type = RESULT_DIRECTIVE_TYPES::OK;
    result_direcitve.message.clear();
    qCInfo(logCore) << "Отправлен сигнал finished";
    emit finished(result_direcitve);
    qCInfo(logCore) << "Завершен метод onPRISResult()";
}


void CommandDirective::onFiledDetected(const QString& message)
{
    qCCritical(logCore) << "При выполнении директивы произошел критический сбой:\n" + message;
    qCInfo(logCore) << "Устанавливаем состояние директивы в Failed";
    directive_state_ = DirectiveState::Failed;
}

void CommandDirective::onFinishedDetected(const ResultDirective& result)
{
    qCInfo(logCore) << "Выполнение директивы завершено!";

    QString log_message = "Устанавливаем состояние директивы в ";

    switch (result.type) {
    case RESULT_DIRECTIVE_TYPES::OK : directive_state_ = DirectiveState::Finished; break;
    case RESULT_DIRECTIVE_TYPES::NOT_COMPLETED : directive_state_ = DirectiveState::Ready; break;
    case RESULT_DIRECTIVE_TYPES::ERROR : directive_state_ = DirectiveState::Failed; break;
    }

    log_message.append(directStateToString());
    qCInfo(logCore) << log_message;
}

CommandDirective::~CommandDirective()
{
    qCInfo(logCore) << "Вызван и завершен деструктор ~CommandDirective()";
}
