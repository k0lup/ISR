#pragma once

#include <QHash>
#include <QString>
#include <QStringList>
#include "ErrorReadFileStruct.h"

using RawConfig = QHash<QString, QStringList>;

//так как было принято решение вынести ошибку в отдельной .h (она подходит для всех файлов), чтобы не править код переопределим имя структуры на то, которое было до выноса
using ConfigError = ErrorReadFile;

/*struct ConfigError
{
    QString filePath;
    int line = -1;     // 1-based; -1 если не привязано к строке
    QString message;

    QString toString() const
    {
        if (line > 0)
            return QString("%1:%2: %3").arg(filePath).arg(line).arg(message);
        if (!filePath.isEmpty())
            return QString("%1: %2").arg(filePath, message);
        return message;
    }
};*/
