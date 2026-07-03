#ifndef INSRUCTIONSDIRECTIVE_H
#define INSRUCTIONSDIRECTIVE_H

#include "directive.h"

class InsructionsDirective : public Direct
{
public:
    InsructionsDirective(const Command directive, QObject *parent = nullptr);
    bool isValid(QStringList& errors) const override;
    void start() override;
    TypeDirect getTypeDirect() const override {return TypeDirect::INSTRUCTIONS;};
    ~InsructionsDirective();
public slots:
    void onWindowResult(const WindowResultData& result) override;
    void onPRISResult(const PRISResultData& result) override;
private:
    QString command_;
};

#endif // INSRUCTIONSDIRECTIVE_H
