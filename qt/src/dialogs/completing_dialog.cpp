#include "completing_dialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

#include "../core/runtime_state.h"

CompletingDialog::CompletingDialog(RuntimeState *state, QWidget *parent) : QDialog(parent), m_state(state) {
    setWindowTitle(QStringLiteral("Completing"));
    resize(360, 140);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(QStringLiteral("Completing stage"), this));

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (m_state) {
            m_state->completingAccepted = true;
        }
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, [this]() {
        if (m_state) {
            m_state->completingAccepted = false;
        }
        reject();
    });
    layout->addWidget(buttons);
}
