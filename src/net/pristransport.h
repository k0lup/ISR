#ifndef PRISTRANSPORT_H
#define PRISTRANSPORT_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>

#include "pormessage.h"

class PrisTransport : public QObject {
    Q_OBJECT
public:
    explicit PrisTransport(QObject *parent = nullptr);

    void connectToHost(const QString &host = QStringLiteral("127.0.0.1"), quint16 porPort = 0x4020, quint16 logPort = 0x4024);
    void sendPor(const PorMessage &message);
    void sendLogLine(const QString &line);

signals:
    void porReceived(const PorMessage &message);
    void transportError(const QString &error);
    void transportInfo(const QString &info);

private slots:
    void onReadyRead();

private:
    QTcpSocket m_porSocket;
    QTcpSocket m_logSocket;
    QByteArray m_rxBuffer;
};

#endif // PRISTRANSPORT_H
