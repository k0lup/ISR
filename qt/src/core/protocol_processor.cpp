#include "protocol_processor.h"

#include "por_codec.h"
#include "protocol_events.h"

ProtocolProcessor::ProtocolProcessor(QObject *parent) : QObject(parent) {}

void ProtocolProcessor::processPacket(const QByteArray &packet) {
    if (packet.size() < static_cast<int>(sizeof(POR))) {
        emit log(QStringLiteral("RX: packet too small"));
        return;
    }

    POR por{};
    if (!por_codec::tryDecodeHeader(packet, por)) {
        return;
    }

    const int rod = por.kindOfWork;
    const int sort = por.sortOfWork;
    const int atomType = por.atomType;
    const QByteArray atom = packet.mid(sizeof(POR));

    emit log(QStringLiteral("RX: kind=%1 sort=%2 atomType=%3 atomSize=%4")
                 .arg(rod)
                 .arg(sort)
                 .arg(atomType)
                 .arg(atom.size()));

    if (m_prer) {
        m_prer = false;
        m_bPrisProcessEndPartition = false;
    }

    if (rod == protocol_events::kRodWork && sort == protocol_events::sort::kMainAnswer) {
        if (atomType != 0 && (m_nR == 4 || m_nR == 3)) {
            emit log(QStringLiteral("Недопустимый ответ во время завершения раздела"));
            m_nR = 0;
            return;
        }

        if (m_totalOperationsInPris != 0) {
            emit requestAnswerForKo(atomType);
            return;
        }

        if (atomType != 0 && (m_nR == 5 || m_bSaveProtocol)) {
            if (m_bSaveProtocol) {
                emit log(QStringLiteral("Ошибка сохранения протокола"));
                emit requestProtocolSaveError();
                m_bSaveProtocol = false;
                m_br = 0;
                return;
            }
        }
    }

    if (rod == protocol_events::kRodWork && sort == protocol_events::sort::kCloseApp && atomType == 0) {
        emit log(QStringLiteral("Получена команда закрытия приложения"));
        emit requestClose();
        m_nR = 0;
        return;
    }

    if (rod == protocol_events::kRodWork && sort == protocol_events::sort::kMakeOperation && atomType == 0) {
        emit requestPreProcessOperation(QString::fromLocal8Bit(atom));
        return;
    }

    if (rod == protocol_events::kRodWork && sort == protocol_events::sort::kEndPartition) {
        m_bPrisProcessEndPartition = false;
        if (atomType == 0) {
            emit requestEndCurrentPartition(false);
        } else {
            emit requestRefusalEndPartition();
        }
        return;
    }

    if (rod == protocol_events::kRodWork && sort == protocol_events::sort::kPrisLoaded && atomType == 0) {
        emit requestPrisLoaded();
        return;
    }

    if (rod == protocol_events::kRodWork && sort == protocol_events::sort::kPartitionPrepared && atomType == 0) {
        if (m_br == 10) {
            m_br = 0;
            emit requestPartitionReady();
        } else {
            m_br = 7;
            if (!m_prVosst) {
                emit requestSendWindowTitle();
            }
        }
        return;
    }

    if (rod == protocol_events::kRodWork && sort == protocol_events::sort::kResult) {
        if (m_br == 11) {
            emit requestEndCurrentPartition(true);
            m_br = 0;
            return;
        }

        if (atomType == 1) {
            emit log(QStringLiteral("Ошибка выполнения команды из PRIS"));
            m_br = 0;
            return;
        }

        emit log(QStringLiteral("Результат операции обработан"));
        emit requestFinalizeOperationResult();
        m_br = 0;
        return;
    }

    if (rod == protocol_events::kRodTitle && sort == protocol_events::title::kSortSetTitle
        && atomType == protocol_events::title::kAtomTypeTitle) {
        emit requestTitle(QString::fromLocal8Bit(atom));
        return;
    }

    if (rod == protocol_events::kRodWork && sort == protocol_events::sort::kEdDirectory && atomType == 0) {
        emit requestProcessEdDir(QString::fromLocal8Bit(atom));
        return;
    }

    if (rod == protocol_events::kRodWork && sort == protocol_events::sort::kNasrDirectory && atomType == 0) {
        emit requestProcessNasrDir(QString::fromLocal8Bit(atom));
        return;
    }

    emit requestUnexpectedPacket(rod, sort, atomType, QString::fromLocal8Bit(atom));
}
