#ifndef VARIANTDIRECTIVE_H
#define VARIANTDIRECTIVE_H

#include "Services/Directives/directive.h"
#include <QStringList>
#include <QList>

class VariantDirective : public Direct
{
public:
    VariantDirective(const DirectiveStruct& directive, QObject *parent = nullptr);
    bool isValid() override;
    void run() override;
    DirectType getTypeDirect() const override {return DirectType::VARIANT;};
    ~VariantDirective();
public slots:
    void onWindowResult(const WindowResultData& result) override;
    void onPRISResult(const PRISResultData& result) override;
private:
    QMap<int, VARIANT_DATA_VAR_DIRECTIVE> getVariants(const QList<DirectiveLine>& lines, QStringList& errors) const;
    void parseDirective() override;
private slots:
    void onFiledDetected(const QString& message) override;
    void onFinishedDetected() override;
private:
    QMap<int, VARIANT_DATA_VAR_DIRECTIVE> variants_;
};

#endif // VARIANTDIRECTIVE_H
