#ifndef COMMANDDIRECTIVE_H
#define COMMANDDIRECTIVE_H
#include "directive.h"

class CommandDirective : public Direct
{
public:
    CommandDirective(const Command directive, QObject *parent = nullptr);
    bool isValid(QStringList& errors) const override;
    void start() override;
    TypeDirect getTypeDirect() const override {return TypeDirect::COMMAND;};
    ~CommandDirective();
public slots:
    void onWindowResult(const WindowResultData& result) override;
    void onPRISResult(const PRISResultData& result) override;
private:
    QString command_;
};

#endif // COMMANDDIRECTIVE_H
