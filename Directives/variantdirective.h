#ifndef VARIANTDIRECTIVE_H
#define VARIANTDIRECTIVE_H

#include "directive.h"
#include <QStringList>
#include <QList>

class VariantDirective : public Direct
{
public:
    VariantDirective(const Command directive, QObject *parent = nullptr);
    bool isValid(QStringList& errors) const override;
    void start() override;
    TypeDirect getTypeDirect() const override {return TypeDirect::VARIANT;};
    ~VariantDirective();
public slots:
    void onWindowResult(const WindowResultData& result) override;
    void onPRISResult(const PRISResultData& result) override;
private:
    QMap<int, VARIANT_DATA_VAR_DIRECTIVE> getVariants(const QList<CommandLine>& command_lines, QStringList& errors) const;
private:
    QMap<int, VARIANT_DATA_VAR_DIRECTIVE> variants_;
};

#endif // VARIANTDIRECTIVE_H
