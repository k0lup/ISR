#pragma once

#include <QDialog>

class QLineEdit;
struct RuntimeState;

class EndDialog final : public QDialog {
    Q_OBJECT
public:
    explicit EndDialog(RuntimeState *state, QWidget *parent = nullptr);

private:
    RuntimeState *m_state = nullptr;
    QLineEdit *m_part1 = nullptr;
    QLineEdit *m_part2 = nullptr;
    QLineEdit *m_part3 = nullptr;
};
