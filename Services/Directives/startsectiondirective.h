#ifndef STARTSECTIONDIRECTIVE_H
#define STARTSECTIONDIRECTIVE_H

#include "Services/Directives/directive.h"
#include <QString>

class StartSectionDirective : public Direct
{
public:
    StartSectionDirective(const DirectiveStruct& directive, QObject *parent = nullptr);
    bool isValid() override;
    void run() override;
    DirectType getTypeDirect() const override {return DirectType::START_SECTION;};
    ExecuteParam getExecuteParam() const;
    ~StartSectionDirective();
public slots:
    void onWindowResult(const WindowResultData& result) override;
    //void onPRISResult(const PRISResultData& result) override {Q_UNUSED(result)};
    void onReqStartProgResult(const bool result) override;

private:
    bool parseOperation(const QString& operation, ExecuteParam& params) const;
    bool checkOperation(const QString& operation) const;
    void parseDirective() override;
private slots:
    void onFiledDetected(const QString& message) override;
    void onFinishedDetected(const ResultDirective& result) override;
private:
    QString operation_;
    QString command_;

    ExecuteParam params_;

private:
    const static QString regular_exp_;
    const static QString state_sec_reg_par_val_;
    const static QString not_state_sec_reg_par_val_;
    const static QString pri_state_sec_reg_par_val_;
    const static QString mode_par_val_auto;
    const static QString mode_par_val_step;

};

#endif // STARTSECTIONDIRECTIVE_H
