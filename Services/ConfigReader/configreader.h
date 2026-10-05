#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include "configparser.h"
#include "configtypes.h"
#include <QStringList>

class ConfigReader
{
public:
    ConfigReader();
public:
    bool readConfig(const QString& cfg_file_path, const QString& on_file_path,
                             const QString& service_on_file_path);

    AppConfig getConfig() const;
    QString getErrors() const;

private:
    QStringList readFile(const QString& file_path, QStringList& error_messages) const;

    AppConfig createConfig(QMap<QString, QStringList> readed_params, QStringList& error_messages) const;
    bool validateParams(const QMap<QString, QStringList>& readed_params, QStringList& error_messages) const;
    bool validateQString(const QStringList& param, QStringList& error_messages, const QString& reg_exp = "") const;
    bool validateQStringList(const QStringList& param, QStringList& error_messages, const QString& reg_exp = "") const;

    int getPortFromString(const QString& port) const;
private:
    ConfigParser parser_;

    QString errors_;
    AppConfig app_config_;
};

#endif // CONFIGREADER_H
