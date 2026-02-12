#include "mainwindow.h"

#include <QAction>
#include <QByteArray>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextEdit>
#include <QToolBar>

#include "core/dialog_manager.h"
#include "core/protocol_processor.h"
#include "net_pris_client.h"
#include "portion.h"
#include "core/por_codec.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_pris(new NetPrisClient(0x4020, this)),
      m_prot(new NetPrisClient(0x4024, this)), m_protocol(new ProtocolProcessor(this)),
      m_dialogs(new DialogManager(&m_state, this)) {
    m_state.variantCount = 5;
    m_state.partitionAbbr = QStringLiteral("SECTION");
    m_state.versionString = QStringLiteral("Qt-port-preview");
    m_state.aboutAutoCloseMs = 0;
    m_state.maxProtocolNumber = QStringLiteral("001");
    m_state.currentProtocolName = m_state.partitionAbbr + QStringLiteral(".") + m_state.maxProtocolNumber;
    m_state.adjustmentFilePath = QStringLiteral("./Adjustment.ini");
    m_state.structureFilePath = QStringLiteral("./Structure.txt");
    m_state.cpkuProgramsPath = QStringLiteral("./CPKU");
    m_state.ssbvsProgramsPath = QStringLiteral("./SSBVS");
    m_state.edDirectory = QStringLiteral("./ED");
    m_state.nasrDirectory = QStringLiteral("./NASR");
    m_state.allPartitions = {QStringLiteral("A01"), QStringLiteral("B02"), QStringLiteral("C03")};
    m_state.hiddenPartitions = {QStringLiteral("C03")};
    m_state.partitionCount = 1;
    m_state.stencilFilePath = QStringLiteral("./stencil.txt");
    m_state.reportCandidateSections = {QStringLiteral("RZD_A"), QStringLiteral("RZD_B"), QStringLiteral("RZD_C")};
    m_state.selectionReportItems = {QStringLiteral("SEL_1"), QStringLiteral("SEL_2")};
    m_state.openReports = {QStringLiteral("Partision_Rep"), QStringLiteral("Second_Rep"), QStringLiteral("Third_Rep")};
    m_state.partisionRepName = QStringLiteral("Partision_Rep");
    m_state.secondRepName = QStringLiteral("Second_Rep");
    m_state.thirdRepName = QStringLiteral("Third_Rep");
    m_state.childWindowRows = {
        {QStringLiteral("SEC-1"), QStringLiteral("CMD-01"), QStringLiteral("OK"), QStringLiteral("Loaded")},
        {QStringLiteral("SEC-2"), QStringLiteral("CMD-02"), QStringLiteral("WAIT"), QStringLiteral("Pending")}};

    setupUi();

    connect(m_pris, &NetPrisClient::packetReceived, this, &MainWindow::onPacketReceived);
    connect(m_prot, &NetPrisClient::packetReceived, this, &MainWindow::onPacketReceived);
    connect(m_pris, &NetPrisClient::socketError, this, &MainWindow::onSocketError);
    connect(m_prot, &NetPrisClient::socketError, this, &MainWindow::onSocketError);
    connect(m_pris, &NetPrisClient::socketLog, this, &MainWindow::onSocketLog);
    connect(m_prot, &NetPrisClient::socketLog, this, &MainWindow::onSocketLog);

    connect(m_dialogs, &DialogManager::log, this, [this](const QString &line) {
        m_log->append(QStringLiteral("[DIALOGS] %1").arg(line));
    });

    m_dialogs->createAll();
    m_log->append(QStringLiteral("[STATE] report slots initialized: %1/%2/%3")
                      .arg(m_state.partisionRepName, m_state.secondRepName, m_state.thirdRepName));
    registerProtocolHandlers();

    m_pris->open();
    m_prot->open();
}

void MainWindow::setupUi() {
    setWindowTitle(QStringLiteral("ISR (Qt 5.15)") );
    resize(1200, 760);

    auto *toolbar = addToolBar(QStringLiteral("Main"));
    auto *reconnectAction = toolbar->addAction(QStringLiteral("Reconnect"));
    connect(reconnectAction, &QAction::triggered, this, [this]() {
        m_pris->close();
        m_prot->close();
        m_pris->open();
        m_prot->open();
    });

    auto *workMenu = menuBar()->addMenu(QStringLiteral("Work"));
    workMenu->addAction(reconnectAction);

    setupDialogMenu();

    m_log = new QTextEdit(this);
    m_log->setReadOnly(true);
    setCentralWidget(m_log);

    statusBar()->showMessage(QStringLiteral("Ready"));
}

void MainWindow::setupDialogMenu() {
    auto *dialogsMenu = menuBar()->addMenu(QStringLiteral("Dialogs"));

    const QStringList keys = m_dialogs->keys();

    for (const auto &key : keys) {
        dialogsMenu->addAction(key, this, [this, key]() { m_dialogs->showDialog(key); });
    }
}

void MainWindow::registerProtocolHandlers() {
    connect(m_protocol, &ProtocolProcessor::log, this, [this](const QString &line) {
        m_log->append(QStringLiteral("[PROTO] %1").arg(line));
    });
    connect(m_protocol, &ProtocolProcessor::requestClose, this, [this]() { close(); });
    connect(m_protocol, &ProtocolProcessor::requestEndCurrentPartition, this, [this](bool withFixation) {
        m_log->append(QStringLiteral("[PROTO] end current partition, fixation=%1").arg(withFixation));
    });
    connect(m_protocol, &ProtocolProcessor::requestRefusalEndPartition, this, [this]() {
        m_log->append(QStringLiteral("[PROTO] refusal end partition"));
    });
    connect(m_protocol, &ProtocolProcessor::requestPrisLoaded, this, [this]() {
        m_log->append(QStringLiteral("[PROTO] PRIS loaded"));
    });
    connect(m_protocol, &ProtocolProcessor::requestPreProcessOperation, this, [this](const QString &op) {
        m_log->append(QStringLiteral("[PROTO] preprocess operation: %1").arg(op));
    });
    connect(m_protocol, &ProtocolProcessor::requestSendWindowTitle, this, [this]() {
        m_log->append(QStringLiteral("[PROTO] send window title to PRIS"));
    });
    connect(m_protocol, &ProtocolProcessor::requestPartitionReady, this, [this]() {
        m_log->append(QStringLiteral("[PROTO] partition ready"));
    });
    connect(m_protocol, &ProtocolProcessor::requestAnswerForKo, this, [this](int atomType) {
        m_log->append(QStringLiteral("[PROTO] answer for KO, atomType=%1").arg(atomType));
    });
    connect(m_protocol, &ProtocolProcessor::requestProtocolSaveError, this, [this]() {
        m_log->append(QStringLiteral("[PROTO] protocol save error"));
    });
    connect(m_protocol, &ProtocolProcessor::requestFinalizeOperationResult, this, [this]() {
        if (m_state.currentProtocolName.size() > 33) {
            m_log->append(QStringLiteral("[PROTO][ERROR] protocol name is too long: %1")
                              .arg(m_state.currentProtocolName));
            return;
        }

        m_log->append(QStringLiteral("[PROTO] protocol title: %1").arg(m_state.partitionTitle));
        m_log->append(QStringLiteral("[PROTO] protocol name: %1").arg(m_state.currentProtocolName));
        m_log->append(QStringLiteral("[PROTO] RAP: %1").arg(m_state.rapName));
        m_log->append(QStringLiteral("[PROTO] operator PUAIS: %1").arg(m_state.operatorPUAISName));
        m_log->append(QStringLiteral("[PROTO] operator AIS: %1").arg(m_state.operatorAISName));
        m_log->append(QStringLiteral("[PROTO] operator TKCK: %1").arg(m_state.operatorTKCKName));
        m_log->append(QStringLiteral("[PROTO] operator NTKSI: %1").arg(m_state.operatorNTKSIName));
        m_log->append(QStringLiteral("[PROTO] operator BS: %1").arg(m_state.operatorBSName));
    });
    connect(m_protocol, &ProtocolProcessor::requestTitle, this, [this](const QString &title) {
        m_state.partitionTitle = title;
        m_log->append(QStringLiteral("[PROTO] set partition title: %1").arg(title));
    });
    connect(m_protocol, &ProtocolProcessor::requestProcessEdDir, this, [this](const QString &path) {
        m_state.edDirectory = path;
        m_log->append(QStringLiteral("[PROTO] ED directory: %1").arg(path));
    });
    connect(m_protocol, &ProtocolProcessor::requestProcessNasrDir, this, [this](const QString &path) {
        m_state.nasrDirectory = path;
        m_log->append(QStringLiteral("[PROTO] NASR directory: %1").arg(path));
    });
    connect(m_protocol, &ProtocolProcessor::requestUnexpectedPacket, this,
            [this](int rod, int sort, int atomType, const QString &atomText) {
                m_log->append(QStringLiteral("[PROTO][WARN] unexpected packet: kind=%1 sort=%2 atomType=%3 atom=%4")
                                  .arg(rod)
                                  .arg(sort)
                                  .arg(atomType)
                                  .arg(atomText));
            });
    connect(m_protocol, &ProtocolProcessor::requestPartitionReady, this, [this]() {
        m_log->append(QStringLiteral("[STATE] current protocol name: %1").arg(m_state.currentProtocolName));
    });
}

void MainWindow::onPacketReceived(const QByteArray &packet) {
    if (packet.size() < static_cast<int>(sizeof(POR))) {
        m_log->append(QStringLiteral("Corrupted packet: too small"));
        return;
    }

    POR por{};
    if (!por_codec::tryDecodeHeader(packet, por)) {
        return;
    }

    m_log->append(
        QStringLiteral("RX packet | kind=%1 sort=%2 atomType=%3 atomCount=%4 size=%5")
            .arg(por.kindOfWork)
            .arg(por.sortOfWork)
            .arg(por.atomType)
            .arg(por.atomQuant)
            .arg(packet.size()));

    m_protocol->processPacket(packet);
}

void MainWindow::onSocketError(const QString &message) {
    m_log->append(QStringLiteral("[ERROR] %1").arg(message));
}

void MainWindow::onSocketLog(const QString &message) {
    m_log->append(QStringLiteral("[LOG] %1").arg(message));
}
