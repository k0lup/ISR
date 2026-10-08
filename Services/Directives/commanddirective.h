#ifndef COMMANDDIRECTIVE_H
#define COMMANDDIRECTIVE_H
#include "Services/Directives/directive.h"

class CommandDirective : public Direct
{
public:
    CommandDirective(const DirectiveStruct& directive, QObject *parent = nullptr);
    bool isValid() override;
    void run() override;
    DirectType getTypeDirect() const override {return DirectType::COMMAND;};
    ~CommandDirective();
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

#endif // COMMANDDIRECTIVE_H
