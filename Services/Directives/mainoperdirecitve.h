#ifndef MAINOPERDIRECITVE_H
#define MAINOPERDIRECITVE_H
#include "Services/Directives/directive.h"


class MainOperDirecitve : public Direct
{
public:
    MainOperDirecitve(const DirectiveStruct& directive, QObject *parent = nullptr);
    bool isValid() override;
    void run() override;
    DirectType getTypeDirect() const override {return DirectType::MAIN_OPERATION;};
    ~MainOperDirecitve();
public slots:
    void onWindowResult(const WindowResultData& result) override;
    void onPRISResult(const PRISResultData& result) override;
private:
    void parseDirective() override;
private slots:
    void onFiledDetected(const QString& message) override;
    void onFinishedDetected(const ResultDirective& result) override;
private:
    QString operation_;
};

#endif // MAINOPERDIRECITVE_H
