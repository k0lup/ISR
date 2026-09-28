#include "variantdirective.h"
#include <QLoggingCategory>
#include <Logger/logger.h>
#include "logging_categories.h"

VariantDirective::VariantDirective(const DirectiveStruct& directive, QObject* parent) :
    Direct(directive, parent)
{
    qCInfo(logCore) << "Вызван конструктор VariantDirective";
    qCInfo(logCore) << "Устанавливаем состояние директивы в Created";
    directive_state_ = DirectiveState::Created;
    QObject::connect(this, &Direct::failed, this, &VariantDirective::onFiledDetected);
    QObject::connect(this, &Direct::finished, this, &VariantDirective::onFinishedDetected);
    if (isValid()) {
        parseDirective();
        qCInfo(logCore) << "Устанавливаем состояние директивы в Ready";
        directive_state_ = DirectiveState::Ready;
    } else {
        qCInfo(logCore) << "Устанавливаем состояние директивы в Failed";
        directive_state_ = DirectiveState::Failed;
    }
    qCInfo(logCore) << "Завершен конструктор VariantDirective";
}


bool VariantDirective::isValid()
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
        if (!m_directive_.lines.at(0).operation_col.trimmed().isEmpty()) {
            errors_valid_.append("В 1 СТРОКЕ ДИРЕКТИВЫ ПОЛЕ ОПЕРАЦИИ ДОЛЖНО БЫТЬ ПУСТЫМ");
            is_valid = false;
        }

        if (m_directive_.lines.at(0).type_col.trimmed() != "В") {
            errors_valid_.append("Неверно указан тип директивы");
            is_valid = false;
        }

        QStringList errors_parse_variants;
        QMap<int, VARIANT_DATA_VAR_DIRECTIVE> variants = getVariants(m_directive_.lines, errors_parse_variants);
        if (!errors_parse_variants.isEmpty()) {
            errors_valid_.append(errors_parse_variants);
            is_valid = false;
        }

        if (variants.keys().count() == 0) {
            errors_valid_.append("НЕ ДОПУСКАЕТСЯ ОТСУТСТВИЕ ВАРИАНТОВ В ДИРЕКТИВЕ!");
            is_valid = false;
        }
    }

    if (!is_valid) {
        qCWarning(logCore) << "При проверке директивы были выявлены следующие проблемы:\n" + errors_valid_.join("\n");
    }

    qCInfo(logCore) << "Завершен метод isValid()";
    return is_valid;
}

QMap<int, VARIANT_DATA_VAR_DIRECTIVE> VariantDirective::getVariants(const QList<DirectiveLine>& lines, QStringList& errors) const
{
    qCInfo(logCore) << "Вызван метод getVariants()";
    QMap<int, VARIANT_DATA_VAR_DIRECTIVE> variants;
    QStringList cur_operation;
    QStringList cur_command;
    int cur_number_variant{-1};
    bool variant_start{false};

    for (int num_line = 1; num_line < lines.count(); ++num_line) {
        if (!lines[num_line].type_col.isEmpty()) {
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
            cur_number_variant = lines[num_line].type_col.toInt(&ok);
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

        QString operation = m_directive_.lines[num_line].operation_col.trimmed();
        QString command = m_directive_.lines[num_line].command_col.trimmed();
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

    qCInfo(logCore) << "Завершен метод getVariants()";
    return variants;
}

void VariantDirective::parseDirective()
{
    if (directive_state_ != DirectiveState::Created) {
        QString error_message ="Парсер директивы вызван в недопустимом состоянии директивы";
        qCCritical(logCore) << error_message;
        qCInfo(logCore) << "Отправляем сигнал failed()";
        emit failed(error_message);
        return;
    }
    qCInfo(logCore) << "Вызван метод parseDirective()";


    QStringList errors;
    variants_ = getVariants(m_directive_.lines, errors);

    qCInfo(logCore) << "Завершен метод parseDirective()";
}

void VariantDirective::run() {
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

    WidgetInfo wgt_info;
    wgt_info.title = "Директива ВАРИАНТ";
    //wgt_info.variants = variants_;
    for (auto var : variants_.keys()) {
        wgt_info.information.append(QString::number(var));
    }

    qCInfo(logCore) << "Устанавливаем состояние директивы в WaitingForGui";
    directive_state_ = DirectiveState::WaitingForGui;
    qCInfo(logCore) << "Отправляем сигнал showWindow()";
    emit showWindow(WidgetTypes::VARIANT, wgt_info);
    qCInfo(logCore) << "Завершен метод run()";
}

void VariantDirective::onWindowResult(const WindowResultData& result) {
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

    if (variants_.isEmpty()) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = "ПОСЛЕ ПОЛУЧЕНИЯ ОТВЕТА ОТ ОКНА ПОЛЬЗОВАТЕЛЯ ОТСУТСТВУЮТ ДАННЫЕ О ВАРИАНТАХ";
        qCCritical(logCore) << result_directive.message;
        qCInfo(logCore) << "Отправляем сигнал failed()";
        emit finished(result_directive);
        return;
    }

    bool ok{false};
    int user_variant = result.data.toInt(&ok);
    if (!ok) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = QString("НЕДОПУСТИМЫЙ ВАРИАНТ ВЫБРАННЫЙ ПОЛЬЗОВАТЕЛЕМ: %1").arg(result.data);
        qCCritical(logCore) << result_directive.message;
        qCInfo(logCore) << "Отправляем сигнал failed()";
        emit failed(result_directive.message);
        return;
    }

    if (!variants_.contains(user_variant)) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = "ПОЛЬЗОВАТЕЛЕМ БЫЛ ВЫБРАН НЕСУЩЕСТВУЮЩИЙ ВАРИАНТ";
        qCCritical(logCore) << result_directive.message;
        qCInfo(logCore) << "Отправляем сигнал failed()";
        emit failed(result_directive.message);
        return;
    }

    QString command = variants_.value(user_variant).command;
    if (command.isEmpty()) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = "ПУСТАЯ КОМАНДА ДЛЯ ОТПРАВКИ В ПРИС!";
        qCCritical(logCore) << result_directive.message;
        qCInfo(logCore) << "Отправляем сигнал failed()";
        emit failed(result_directive.message);
        return;
    }

    PRISMessage message;
    message.message = command;
    qCInfo(logCore) << "Устанавливаем состояние директивы в WaitingForPris";
    directive_state_ = DirectiveState::WaitingForPris;
    qCInfo(logCore) << "Отправляем сигнал sendMessageToPris";
    emit sendMessageToPris(message);
    qCInfo(logCore) << "Завершен метод onWindowResult()";
    return;
}

void VariantDirective::onPRISResult(const PRISResultData& result)
{
    if (directive_state_ != DirectiveState::WaitingForPris) {
        QString error_message = "onPRISResult() вызван в недопустимом состоянии директивы";
        qCCritical(logCore) << error_message;
        qCInfo(logCore) << "Отправляем сигнал failed()";
        emit failed(error_message);
        return;
    }

    qCInfo(logCore) << "Вызван метод onPRISResult()";

    ResultDirective result_directive;
    if (!result.status) {
        result_directive.type = RESULT_DIRECTIVE_TYPES::ERROR;
        result_directive.message = result.data;
        qCWarning(logCore) << result_directive.message;
        qCInfo(logCore) << "Отправляем сигнал finished()";
        emit finished(result_directive);
        return;
    }

    result_directive.type = RESULT_DIRECTIVE_TYPES::OK;
    result_directive.message = "";
    qCInfo(logCore) << "Отправляем сигнал finished()";
    emit finished(result_directive);
    qCInfo(logCore) << "Завершен метод onPRISResult()";
    return;
}

void VariantDirective::onFiledDetected(const QString& message)
{
    qCCritical(logCore) << "При выполнении директивы произошел критический сбой:\n" + message;
    qCInfo(logCore) << "Устанавливаем состояние директивы в Failed";
    directive_state_ = DirectiveState::Failed;
}

void VariantDirective::onFinishedDetected()
{
    qCInfo(logCore) << "Выполнение директивы завершено!";
    qCInfo(logCore) << "Устанавливаем состояние директивы в Finished";
    directive_state_ = DirectiveState::Finished;
}

VariantDirective::~VariantDirective() {

}
