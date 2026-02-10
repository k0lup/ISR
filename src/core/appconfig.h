#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>
#include <QStringList>
#include <QMap>

struct AppConfig {
    QString userOnPath;
    QString prisCfgPath;
    bool checkpointsEnabled = false;

    QString razdel;
    QString pfksPath;

    QString spoRoot;
    QString settingsRoot;
    QString filesRoot;
    QString sectionsRoot;
    QString protocolPath;
    QStringList programs;
    QStringList sections;
    QMap<QString, QString> edDocs;

    bool isValid(QString *error = nullptr) const;
};

#endif // APPCONFIG_H
