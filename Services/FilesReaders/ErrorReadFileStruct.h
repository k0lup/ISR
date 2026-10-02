#ifndef ERRORREADFILESTRUCT_H
#define ERRORREADFILESTRUCT_H
#include <QString>

struct ErrorReadFile
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
};

#endif // ERRORREADFILESTRUCT_H
