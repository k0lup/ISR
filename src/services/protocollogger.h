#ifndef PROTOCOLLOGGER_H
#define PROTOCOLLOGGER_H

#include <QObject>
#include <QFile>

class PrisTransport;

class ProtocolLogger : public QObject {
    Q_OBJECT
public:
    explicit ProtocolLogger(PrisTransport *transport, QObject *parent = nullptr);

    void logLocal(const QString &line);
    void logPris(const QString &line);

private:
    QFile m_file;
    PrisTransport *m_transport = nullptr;
};

#endif // PROTOCOLLOGGER_H
