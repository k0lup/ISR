#pragma once

#include <QDialog>

class QListWidget;
class QPushButton;
struct RuntimeState;

class PartitionListDialog final : public QDialog {
    Q_OBJECT
public:
    explicit PartitionListDialog(RuntimeState *state, QWidget *parent = nullptr);

private:
    void refreshItems();

    RuntimeState *m_state = nullptr;
    QListWidget *m_list = nullptr;
    QPushButton *m_choose = nullptr;
    QPushButton *m_load = nullptr;
};
