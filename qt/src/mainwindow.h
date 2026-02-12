#pragma once

#include <QMainWindow>

#include "core/runtime_state.h"

class QTextEdit;
class NetPrisClient;
class ProtocolProcessor;
class DialogManager;

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onPacketReceived(const QByteArray &packet);
    void onSocketError(const QString &message);
    void onSocketLog(const QString &message);

private:
    void setupUi();
    void registerProtocolHandlers();
    void setupDialogMenu();

    RuntimeState m_state;
    QTextEdit *m_log = nullptr;
    NetPrisClient *m_pris = nullptr;
    NetPrisClient *m_prot = nullptr;
    ProtocolProcessor *m_protocol = nullptr;
    DialogManager *m_dialogs = nullptr;
};
