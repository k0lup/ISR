#ifndef DOKLADDIRECTIVE_H
#define DOKLADDIRECTIVE_H
#include "directive.h"


class DokladDirective : public Direct
{
public:
    DokladDirective(const Command directive, QObject *parent = nullptr);
    bool isValid(QStringList& errors) const override;
    void start() override;
    TypeDirect getTypeDirect() const override {return TypeDirect::DOKLAD;};
    ~DokladDirective();
public slots:
    void onWindowResult(const WindowResultData& result) override;
    void onPRISResult(const PRISResultData& result) override;
private:
    QString command_;
};

#endif // DOKLADDIRECTIVE_H
