#include "pristransport.h"

#include <QString>
#include <cstring>

PrisTransport::PrisTransport(QObject *parent)
    : QObject(parent)
{
    connect(&m_porSocket, &QTcpSocket::readyRead, this, &PrisTransport::onReadyRead);
    connect(&m_porSocket, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::errorOccurred), this,
            [this](QAbstractSocket::SocketError) { emit transportError(m_porSocket.errorString()); });
    connect(&m_logSocket, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::errorOccurred), this,
            [this](QAbstractSocket::SocketError) { emit transportError(m_logSocket.errorString()); });

    connect(&m_porSocket, &QTcpSocket::connected, this, [this] { emit transportInfo(QStringLiteral("POR сокет подключен")); });
    connect(&m_logSocket, &QTcpSocket::connected, this, [this] { emit transportInfo(QStringLiteral("LOG сокет подключен")); });
}

void PrisTransport::connectToHost(const QString &host, quint16 porPort, quint16 logPort)
{
    m_porSocket.connectToHost(host, porPort);
    m_logSocket.connectToHost(host, logPort);
}

void PrisTransport::sendPor(const PorMessage &message)
{
    const QByteArray raw = message.serialize();
    m_porSocket.write(raw);
}

void PrisTransport::sendLogLine(const QString &line)
{
    QByteArray bytes = line.toUtf8();
    bytes.append('\n');
    m_logSocket.write(bytes);
}

void PrisTransport::onReadyRead()
{
    m_rxBuffer.append(m_porSocket.readAll());

    while (m_rxBuffer.size() >= PorMessage::headerSize()) {
        PorHeader header;
        std::memcpy(&header, m_rxBuffer.constData(), sizeof(PorHeader));

        if (header.LenPor < PorMessage::headerSize()) {
            emit transportError(QStringLiteral("Получен POR с некорректной длиной"));
            m_rxBuffer.clear();
            return;
        }

        if (m_rxBuffer.size() < header.LenPor) {
            return;
        }

        const QByteArray frame = m_rxBuffer.left(header.LenPor);
        m_rxBuffer.remove(0, header.LenPor);

        PorMessage message;
        QString parseError;
        if (!PorMessage::deserialize(frame, message, &parseError)) {
            emit transportError(parseError);
            continue;
        }

        emit porReceived(message);
    }
}
