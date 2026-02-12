#pragma once

#include <QDialog>

class QLineEdit;
class QPushButton;
struct RuntimeState;

class FindDialog final : public QDialog {
    Q_OBJECT
public:
    explicit FindDialog(RuntimeState *state, QWidget *parent = nullptr);

private:
    void updateOkState();

    RuntimeState *m_state = nullptr;
    QLineEdit *m_abbrEdit = nullptr;
    QPushButton *m_okButton = nullptr;
};
