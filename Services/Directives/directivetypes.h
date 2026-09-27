#pragma once
#include <QString>


struct VARIANT_DATA_VAR_DIRECTIVE {
    QString command;
    QString operation;

    void clear() {
        command.clear();
        operation.clear();
    }
};
