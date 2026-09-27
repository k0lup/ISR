#ifndef DOKLADDIRECTIVE_H
#define DOKLADDIRECTIVE_H
#include "Services/Directives/directive.h"

class DokladDirective : public Direct
{
public:
    DokladDirective(const DirectiveStruct& directive, QObject *parent = nullptr);
    bool isValid() override;
    void run() override;
    DirectType getTypeDirect() const override {return DirectType::DOKLAD;};
    ~DokladDirective();
public slots:
    void onWindowResult(const WindowResultData& result) override;
    void onPRISResult(const PRISResultData& result) override;
private:
    void parseDirective() override;
private slots:
    void onFiledDetected(const QString& message) override;
    void onFinishedDetected() override;
private:
    QString command_;
};

#endif // DOKLADDIRECTIVE_H
