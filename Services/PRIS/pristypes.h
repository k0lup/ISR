#pragma once

#include <QByteArray>
#include <QString>

struct PRISResultData {
    bool status;
    QByteArray data;
};

struct PRISMessage {
    QString message;
};
