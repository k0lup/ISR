#include "net_pris_client.h"
#include "core/por_codec.h"
NetPrisClient::NetPrisClient(quint16 port, QObject *parent)
    : QObject(parent), m_port(port) {
    connect(&m_socket, &QTcpSocket::connected, this, &NetPrisClient::onConnected);
    connect(&m_socket, &QTcpSocket::disconnected, this, &NetPrisClient::onDisconnected);
    connect(&m_socket, &QTcpSocket::readyRead, this, &NetPrisClient::onReadyRead);
    connect(&m_socket,
            qOverload<QAbstractSocket::SocketError>(&QTcpSocket::errorOccurred),
            this,
            &NetPrisClient::onErrorOccurred);
}

void NetPrisClient::open() {
    if (m_socket.state() == QAbstractSocket::ConnectedState ||
        m_socket.state() == QAbstractSocket::ConnectingState) {
        return;
    }

    m_socket.connectToHost(QStringLiteral("127.0.0.1"), m_port);
}

void NetPrisClient::close() {
    m_socket.disconnectFromHost();
}

bool NetPrisClient::sendPacket(const QByteArray &payload,
                               quint8 from,
                               quint8 to,
                               quint8 sort,
                               quint8 type,
                               quint8 atomCount,
                               quint8 kind,
                               qint16 no,
                               quint8 s1,
                               quint8 s2) {
    if (m_socket.state() != QAbstractSocket::ConnectedState) {
        emit socketError(endpointTag() + QStringLiteral(": socket is not connected"));
        return false;
    }

    QMutexLocker locker(&m_sendMutex);

    const POR header = por_codec::makeHeader(payload.size(), from, to, sort, type, atomCount, kind, no, s1, s2);
    const QByteArray packet = por_codec::encodePacket(header, payload);

    const auto written = m_socket.write(packet);
    if (written != packet.size()) {
        emit socketError(endpointTag() + QStringLiteral(": failed to write full packet"));
        return false;
    }

    emit socketLog(endpointTag() + QStringLiteral(": packet sent"));
    return m_socket.waitForBytesWritten(2000);
}

void NetPrisClient::onConnected() {
    emit socketLog(endpointTag() + QStringLiteral(": connected"));
}

void NetPrisClient::onDisconnected() {
    emit socketLog(endpointTag() + QStringLiteral(": disconnected"));
}

void NetPrisClient::onReadyRead() {
    m_rxBuffer += m_socket.readAll();

    while (m_rxBuffer.size() >= static_cast<int>(sizeof(POR))) {
        POR header{};
        if (!por_codec::tryDecodeHeader(m_rxBuffer, header)) {
            return;
        }

        if (header.lenPor < sizeof(POR)) {
            emit socketError(endpointTag() + QStringLiteral(": invalid packet size"));
            m_rxBuffer.clear();
            return;
        }

        if (m_rxBuffer.size() < header.lenPor) {
            return;
        }

        const QByteArray packet = m_rxBuffer.left(header.lenPor);
        m_rxBuffer.remove(0, header.lenPor);
        emit packetReceived(packet);
    }
}

void NetPrisClient::onErrorOccurred(QAbstractSocket::SocketError error) {
    emit socketError(endpointTag() + QStringLiteral(": ") + socketErrorText(error));
}

QString NetPrisClient::endpointTag() const {
    return m_port == 0x4020 ? QStringLiteral("NET_PRIS") : QStringLiteral("NET_PROT");
}

QString NetPrisClient::socketErrorText(QAbstractSocket::SocketError error) const {
    switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
        return QStringLiteral("connection refused");
    case QAbstractSocket::RemoteHostClosedError:
        return QStringLiteral("remote host closed connection");
    case QAbstractSocket::HostNotFoundError:
        return QStringLiteral("host not found");
    case QAbstractSocket::SocketTimeoutError:
        return QStringLiteral("socket timeout");
    default:
        return m_socket.errorString();
    }
}
