#include "insructionsdirective.h"
#include <QLoggingCategory>
#include <Logger/logger.h>
#include "logging_categories.h"

InsructionsDirective::InsructionsDirective(const DirectiveStruct& directive, QObject *parent) :
    Direct(directive, parent)
{
    qCInfo(logCore) << "Вызван конструктор InsructionsDirective";
    qCInfo(logCore) << "Устанавливаем состояние директивы в Created";
    directive_state_ = DirectiveState::Created;
    QObject::connect(this, &Direct::failed, this, &InsructionsDirective::onFiledDetected);
    QObject::connect(this, &Direct::finished, this, &InsructionsDirective::onFinishedDetected);
    if (isValid()) {
        parseDirective();
        qCInfo(logCore) << "Устанавливаем состояние директивы в Ready";
        directive_state_ = DirectiveState::Ready;
    } else {
        qCInfo(logCore) << "Устанавливаем состояние директивы в Failed";
        directive_state_ = DirectiveState::Failed;
    }
    qCInfo(logCore) << "Завершен конструктор InsructionsDirective";
}

bool InsructionsDirective::isValid()
{
    qCInfo(logCore) << "Вызван метод isValid()";

    bool is_valid = true;
    errors_valid_.clear();

    if (m_directive_.number < 0) {
        errors_valid_.append("Недопустимый номер директивы!");
        is_valid = false;
    }

    if (m_directive_.lines.count() < 1) {
        errors_valid_.append("В директиве нет строк. КРИТИЧЕСКАЯ ОШИБКА В ПАРСЕРЕ ФАЙЛА СТРУКТУРЫ РАЗДЕЛА");
        is_valid = false;
    }

    if (m_directive_.lines.at(0).type_col.trimmed() != "П") {
        errors_valid_.append("Неверно указан тип директивы");
        is_valid = false;
    }

    if (m_directive_.lines.at(0).command_col.trimmed().isEmpty()) {
        errors_valid_.append("Не указана команда в 1 строке директивы!");
        is_valid = false;
    }

    if (!is_valid) {
        qCWarning(logCore) << "При проверке директивы были выявлены следующие проблемы:\n" + errors_valid_.join("\n");
    }

    qCInfo(logCore) << "Завершен метод isValid()";
    return is_valid;
}

void InsructionsDirective::parseDirective()
{
    if (directive_state_ != DirectiveState::Created) {
        QString error_message ="Парсер директивы вызван в недопустимом состоянии директивы";
        qCCritical(logCore) << error_message;
        qCInfo(logCore) << "Отправляем сигнал failed()";
        emit failed(error_message);
        return;
    }

    qCInfo(logCore) << "Вызван метод parseDirective()";

    for (int num_line = 0; num_line < m_directive_.lines.count(); ++num_line) {
        QString line = m_directive_.lines[num_line].command_col.trimmed();
        if (!line.isEmpty()) {
            command_.append(" " + line);
        }
    }

    qCInfo(logCore) << "Завершен метод parseDirective()";
}

void InsructionsDirective::run()
{
    if (directive_state_ != DirectiveState::Ready) {
        QString error_message ="Парсер директивы вызван в недопустимом состоянии директивы";
        qCCritical(logCore) << error_message;
        qCInfo(logCore) << "Отправляем сигнал failed()";
        emit failed(error_message);
        return;
    }

    qCInfo(logCore) << "Вызван метод isValid()";
    ResultDirective result;

    if (command_.isEmpty()) {
        result.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result.message = "Пустая операция для передачи в прис";
        qCWarning(logCore) << result.message;
        qCInfo(logCore) << "Отправляем сигнал finished()";
        emit finished(result);
        return;
    }

    WidgetInfo widget_info;
    widget_info.title = "Директива для ПРИС";
    widget_info.information.append(command_);

    qCInfo(logCore) << "Устанавливаем состояние директивы в WaitingForGui";
    directive_state_ = DirectiveState::WaitingForGui;
    qCInfo(logCore) << "Отправляем сигнал showWindow";
    emit showWindow(WidgetTypes::INFO, widget_info);
    qCInfo(logCore) << "Завершен метод isValid()";
}

void InsructionsDirective::onWindowResult(const WindowResultData& result)
{
    if (directive_state_ != DirectiveState::WaitingForGui) {
        QString error_message ="onWindowResult() вызван в недопустимом состоянии директивы";
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
        qCInfo(logCore) << "Отправляем сигнал finished()";
        emit finished(result_directive);
        return;
    }

    if (command_.isEmpty()) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = "Отсуствует операция после вывода сообщения перед передачей в ПРИС";
        qCCritical(logCore) << result_directive.message;
        qCInfo(logCore) << "Отправляе сигнал failed()";
        emit failed(result_directive.message);
        return;
    }

    PRISMessage pris_message;
    pris_message.message = command_;
    qCInfo(logCore) << "Устанавливаем состояние директивы в WaitingForPris";
    directive_state_ = DirectiveState::WaitingForPris;
    qCInfo(logCore) << "Отправляем сигнал sendMessageToPris()";
    emit sendMessageToPris(pris_message);
    qCInfo(logCore) << "Завершен метод onWindowResult()";
}

void InsructionsDirective::onPRISResult(const PRISResultData& result)
{
    if (directive_state_ != DirectiveState::WaitingForPris) {
        QString error_message ="onPRISResult() вызван в недопустимом состоянии директивы";
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
        qCInfo(logCore) << "Отправляем сигнал finished()";
        emit finished(result_direcitve);
        return;
    }

    result_direcitve.type = RESULT_DIRECTIVE_TYPES::OK;
    result_direcitve.message.clear();
    qCInfo(logCore) << "Отправляем сигнал finished()";
    emit finished(result_direcitve);
    qCInfo(logCore) << "Завершен метод onPRISResult()";
}

void InsructionsDirective::onFiledDetected(const QString& message)
{
    qCCritical(logCore) << "При выполнении директивы произошел критический сбой:\n" + message;
    qCInfo(logCore) << "Устанавливаем состояние директивы в Failed";
    directive_state_ = DirectiveState::Failed;
}

void InsructionsDirective::onFinishedDetected()
{
    qCInfo(logCore) << "Выполнение директивы завершено!";
    qCInfo(logCore) << "Устанавливаем состояние директивы в Finished";
    directive_state_ = DirectiveState::Finished;
}

InsructionsDirective::~InsructionsDirective()
{
    qCInfo(logCore) << "Вызван и завершен деструктор ~InsructionsDirective()";
}
