#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include "appconfig.h"

class ConfigLoader {
public:
    bool loadFromArgs(const QStringList &args, AppConfig &config, QString &error) const;

private:
    bool loadPrisConfig(const QString &path, AppConfig &config, QString &error) const;
    bool loadUserOn(const QString &path, AppConfig &config, QString &error) const;
    static QStringList splitMultiValue(const QString &value);
};

#endif // CONFIGLOADER_H
