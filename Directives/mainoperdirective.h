#ifndef MAINOPERDIRECTIVE_H
#define MAINOPERDIRECTIVE_H
#include "directive.h"
#include "setReader/diireader.h"


class MainOperDirective : public Direct
{
public:
    MainOperDirective(const Command directive, QObject *parent = nullptr);
    bool isValid(QStringList& errors) const override;
    void start() override;
    TypeDirect getTypeDirect() const override {return TypeDirect::MAIN_OPERATION;};
    ~MainOperDirective();
public slots:
    void onWindowResult(const WindowResultData& result) override;
    void onPRISResult(const PRISResultData& result) override;
private:
    QString operation_;
};

#endif // MAINOPERDIRECTIVE_H
