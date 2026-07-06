#include "startsectiondirective.h"

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

    QStringList operation_list = operation.split("|");
    if (operation_list.isEmpty()) {
        errors.append("НЕТ ПАРАМЕТРОВ В СЕКЦИИ ОПЕРАЦИИ");
        res = false;
    }

    if (operation_list.last().isEmpty()) {
        operation_list.removeLast();
    }

    if (operation_list.count() < 1 || operation_list.count() > 4) {
        errors.append("НЕДОПУСТИМОЕ КОЛИЧЕСТВО ПАРАМЕТРОВ В СЕКЦИИ ОПЕРАЦИИ");
        res = false;
    }

    if (operation_list[0].isEmpty()) {
        errors.append("ПАРАМЕТР 'аббрев' ПУСТ!");
        res = false;
    }

    if (operation_list.count() > 1) {

        QString section = operation_list[1].trimmed();
        if (!section.isEmpty() && section != "ШТАТ" && section != "НШС" && section != "ПРИ") {
            errors.append("НЕДОПУСТИМАЯ СЕКЦИЯ");
        }
    }

    return res;
}
