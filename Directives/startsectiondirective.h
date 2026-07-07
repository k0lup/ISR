#ifndef STARTSECTIONDIRECTIVE_H
#define STARTSECTIONDIRECTIVE_H

#include "directive.h"
#include <QString>

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
    //void onPRISResult(const PRISResultData& result) override {Q_UNUSED(result)};
    void onReqStartProgResult(const bool result) override;

private:
    bool parseOperation(const QString& operation, START_SECTION_PARAMS& params) const;
    bool checkOperation(const QString& operation) const;

private:
    QString regular_exp_ = (R"(^[ ]*НАЧРАЗД[ ]+(?<abbr>\S{1,16})(?:[ ]+(?:(?<sec_main>ШТАТ|ПРИ)(?:[ ]+(?<mode_main>АВТ|ШАГ))?|(?<sec_nshs>НШС)(?:[ ]+:(?<label>[^ ]+))?(?:[ ]+(?<mode_nshs>АВТ|ШАГ))?|(?<mode_only>АВТ|ШАГ)))?[ ]*$)");

    QString operation_;
    QString command_;

    START_SECTION_PARAMS params_;

};

#endif // STARTSECTIONDIRECTIVE_H
