#pragma once

#include <QDialog>

class QLineEdit;
class QPushButton;
struct RuntimeState;

class TitleDialog final : public QDialog {
    Q_OBJECT
public:
    explicit TitleDialog(RuntimeState *state, QWidget *parent = nullptr);

private:
    void updateOkState();

    RuntimeState *m_state = nullptr;
    QLineEdit *m_title = nullptr;
    QLineEdit *m_protocolPart1 = nullptr;
    QLineEdit *m_protocolPart2 = nullptr;
    QLineEdit *m_rap = nullptr;
    QLineEdit *m_opPUAIS = nullptr;
    QLineEdit *m_opAIS = nullptr;
    QLineEdit *m_opTKCK = nullptr;
    QLineEdit *m_opNTKSI = nullptr;
    QLineEdit *m_opBS = nullptr;
    QPushButton *m_okButton = nullptr;
};
