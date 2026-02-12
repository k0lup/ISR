#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QMutex>

#include "portion.h"

class NetPrisClient final : public QObject {
    Q_OBJECT
public:
    explicit NetPrisClient(quint16 port, QObject *parent = nullptr);

    void open();
    void close();

    bool sendPacket(const QByteArray &payload,
                    quint8 from,
                    quint8 to,
                    quint8 sort,
                    quint8 type,
                    quint8 atomCount,
                    quint8 kind,
                    qint16 no,
                    quint8 s1,
                    quint8 s2);

signals:
    void packetReceived(const QByteArray &packet);
    void socketError(const QString &message);
    void socketLog(const QString &message);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError error);

private:
    QString endpointTag() const;
    QString socketErrorText(QAbstractSocket::SocketError error) const;

    QTcpSocket m_socket;
    QByteArray m_rxBuffer;
    quint16 m_port;
    QMutex m_sendMutex;
};
