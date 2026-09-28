#include "startsectiondirective.h"
#include <QString>
#include <QRegularExpression>
#include <QLoggingCategory>
#include <Logger/logger.h>
#include "logging_categories.h"

const QString StartSectionDirective::regular_exp_ = (R"(^[ ]*НАЧРАЗД[ ]+(?<abbr>\S{1,16})(?:[ ]+(?:(?<sec_main>ШТАТ|ПРИ)(?:[ ]+(?<mode_main>АВТ|ШАГ))?|(?<sec_nshs>НШС)(?:[ ]+:(?<label>[^ ]+))?(?:[ ]+(?<mode_nshs>АВТ|ШАГ))?|(?<mode_only>АВТ|ШАГ)))?[ ]*$)");
const QString StartSectionDirective::state_sec_reg_par_val_ = "ШТАТ";
const QString StartSectionDirective::not_state_sec_reg_par_val_ = "НШС";
const QString StartSectionDirective::pri_state_sec_reg_par_val_ = "ПРИ";
const QString StartSectionDirective::mode_par_val_auto = "АВТ";
const QString StartSectionDirective::mode_par_val_step = "ШАГ";

StartSectionDirective::StartSectionDirective(const DirectiveStruct& directive, QObject* parent) :
    Direct(directive, parent)
{
    qCInfo(logCore) << "Вызван конструктор StartSectionDirective";
    qCInfo(logCore) << "Устанавливаем состояние директивы в Created";
    directive_state_ = DirectiveState::Created;
    QObject::connect(this, &Direct::failed, this, &StartSectionDirective::onFiledDetected);
    QObject::connect(this, &Direct::finished, this, &StartSectionDirective::onFinishedDetected);
    if (isValid()) {
        parseDirective();
        qCInfo(logCore) << "Устанавливаем состояние директивы в Ready";
        directive_state_ = DirectiveState::Ready;
    } else {
        qCInfo(logCore) << "Устанавливаем состояние директивы в Failed";
        directive_state_ = DirectiveState::Failed;
    }
    qCInfo(logCore) << "Завершен конструктор StartSectionDirective";
}

bool StartSectionDirective::isValid()
{
    bool is_valid = true;
    errors_valid_.clear();

    if (m_directive_.number < 0) {
        errors_valid_.append("НЕДОПУСТИМЫЙ НОМЕР ДИРЕКТИВЫ");
        is_valid = false;
    }

    if (m_directive_.lines.count() < 1) {
        errors_valid_.append("В директиве нет строк. КРИТИЧЕСКАЯ ОШИБКА В ПАРСЕРЕ ФАЙЛА СТРУКТУРЫ РАЗДЕЛА");
        is_valid = false;
    } else {
        if (m_directive_.lines.at(0).operation_col.isEmpty()) {
            errors_valid_.append("В 1 СТРОКЕ ДИРЕКТИВЫ НЕТ ОПЕРАЦИИ ДЛЯ ВЫПОЛНЕНИЯ!");
            is_valid = false;
        }

        if (m_directive_.lines.at(0).type_col.trimmed() != "И") {
            errors_valid_.append("Неверно указан тип директивы");
            is_valid = false;
        }

        QString operation;

        for (int num_line = 0; num_line < m_directive_.lines.count(); ++num_line) {
            QString cur_operation = m_directive_.lines.at(num_line).operation_col;

            if (!cur_operation.isEmpty()) {
                operation.append(cur_operation);
            }
        }

        if (operation.isEmpty()) {
            errors_valid_.append("ПУСТАЯ СЕКЦИЯ ОПЕРАЦИИ ДИРЕКТИВЫ!");
            is_valid = false;
        }

        if (!checkOperation(operation)) {
            errors_valid_.append("НЕДОПУСТИМЫЙ ФОРМАТ КОМАНДЫ НАЧРАЗД");
            is_valid = false;
        }
    }


    if (!is_valid) {
        qCWarning(logCore) << "При проверке директивы были выявлены следующие проблемы:\n" + errors_valid_.join("\n");
    }

    qCInfo(logCore) << "Завершен метод isValid()";
    return is_valid;
}

void StartSectionDirective::parseDirective()
{
    if (directive_state_ != DirectiveState::Created) {
        QString error_message ="Парсер директивы вызван в недопустимом состоянии директивы";
        qCCritical(logCore) << error_message;
        qCInfo(logCore) << "Отправляем сигнал failed()";
        emit failed(error_message);
        return;
    }
    qCInfo(logCore) << "Вызван метод parseDirective()";

    operation_.clear();
    command_.clear();
    params_.clear();

    for (int num_line = 0; num_line < m_directive_.lines.count(); ++num_line) {
        QString cur_operation = m_directive_.lines.at(num_line).operation_col;
        QString cur_command = m_directive_.lines.at(num_line).command_col;

        if (!cur_operation.isEmpty()) {
            operation_.append(cur_operation);
        }

        if (!cur_command.isEmpty()) {
            command_.append(cur_command + "\n");
        }
    }

    if (!command_.isEmpty()) {
        command_.chop(1);
    }

    qCInfo(logCore) << "Завершен метод parseDirective()";
}


void StartSectionDirective::run()
{
    if (directive_state_ != DirectiveState::Ready) {
        QString error_message = "run() вызван в недопустимом состоянии директивы";
        qCCritical(logCore) << error_message;
        qCInfo(logCore) << "Отправляем сигнал failed()";
        emit failed(error_message);
        return;
    }

    qCInfo(logCore) << "Вызван метод run()";
    ResultDirective result;
    QStringList errors;

    if (!parseOperation(operation_, params_)) {
        result.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result.message = "НЕДОПУСТИМЫЙ ФОРМАТ КОМАНДЫ НАЧРАЗД";
        qCWarning(logCore) << result.message;
        qCInfo(logCore) << "Отправляем сигнал finished()";
        emit finished(result);
        return;
    }

    WidgetInfo wgt_info;
    wgt_info.information.append(operation_);
    wgt_info.title = "Директива НАЧРАЗД";

    qCInfo(logCore) << "Устанавливаем состояние директивы в WaitingForGui";
    directive_state_ = DirectiveState::WaitingForGui;
    qCInfo(logCore) << "Отправляем сигнал showWindow()";
    emit showWindow(WidgetTypes::INFO, wgt_info);
    qCInfo(logCore) << "Завершен метод run()";
}

void StartSectionDirective::onWindowResult(const WindowResultData& result)
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
        result_directive.message = "";
        qCInfo(logCore) << "Пользователь отказался от выполнения директивы";
        qCInfo(logCore) << "Отправляем сигнал finished()";
        emit finished(result_directive);
        return;
    }

    if (operation_.isEmpty()) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = "ПОСЛЕ ПОЛУЧЕНИЯ ОТВЕТА ОТ ПОЛЬЗОВАТЕЛЯ ОТСУТСВУЕТ ОПЕРАЦИЯ ДЛЯ ВЫПОЛНЕНИЯ";
        qCCritical(logCore) << result_directive.message;
        qCInfo(logCore) << "Отправляем сигнал failed()";
        emit failed(result_directive.message);
        return;
    }

    qCInfo(logCore) << "Устанавливаем состояние директивы в WaitingForProgram";
    directive_state_ = DirectiveState::WaitingForProgram;
    qCInfo(logCore) << "Отправляем сигнал requestStartProgram()";
    emit requestStartProgram(params_);
    qCInfo(logCore) << "Завершен метод onWindowResult()";
}

void StartSectionDirective::onReqStartProgResult(const bool result)
{
    if (directive_state_ != DirectiveState::WaitingForProgram) {
        QString error_message = "onReqStartProgResult() вызван в недопустимом состоянии директивы";
        qCCritical(logCore) << error_message;
        qCInfo(logCore) << "Отправляем сигнал failed()";
        emit failed(error_message);
        return;
    }
    qCInfo(logCore) << "Вызван метод onReqStartProgResult()";

    ResultDirective result_directive;
    if (!result) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = "НЕ УДАЛОСЬ ЗАПУСТИТЬ РАЗДЕЛ";
        qCWarning(logCore) << result_directive.message;
        qCInfo(logCore) << "Отправляем сигнал finished()";
        emit finished(result_directive);
        return;
    } else {
        result_directive.type = RESULT_DIRECTIVE_TYPES::OK;
        result_directive.message = "";
        qCInfo(logCore) << "Отправляем сигнал finished()";
        emit finished(result_directive);
    }
    qCInfo(logCore) << "Завершен метод onReqStartProgResult()";
}

bool StartSectionDirective::checkOperation(const QString& operation) const
{
    qCInfo(logCore) << "Вызван метод checkOperation(). Метод завершен с результатом выполнения parseOperation()";
    ExecuteParam params_temp;
    return parseOperation(operation, params_temp);
}

bool StartSectionDirective::parseOperation(const QString& operation, ExecuteParam& params) const
{
    qCInfo(logCore) << "Вызван метод parseOperation()";
    params.clear();
    QRegularExpression re;
    re.setPattern(regular_exp_);
    QRegularExpressionMatch match = re.match(operation);
    if (!match.hasMatch()) {
        qCInfo(logCore) << "Завершен метод parseOperation() - false";
        return false;
    }

    QString abbrev = match.captured("abbr");

    QString section;
    if (!match.captured("sec_nshs").isEmpty()) {
        section = not_state_sec_reg_par_val_;
    } else if (!match.captured("sec_main").isEmpty()) {
        section = match.captured("sec_main"); // ШТАТ или ПРИ
    } else {
        section = state_sec_reg_par_val_; // секция не задана
    }

    QString label = match.captured("label"); // без символа ':'

    QString mode;
    if (!match.captured("mode_nshs").isEmpty()) {
        mode = match.captured("mode_nshs");
    } else if (!match.captured("mode_main").isEmpty()) {
        mode = match.captured("mode_main");
    } else if (!match.captured("mode_only").isEmpty()) {
        mode = match.captured("mode_only");
    } else {
        mode = mode_par_val_step; // режим не задан
    }

    if (abbrev.isEmpty() || section.isEmpty() || mode.isEmpty()) {
        qCInfo(logCore) << "Завершен метод parseOperation() - false";
        return false;
    }

    if (section == state_sec_reg_par_val_) {
        params.active_chapter = ChapterType::STATE;
    } else if (section == not_state_sec_reg_par_val_) {
        params.active_chapter = ChapterType::NOT_STATE;
    } else if (section == pri_state_sec_reg_par_val_) {
        params.active_chapter = ChapterType::PRI;
    } else {
        params.clear();
        return false;
    }

    if (mode == mode_par_val_auto) {
        params.mode = ExecuteMode::AUTO;
    } else if (mode == mode_par_val_step) {
        params.mode = ExecuteMode::STEP;
    } else {
        params.clear();
        qCCritical(logCore) << "Завершен метод parseOperation() - false (ТУТ НЕ ДОЛЖЕН СРАБАТЫВАТЬ)";
        return false;
    }

    params.section_name = abbrev;
    params.label = label;

    qCInfo(logCore) << "Завершен метод parseOperation() - true";
    return true;
}

void StartSectionDirective::onFiledDetected(const QString& message)
{
    qCCritical(logCore) << "При выполнении директивы произошел критический сбой:\n" + message;
    qCInfo(logCore) << "Устанавливаем состояние директивы в Failed";
    directive_state_ = DirectiveState::Failed;
}

void StartSectionDirective::onFinishedDetected()
{
    qCInfo(logCore) << "Выполнение директивы завершено!";
    qCInfo(logCore) << "Устанавливаем состояние директивы в Finished";
    directive_state_ = DirectiveState::Finished;
}

StartSectionDirective::~StartSectionDirective()
{
    qCInfo(logCore) << "Вызван и завершен деструктор ~InsructionsDirective()";
}
