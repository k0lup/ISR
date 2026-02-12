#pragma once

#include <QDialog>

class QTimer;
struct RuntimeState;

class AboutDialog final : public QDialog {
    Q_OBJECT
public:
    explicit AboutDialog(RuntimeState *state, QWidget *parent = nullptr);

private:
    RuntimeState *m_state = nullptr;
    QTimer *m_autoCloseTimer = nullptr;
};
