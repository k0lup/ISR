#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <QObject>

#include "configparser.h"
#include "configtypes.h"

class ConfigReader : public QObject
{
    Q_OBJECT
public:
    ConfigReader(QObject* parent = nullptr);

public slots:
    void onReadFileRequested(const QString& cfg_file_path, const QString& on_file_path,
                             const QString& service_on_file_path);
signals:
    void failed(const QString& error_message);
    void success(const AppConfig& cfg);

private:
    QStringList readFile(const QString& file_path, QStringList& error_messages) const;

    AppConfig createConfig(QMap<QString, QStringList> readed_params, QStringList& error_messages) const;
    bool validateParams(const QMap<QString, QStringList>& readed_params, QStringList& error_messages) const;
    bool validateQString(const QStringList& param, QStringList& error_messages, const QString& reg_exp = "") const;
    bool validateQStringList(const QStringList& param, QStringList& error_messages, const QString& reg_exp = "") const;

    int getPortFromString(const QString& port) const;
private:
    ConfigParser parser_;
};

#endif // CONFIGREADER_H
