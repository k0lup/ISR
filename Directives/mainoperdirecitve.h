#ifndef MAINOPERDIRECITVE_H
#define MAINOPERDIRECITVE_H
#include "directive.h"
#include "setReader/diireader.h"


class MainOperDirecitve : public Direct
{
public:
    MainOperDirecitve(const Command directive, QObject *parent = nullptr);
    bool isValid(QStringList& errors) const override;
    void start() override;
    TypeDirect getTypeDirect() const override {return TypeDirect::MAIN_OPERATION;};
    ~MainOperDirecitve();
public slots:
    void onWindowResult(const WindowResultData& result) override;
    void onPRISResult(const PRISResultData& result) override;
private:
    QString operation_;
};

#endif // MAINOPERDIRECITVE_H
