#pragma once

#include <QDialog>

class QLineEdit;
class QPushButton;
struct RuntimeState;

class SpDialog final : public QDialog {
    Q_OBJECT
public:
    explicit SpDialog(RuntimeState *state, QWidget *parent = nullptr);

private:
    void updateOkState();

    RuntimeState *m_state = nullptr;
    QLineEdit *m_part1 = nullptr;
    QLineEdit *m_part2 = nullptr;
    QLineEdit *m_part3 = nullptr;
    QPushButton *m_okButton = nullptr;
};
