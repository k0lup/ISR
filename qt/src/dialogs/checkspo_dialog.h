#pragma once

#include <QDialog>

class QCheckBox;
class QPushButton;
class QTextEdit;
struct RuntimeState;

class CheckSpoDialog final : public QDialog {
    Q_OBJECT
public:
    explicit CheckSpoDialog(RuntimeState *state, QWidget *parent = nullptr);

private:
    void updateCheckButtonState();
    void runChecks();

    RuntimeState *m_state = nullptr;
    QCheckBox *m_adjustment = nullptr;
    QCheckBox *m_structure = nullptr;
    QCheckBox *m_cpkuPrograms = nullptr;
    QCheckBox *m_ssbvsPrograms = nullptr;
    QPushButton *m_checkButton = nullptr;
    QTextEdit *m_report = nullptr;
};
