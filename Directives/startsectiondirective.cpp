#include "startsectiondirective.h"
#include <QString>
#include <QRegularExpression>

StartSectionDirective::StartSectionDirective(const Command directive, QObject* parent) :
    Direct(directive, parent)
{

}

bool StartSectionDirective::isValid(QStringList &errors) const {
    errors.clear();
    bool res{true};

    if (m_directive_.number < 0) {
        errors.append("НЕДОПУСТИМЫЙ НОМЕР ДИРЕКТИВЫ");
        res = false;
    }

    if (m_directive_.command_lines.at(0).operation.isEmpty()) {
        errors.append("В 1 СТРОКЕ ДИРЕКТИВЫ НЕТ ОПЕРАЦИИ ДЛЯ ВЫПОЛНЕНИЯ!");
        res = false;
    }

    if (m_directive_.command_lines.at(0).type.trimmed() != "И") {
        errors.append("Неверно указан тип директивы");
        res = false;
    }

    QString operation;

    for (int num_line = 0; num_line < m_directive_.command_lines.count(); ++num_line) {
        QString cur_operation = m_directive_.command_lines.at(num_line).operation;

        if (!cur_operation.isEmpty()) {
            operation.append(cur_operation);
        }
    }

    if (operation.isEmpty()) {
        errors.append("ПУСТАЯ СЕКЦИЯ ОПЕРАЦИИ ДИРЕКТИВЫ!");
        res = false;
    }

    if (!checkOperation(operation)) {
        errors.append("НЕДОПУСТИМЫЙ ФОРМАТ КОМАНДЫ НАЧРАЗД");
        res = false;
    }


    return res;
}


void StartSectionDirective::start() {
    ResultDirective result;
    QStringList errors;
    if (!isValid(errors)) {
        result.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result.message = errors.join("\n");
        emit finished(result);
        return;
    }
    operation_.clear();
    command_.clear();
    params_.clear();

    for (int num_line = 0; num_line < m_directive_.command_lines.count(); ++num_line) {
        QString cur_operation = m_directive_.command_lines.at(num_line).operation;
        QString cur_command = m_directive_.command_lines.at(num_line).command;

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

    if (!parseOperation(operation_, params_)) {
        result.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result.message = "НЕДОПУСТИМЫЙ ФОРМАТ КОМАНДЫ НАЧРАЗД";
        emit finished(result);
        return;
    }

    WidgetInfo wgt_info;
    wgt_info.information.append(operation_);
    wgt_info.title = "Директива НАЧРАЗД";

    emit showWindow(WidgetTypes::INFO, wgt_info);
}

void StartSectionDirective::onWindowResult(const WindowResultData& result) {
    ResultDirective result_directive;

    if (!result.status) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::NOT_COMPLETED;
        result_directive.message = "";
        emit finished(result_directive);
        return;
    }

    if (operation_.isEmpty()) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = "ПОСЛЕ ПОЛУЧЕНИЯ ОТВЕТА ОТ ПОЛЬЗОВАТЕЛЯ ОТСУТСВУЕТ ОПЕРАЦИЯ ДЛЯ ВЫПОЛНЕНИЯ";
        emit finished(result_directive);
        return;
    }

    emit requestStartProgram(params_);
}

void StartSectionDirective::onReqStartProgResult(const bool result) {
    ResultDirective result_directive;
    if (result) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = "НЕ УДАЛОСЬ ЗАПУСТИТЬ РАЗДЕЛ";
        emit finished(result_directive);
        return;
    } else {
        result_directive.type = RESULT_DIRECTIVE_TYPES::OK;
        result_directive.message = "";
        emit finished(result_directive);
    }
}

bool StartSectionDirective::checkOperation(const QString& operation) const {
    START_SECTION_PARAMS params_temp;
    return parseOperation(operation, params_temp);
}

bool StartSectionDirective::parseOperation(const QString& operation, START_SECTION_PARAMS& params) const {
    params.clear();
    QRegularExpression re;
    re.setPattern(regular_exp_);
    QRegularExpressionMatch match = re.match(operation);
    if (!match.hasMatch()) {
        return false;
    }

    QString abbrev = match.captured("abbr");

    QString section;
    if (!match.captured("sec_nshs").isEmpty()) {
        section = "НШС";
    } else if (!match.captured("sec_main").isEmpty()) {
        section = match.captured("sec_main"); // ШТАТ или ПРИ
    } else {
        section = "ШТАТ"; // секция не задана
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
        mode = "ШАГ"; // режим не задан
    }

    if (abbrev.isEmpty() || section.isEmpty() || mode.isEmpty()) {
        return false;
    }

    params.abbrev = abbrev;
    params.label = label;
    params.section = section;
    params.mode = mode;

    return true;
}

StartSectionDirective::~StartSectionDirective() {

}
