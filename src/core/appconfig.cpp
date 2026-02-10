#include "appconfig.h"

#include <QFileInfo>

bool AppConfig::isValid(QString *error) const
{
    auto setError = [error](const QString &text) {
        if (error) {
            *error = text;
        }
        return false;
    };

    if (userOnPath.isEmpty() || !QFileInfo::exists(userOnPath)) {
        return setError(QStringLiteral("Файл .on не найден: %1").arg(userOnPath));
    }
    if (prisCfgPath.isEmpty() || !QFileInfo::exists(prisCfgPath)) {
        return setError(QStringLiteral("Файл cfg не найден: %1").arg(prisCfgPath));
    }
    if (sectionsRoot.isEmpty() || !QFileInfo::exists(sectionsRoot)) {
        return setError(QStringLiteral("Каталог разделов не найден: %1").arg(sectionsRoot));
    }
    if (filesRoot.isEmpty() || !QFileInfo::exists(filesRoot)) {
        return setError(QStringLiteral("Каталог файлов не найден: %1").arg(filesRoot));
    }

    return true;
}
