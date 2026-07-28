#include "variantdirective.h"

VariantDirective::VariantDirective(const Command directive, QObject* parent) :
    Direct(directive, parent)
{

}


bool VariantDirective::isValid(QStringList& errors) const {
    return true;
    errors.clear();
    bool res{true};

    if (m_directive_.number < 0) {
        errors.append("НЕДОПУСТИМЫЙ НОМЕР ДИРЕКТИВЫ");
        res = false;
    }

    if (!m_directive_.command_lines.at(0).operation.trimmed().isEmpty()) {
        errors.append("В 1 СТРОКЕ ДИРЕКТИВЫ ПОЛЕ ОПЕРАЦИИ ДОЛЖНО БЫТЬ ПУСТЫМ");
        res = false;
    }

    if (m_directive_.command_lines.at(0).type.trimmed() != "В") {
        errors.append("Неверно указан тип директивы");
        res = false;
    }

    QStringList errors_parse_variants;
    QMap<int, VARIANT_DATA_VAR_DIRECTIVE> variants = getVariants(m_directive_.command_lines, errors);
    if (!errors_parse_variants.isEmpty()) {
        errors.append(errors_parse_variants);
        res = false;
    }

    if (variants.keys().count() == 0) {
        errors.append("НЕ ДОПУСКАЕТСЯ ОТСУТСТВИЕ ВАРИАНТОВ В ДИРЕКТИВЕ!");
        res = false;
    }

    return res;
}

QMap<int, VARIANT_DATA_VAR_DIRECTIVE> VariantDirective::getVariants(const QList<CommandLine>& command_lines, QStringList& errors) const {
    QMap<int, VARIANT_DATA_VAR_DIRECTIVE> variants;
    QStringList cur_operation;
    QStringList cur_command;
    int cur_number_variant{-1};
    bool variant_start{false};

    for (int num_line = 1; num_line < command_lines.count(); ++num_line) {
        if (!command_lines[num_line].type.isEmpty()) {
            if (variant_start) {
                if (cur_operation.isEmpty()) {
                    errors.append(QString("ПУСТОЕ ПОЯСЕНЕНИЯ ДЛЯ ВАРИАНТА %1").arg(cur_number_variant));
                }
                if (cur_command.isEmpty()) {
                    errors.append(QString("НЕТ КОМАНДЫ ДЛЯ ВАРИАНТА %1").arg(cur_number_variant));
                }
                if (variants.contains(cur_number_variant)) {
                    errors.append(QString("ПОВТОРНАЯ ПОПЫТКА ДОБАВИТЬ ВАРИАНТ %1").arg(cur_number_variant));
                } else {
                    VARIANT_DATA_VAR_DIRECTIVE cur_variant;
                    cur_variant.command = cur_command.join("\n");
                    cur_variant.operation = cur_operation.join("\n");
                    variants.insert(cur_number_variant, cur_variant);
                }
            }

            variant_start = true;
            cur_command.clear();
            cur_operation.clear();
            cur_number_variant = -1;
            bool ok{false};
            cur_number_variant = command_lines[num_line].type.toInt(&ok);
            if (!ok) {
                errors.append("НЕДОПУСТИМЫЙ ФОРМАТ НОМЕРА ВАРИАНТА!");
                cur_number_variant = -1;
            } else if (cur_number_variant < 1 || cur_number_variant > 10) {
                errors.append("НЕДОПУСТИМЫЙ НОМЕР ВАРИАНТА! НОМЕР ДОЛЖЕН БЫТЬ В ДИАПАЗОНЕ ОТ 1 ДО 10!");
                cur_number_variant = -1;
            }
        }
        if (!variant_start) {
            continue;
        }

        QString operation = m_directive_.command_lines[num_line].operation.trimmed();
        QString command = m_directive_.command_lines[num_line].command.trimmed();
        if (!operation.isEmpty()) {
            cur_operation.append(operation);
        }
        if (!command.isEmpty()) {
            cur_command.append(command);
        }
    }

    if (variant_start) {
        if (cur_operation.isEmpty()) {
            errors.append(QString("ПУСТОЕ ПОЯСЕНЕНИЯ ДЛЯ ВАРИАНТА %1").arg(cur_number_variant));
        }
        if (cur_command.isEmpty()) {
            errors.append(QString("НЕТ КОМАНДЫ ДЛЯ ВАРИАНТА %1").arg(cur_number_variant));
        }
        if (variants.contains(cur_number_variant)) {
            errors.append(QString("ПОВТОРНАЯ ПОПЫТКА ДОБАВИТЬ ВАРИАНТ %1").arg(cur_number_variant));
        } else {
            VARIANT_DATA_VAR_DIRECTIVE cur_variant;
            cur_variant.command = cur_command.join("\n");
            cur_variant.operation = cur_operation.join("\n");
            variants.insert(cur_number_variant, cur_variant);
        }
    }

    return variants;
}

void VariantDirective::start() {
    ResultDirective result;
    QStringList errors;
    variants_.clear();
    if (!isValid(errors)) {
        result.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result.message = errors.join("\n");
        emit finished(result);
        return;
    }

    variants_ = getVariants(m_directive_.command_lines, errors);
    if (!errors.isEmpty()) {
        result.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result.message = errors.join("\n");
        emit finished(result);
        return;
    }

    WidgetInfo wgt_info;
    wgt_info.title = "Директива ВАРИАНТ";
    //wgt_info.variants = variants_;
    for (auto var : variants_.keys()) {
        wgt_info.information.append(QString::number(var));
    }

    emit showWindow(WidgetTypes::VARIANT, wgt_info);
}

void VariantDirective::onWindowResult(const WindowResultData& result) {
    ResultDirective result_directive;
    if (!result.status) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::NOT_COMPLETED;
        result_directive.message = "";
        emit finished(result_directive);
        return;
    }

    if (variants_.isEmpty()) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = "ПОСЛЕ ПОЛУЧЕНИЯ ОТВЕТА ОТ ОКНА ПОЛЬЗОВАТЕЛЯ ОТСУТСТВУЮТ ДАННЫЕ О ВАРИАНТАХ";
        emit finished(result_directive);
        return;
    }

    bool ok{false};
    int user_variant = result.data.toInt(&ok);
    if (!ok) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = QString("НЕДОПУСТИМЫЙ ВАРИАНТ ВЫБРАННЫЙ ПОЛЬЗОВАТЕЛЕМ: %1").arg(result.data);
        emit finished(result_directive);
        return;
    }

    if (!variants_.contains(user_variant)) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = "ПОЛЬЗОВАТЕЛЕМ БЫЛ ВЫБРАН НЕСУЩЕСТВУЮЩИЙ ВАРИАНТ";
        emit finished(result_directive);
        return;
    }

    QString command = variants_.value(user_variant).command;
    if (command.isEmpty()) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = "ПУСТАЯ КОМАНДА ДЛЯ ОТПРАВКИ В ПРИС!";
        emit finished(result_directive);
        return;
    }

    PRISMessage message;
    message.message = command;
    emit sendMessageToPris(message);

    return;
}

void VariantDirective::onPRISResult(const PRISResultData& result) {
    ResultDirective result_directive;
    if (!result.status) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = result.data;
        emit finished(result_directive);
        return;
    }

    result_directive.type = RESULT_DIRECTIVE_TYPES::OK;
    result_directive.message = "";
    emit finished(result_directive);
    return;
}

VariantDirective::~VariantDirective() {

}
