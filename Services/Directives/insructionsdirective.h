#ifndef INSRUCTIONSDIRECTIVE_H
#define INSRUCTIONSDIRECTIVE_H

#include "Services/Directives/directive.h"

class InsructionsDirective : public Direct
{
public:
    InsructionsDirective(const DirectiveStruct& directive, QObject *parent = nullptr);
    bool isValid() override;
    void run() override;
    DirectType getTypeDirect() const override {return DirectType::INSTRUCTIONS;};
    ~InsructionsDirective();
public slots:
    void onWindowResult(const WindowResultData& result) override;
    void onPRISResult(const PRISResultData& result) override;
private:
    void parseDirective() override;
private slots:
    void onFiledDetected(const QString& message) override;
    void onFinishedDetected(const ResultDirective& result) override;
private:
    QString command_;
};

#endif // INSRUCTIONSDIRECTIVE_H
