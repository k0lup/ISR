#pragma once

#include <QDialog>

class QListWidget;
class QPushButton;
struct RuntimeState;

class VariantDialog final : public QDialog {
    Q_OBJECT
public:
    explicit VariantDialog(RuntimeState *state, QWidget *parent = nullptr);

private:
    RuntimeState *m_state = nullptr;
    QListWidget *m_list = nullptr;
    QPushButton *m_choose = nullptr;
};
