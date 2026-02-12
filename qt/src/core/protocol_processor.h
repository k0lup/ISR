#pragma once

#include <QObject>
#include <QByteArray>

#include "../portion.h"

class ProtocolProcessor final : public QObject {
    Q_OBJECT
public:
    explicit ProtocolProcessor(QObject *parent = nullptr);

    void processPacket(const QByteArray &packet);

signals:
    void log(const QString &line);
    void requestClose();
    void requestEndCurrentPartition(bool withFixation);
    void requestRefusalEndPartition();
    void requestPrisLoaded();
    void requestPreProcessOperation(const QString &op);
    void requestSendWindowTitle();
    void requestPartitionReady();
    void requestAnswerForKo(int atomType);
    void requestProtocolSaveError();
    void requestFinalizeOperationResult();
    void requestTitle(const QString &title);
    void requestProcessEdDir(const QString &path);
    void requestProcessNasrDir(const QString &path);
    void requestUnexpectedPacket(int rod, int sort, int atomType, const QString &atomText);

private:
    int m_nR = 0;
    int m_br = 0;
    bool m_prer = false;
    bool m_bSaveProtocol = false;
    bool m_bPrisProcessEndPartition = false;
    bool m_prVosst = false;
    int m_totalOperationsInPris = 0;
};
