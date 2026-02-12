#pragma once

#include <QDialog>

class QCheckBox;
struct RuntimeState;

class SettingsDialog final : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(RuntimeState *state, QWidget *parent = nullptr);

private:
    RuntimeState *m_state = nullptr;
    QCheckBox *m_allowBackCrossing = nullptr;
};
