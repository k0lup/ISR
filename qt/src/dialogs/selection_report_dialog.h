#pragma once

#include <QDialog>

class QListWidget;
struct RuntimeState;

class SelectionReportDialog final : public QDialog {
    Q_OBJECT
public:
    explicit SelectionReportDialog(RuntimeState *state, QWidget *parent = nullptr);

private:
    RuntimeState *m_state = nullptr;
    QListWidget *m_list = nullptr;
};
