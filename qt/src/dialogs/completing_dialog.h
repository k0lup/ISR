#pragma once

#include <QDialog>

struct RuntimeState;

class CompletingDialog final : public QDialog {
    Q_OBJECT
public:
    explicit CompletingDialog(RuntimeState *state, QWidget *parent = nullptr);

private:
    RuntimeState *m_state = nullptr;
};
