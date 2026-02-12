#pragma once

#include <QDialog>

class QTextEdit;
struct RuntimeState;

class RequestDialog final : public QDialog {
    Q_OBJECT
public:
    explicit RequestDialog(RuntimeState *state, QWidget *parent = nullptr);

private:
    RuntimeState *m_state = nullptr;
    QTextEdit *m_memo = nullptr;
};
