#include "sp_dialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>

#include "../core/runtime_state.h"
#include "../core/protocol_naming.h"

SpDialog::SpDialog(RuntimeState *state, QWidget *parent) : QDialog(parent), m_state(state) {
    setWindowTitle(QStringLiteral("SP"));

    auto *root = new QVBoxLayout(this);
    auto *form = new QFormLayout();
    m_part1 = new QLineEdit(this);
    m_part2 = new QLineEdit(this);
    m_part3 = new QLineEdit(this);
    form->addRow(QStringLiteral("Protocol part 1"), m_part1);
    form->addRow(QStringLiteral("Protocol part 2"), m_part2);
    form->addRow(QStringLiteral("Protocol part 3"), m_part3);
    root->addLayout(form);

    if (m_state) {
        if (m_state->bisectNumber > m_state->maxBisectCount) {
            QMessageBox::warning(this,
                                 QStringLiteral("SP"),
                                 QStringLiteral("Деление раздела недоступно: превышен максимум."));
        }
        m_part1->setText(m_state->protocolNamePart1);
        m_part2->setText(m_state->protocolNamePart2);
        m_part3->setText(protocol_naming::formatBisectPart(m_state->bisectNumber));
    }

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_okButton = buttons->button(QDialogButtonBox::Ok);
    root->addWidget(buttons);

    connect(m_part1, &QLineEdit::textChanged, this, [this]() { updateOkState(); });
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (m_state) {
            m_state->spAccepted = true;
            m_state->protocolNamePart1 = m_part1->text();
            m_state->protocolNamePart2 = m_part2->text();
            m_state->currentProtocolName = protocol_naming::buildProtocolName(
                m_state->protocolNamePart1,
                m_state->protocolNamePart2,
                m_state->bisectNumber);
            m_state->bisectNumber += 1;
        }
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, [this]() {
        if (m_state) {
            m_state->spAccepted = false;
        }
        reject();
    });

    updateOkState();
}

void SpDialog::updateOkState() {
    if (m_okButton) {
        m_okButton->setEnabled(!m_part1->text().isEmpty());
    }
}
