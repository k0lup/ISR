#include "find_dialog.h"

#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "../core/runtime_state.h"

FindDialog::FindDialog(RuntimeState *state, QWidget *parent) : QDialog(parent), m_state(state) {
    setWindowTitle(QStringLiteral("Find"));

    auto *layout = new QVBoxLayout(this);
    m_abbrEdit = new QLineEdit(this);
    layout->addWidget(m_abbrEdit);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_okButton = buttons->button(QDialogButtonBox::Ok);
    layout->addWidget(buttons);

    connect(m_abbrEdit, &QLineEdit::textChanged, this, [this]() { updateOkState(); });
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (m_state) {
            m_state->findAbbr = m_abbrEdit->text();
        }
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, [this]() {
        if (m_state) {
            m_state->findAbbr.clear();
        }
        reject();
    });

    m_abbrEdit->setFocus();
    updateOkState();
}

void FindDialog::updateOkState() {
    if (m_okButton) {
        m_okButton->setEnabled(!m_abbrEdit->text().isEmpty());
    }
}
