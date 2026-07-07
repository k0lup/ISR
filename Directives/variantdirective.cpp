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
}

void VariantDirective::start() {
    return;
}

void VariantDirective::onWindowResult(const WindowResultData& result) {
    Q_UNUSED(result);
    return;
}

void VariantDirective::onPRISResult(const PRISResultData& result) {
    Q_UNUSED(result);
    return;
}

VariantDirective::~VariantDirective() {

}
