#include "settings_dialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "../core/runtime_state.h"

SettingsDialog::SettingsDialog(RuntimeState *state, QWidget *parent) : QDialog(parent), m_state(state) {
    setWindowTitle(QStringLiteral("Settings"));

    auto *layout = new QVBoxLayout(this);
    m_allowBackCrossing = new QCheckBox(QStringLiteral("Allow back crossing"), this);
    layout->addWidget(m_allowBackCrossing);

    if (m_state) {
        m_allowBackCrossing->setChecked(m_state->allowBackCrossing);
    }

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    auto *reconfigureButton = buttons->addButton(QStringLiteral("Reconfigure menu"), QDialogButtonBox::ActionRole);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (m_state) {
            m_state->allowBackCrossing = m_allowBackCrossing->isChecked();
        }
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(reconfigureButton, &QPushButton::clicked, this, [this]() {
        if (m_state) {
            m_state->menuReconfigureRequested = true;
        }
    });
}
