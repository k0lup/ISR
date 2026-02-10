#include "directivehandler.h"

#include "../core/isrcontroller.h"
#include "../net/pristransport.h"
#include "protocollogger.h"

#include <QDesktopServices>
#include <QFileInfo>
#include <QUrl>

DirectiveHandler::DirectiveHandler(IsrController *controller, QObject *parent)
    : QObject(parent)
    , m_controller(controller)
{
}

void DirectiveHandler::onPorReceived(const PorMessage &message)
{
    auto *logger = m_controller->m_logger;
    logger->logLocal(QStringLiteral("IN POR k/s/t=%1/%2/%3 len=%4")
                     .arg(message.header.KindofWork)
                     .arg(message.header.SortofWork)
                     .arg(message.header.AtomType)
                     .arg(message.header.LenPor));

    if (message.header.KindofWork == 1 && message.header.SortofWork == 9 && message.header.AtomType == 0) {
        handleNaSr(message);
    } else if (message.header.KindofWork == 1 && message.header.SortofWork == 8 && message.header.AtomType == 0) {
        handleEd(message);
    } else if (message.header.KindofWork == 1 && message.header.SortofWork == 5 && message.header.AtomType == 0) {
        handlePuskRazd(message);
    }
}

void DirectiveHandler::handleNaSr(const PorMessage &message)
{
    const QString text = QString::fromUtf8(message.payload).trimmed().remove('"');
    const QStringList parts = text.split('|');
    const QString sectionRaw = parts.value(0).trimmed().toUpper();

    QString mapped;
    if (sectionRaw == QStringLiteral("ШТАТНОЕ") || sectionRaw == QStringLiteral("ШТАТ") || sectionRaw == QStringLiteral("SHTAT")) {
        mapped = QStringLiteral("Штат");
    } else if (sectionRaw == QStringLiteral("НШС") || sectionRaw == QStringLiteral("NHS")) {
        mapped = QStringLiteral("НШС");
    } else if (sectionRaw == QStringLiteral("ПРИ") || sectionRaw == QStringLiteral("PRI")) {
        mapped = QStringLiteral("ПРИ");
    }

    if (!mapped.isEmpty()) {
        emit m_controller->switchSectionTab(mapped);
        m_controller->sendSimpleAck(2, 9, 1, true, 101);
    } else {
        m_controller->sendSimpleAck(2, 9, 1, false, 101);
    }
}

void DirectiveHandler::handleEd(const PorMessage &message)
{
    auto *logger = m_controller->m_logger;
    const QString text = QString::fromUtf8(message.payload).trimmed().remove('"');
    const QStringList parts = text.split('|');
    const QString kind = parts.value(0).trimmed().toUpper();
    const QString mark = parts.value(1).trimmed();

    const QString docName = m_controller->m_config.edDocs.value(kind);
    if (docName.isEmpty()) {
        logger->logLocal(QStringLiteral("Неизвестный тип ЭД: %1").arg(kind));
        m_controller->sendSimpleAck(2, 8, 1, false, 101);
        return;
    }

    const QString path = m_controller->m_config.filesRoot + QStringLiteral("/") + docName + QStringLiteral(".rtf");
    if (!QFileInfo::exists(path)) {
        logger->logLocal(QStringLiteral("RTF не найден: %1").arg(path));
        m_controller->sendSimpleAck(2, 8, 1, false, 101);
        return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    logger->logLocal(QStringLiteral("Открыт ЭД %1 (метка %2)").arg(path, mark));
    m_controller->sendSimpleAck(2, 8, 1, true, 101);
}

void DirectiveHandler::handlePuskRazd(const PorMessage &message)
{
    Q_UNUSED(message)
    m_controller->m_logger->logLocal(QStringLiteral("ПУСК_РАЗД получен (MVP-заглушка)"));
}
