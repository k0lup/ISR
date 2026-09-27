#pragma once
#include <QString>
#include <QStringList>

enum class WidgetTypes {
    INFO,
    VARIANT,
    WARNING
};

struct WidgetInfo {
    QString title;
    QStringList information;

    //QMap<int, VARIANT_DATA_VAR_DIRECTIVE> variants; //only_for_variant_direct

    void clear() {
        title.clear();
        information.clear();
        //variants.clear();
    }
};

struct WindowResultData {
    bool status;
    QString data;
};
