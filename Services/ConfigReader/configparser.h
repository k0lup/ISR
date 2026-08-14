#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <QMap>
#include <QStringList>

class ConfigParser
{
public:
    ConfigParser();
public:
    QMap<QString, QStringList> parseData(const QStringList& data, QStringList error_messages);
};

#endif // CONFIGPARSER_H
