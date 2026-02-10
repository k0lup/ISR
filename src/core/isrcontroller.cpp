#include "isrcontroller.h"

#include "configloader.h"
#include "../net/pristransport.h"
#include "../services/protocollogger.h"
#include "../services/directivehandler.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QFile>
#include <QTextStream>
#include <QUrl>

IsrController::IsrController(QObject *parent)
    : QObject(parent)
    , m_transport(new PrisTransport(this))
    , m_logger(new ProtocolLogger(m_transport, this))
    , m_directiveHandler(new DirectiveHandler(this, this))
{
    connect(m_transport, &PrisTransport::porReceived, m_directiveHandler, &DirectiveHandler::onPorReceived);
    connect(m_transport, &PrisTransport::transportError, this, [this](const QString &error) {
        m_logger->logLocal(QStringLiteral("[NET][ERR] %1").arg(error));
    });
    connect(m_transport, &PrisTransport::transportInfo, this, [this](const QString &info) {
        m_logger->logLocal(QStringLiteral("[NET] %1").arg(info));
    });
}

bool IsrController::initialize(const QStringList &args, QString &error)
{
    ConfigLoader loader;
    if (!loader.loadFromArgs(args, m_config, error)) {
        return false;
    }

    SpoRepository repo;
    m_sections = repo.loadSections(m_config, error);
    if (m_sections.isEmpty()) {
        return false;
    }

    m_transport->connectToHost();
    loadStencil();

    m_logger->logLocal(QStringLiteral("ИСР запущен. .on=%1 cfg=%2 checkpoints=%3")
                       .arg(m_config.userOnPath, m_config.prisCfgPath)
                       .arg(m_config.checkpointsEnabled));
    emit statusChanged(QStringLiteral("ИСР - [раздел не выбран]"));
    return true;
}

const AppConfig &IsrController::config() const { return m_config; }
QVector<SectionInfo> IsrController::sections() const { return m_sections; }
QString IsrController::currentSection() const { return m_currentSection; }
QString IsrController::stencil() const { return m_stencil; }

bool IsrController::selectSection(const QString &abbr, QString &error)
{
    for (const SectionInfo &section : m_sections) {
        if (section.abbreviation == abbr) {
            m_currentSection = abbr;
            emit sectionChanged(abbr);
            emit statusChanged(QStringLiteral("ИСР - [выбран раздел: %1]").arg(abbr));
            m_logger->logLocal(QStringLiteral("Выбран раздел %1").arg(abbr));
            return true;
        }
    }

    error = QStringLiteral("Раздел не найден: %1").arg(abbr);
    return false;
}

bool IsrController::setStencil(const QString &value, QString &error)
{
    m_stencil = value.trimmed();
    QFile file(stencilFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        error = QStringLiteral("Не удалось сохранить трафарет: %1").arg(file.fileName());
        return false;
    }
    QTextStream out(&file);
    out << m_stencil;
    m_logger->logLocal(QStringLiteral("Трафарет установлен: %1").arg(m_stencil));
    return true;
}

bool IsrController::setHeader(const QString &headerText, QString &error)
{
    if (m_stencil.isEmpty()) {
        error = QStringLiteral("Сначала задайте трафарет");
        return false;
    }

    const QString protocolName = QStringLiteral("%1.%2").arg(headerText.trimmed(), m_stencil);

    PorMessage message;
    message.header.AdrFrom = 200;
    message.header.AdrTo = 105;
    message.header.KindofWork = 1;
    message.header.SortofWork = 1;
    message.header.AtomType = 3;
    message.header.AtomQuant = 1;
    message.payload = headerText.toUtf8();
    m_transport->sendPor(message);

    m_logger->logPris(QStringLiteral("[HDR] %1").arg(protocolName));
    emit statusChanged(QStringLiteral("ИСР - [%1, структура не загружена]").arg(m_currentSection));
    m_logger->logLocal(QStringLiteral("Отправлен заголовок раздела: %1").arg(headerText));
    return true;
}

QString IsrController::stencilFilePath() const
{
    return QCoreApplication::applicationDirPath() + QStringLiteral("/Stencil.dat");
}

void IsrController::loadStencil()
{
    QFile file(stencilFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    m_stencil = QString::fromUtf8(file.readAll()).trimmed();
}

void IsrController::sendSimpleAck(quint16 kind, quint16 sort, quint16 atomType, bool ok, quint16 adrTo)
{
    PorMessage ack;
    ack.header.AdrFrom = 200;
    ack.header.AdrTo = adrTo;
    ack.header.KindofWork = kind;
    ack.header.SortofWork = sort;
    ack.header.AtomType = ok ? 1 : 0;
    ack.header.AtomQuant = atomType;
    m_transport->sendPor(ack);
    m_logger->logLocal(QStringLiteral("ACK отправлен kind=%1 sort=%2 type=%3 ok=%4")
                       .arg(kind).arg(sort).arg(atomType).arg(ok));
}
