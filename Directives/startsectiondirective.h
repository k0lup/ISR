#ifndef STARTSECTIONDIRECTIVE_H
#define STARTSECTIONDIRECTIVE_H

#include "directive.h"

class StartSectionDirective : public Direct
{
public:
    StartSectionDirective(const Command directive, QObject *parent = nullptr);
    bool isValid(QStringList& errors) const override;
    void start() override;
    TypeDirect getTypeDirect() const override {return TypeDirect::START_SECTION;};
    ~StartSectionDirective();
public slots:
    void onWindowResult(const WindowResultData& result) override;
    void onPRISResult(const PRISResultData& result) override;
};

#endif // STARTSECTIONDIRECTIVE_H
