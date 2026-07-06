#ifndef VARIANTDIRECTIVE_H
#define VARIANTDIRECTIVE_H

#include "directive.h"

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
};

#endif // VARIANTDIRECTIVE_H
