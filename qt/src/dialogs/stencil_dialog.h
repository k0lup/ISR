#pragma once

#include <QDialog>

class QLineEdit;
struct RuntimeState;

class StencilDialog final : public QDialog {
    Q_OBJECT
public:
    explicit StencilDialog(RuntimeState *state, QWidget *parent = nullptr);

private:
    RuntimeState *m_state = nullptr;
    QLineEdit *m_stencil = nullptr;
};
