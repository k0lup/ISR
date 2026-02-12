#pragma once

#include <QDialog>

class QTableWidget;
struct RuntimeState;

class ChildWindowDialog final : public QDialog {
    Q_OBJECT
public:
    explicit ChildWindowDialog(RuntimeState *state, QWidget *parent = nullptr);

private:
    RuntimeState *m_state = nullptr;
    QTableWidget *m_table = nullptr;
};
