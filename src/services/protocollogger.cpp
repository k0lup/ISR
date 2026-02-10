#include "protocollogger.h"

#include "../net/pristransport.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QTextStream>

ProtocolLogger::ProtocolLogger(PrisTransport *transport, QObject *parent)
    : QObject(parent)
    , m_file(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("isr.log")))
    , m_transport(transport)
{
    m_file.open(QIODevice::Append | QIODevice::Text);
}

void ProtocolLogger::logLocal(const QString &line)
{
    if (!m_file.isOpen()) {
        return;
    }

    QTextStream out(&m_file);
    out << QDateTime::currentDateTime().toString(Qt::ISODate) << " " << line << "\n";
    m_file.flush();
}

void ProtocolLogger::logPris(const QString &line)
{
    if (m_transport) {
        m_transport->sendLogLine(line);
    }
    logLocal(QStringLiteral("[PRIS] %1").arg(line));
}
