#include "end_dialog.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "../core/runtime_state.h"
#include "../core/protocol_naming.h"

EndDialog::EndDialog(RuntimeState *state, QWidget *parent) : QDialog(parent), m_state(state) {
    setWindowTitle(QStringLiteral("End Partition"));

    auto *layout = new QVBoxLayout(this);

    auto *protoLayout = new QHBoxLayout();
    m_part1 = new QLineEdit(this);
    m_part2 = new QLineEdit(this);
    m_part3 = new QLineEdit(this);
    protoLayout->addWidget(new QLabel(QStringLiteral("Protocol:"), this));
    protoLayout->addWidget(m_part1);
    protoLayout->addWidget(new QLabel(QStringLiteral("."), this));
    protoLayout->addWidget(m_part2);
    protoLayout->addWidget(new QLabel(QStringLiteral("-"), this));
    protoLayout->addWidget(m_part3);
    layout->addLayout(protoLayout);

    if (m_state) {
        m_part1->setText(m_state->protocolNamePart1);
        m_part2->setText(m_state->protocolNamePart2);
        m_part3->setText(protocol_naming::formatBisectPart(m_state->bisectNumber));
        m_part3->setVisible(m_state->bisectNumber > 1);
    }

    auto *buttons = new QDialogButtonBox(this);
    auto *withSave = buttons->addButton(QStringLiteral("End with save"), QDialogButtonBox::AcceptRole);
    auto *withoutSave = buttons->addButton(QStringLiteral("End without save"), QDialogButtonBox::DestructiveRole);
    buttons->addButton(QDialogButtonBox::Cancel);
    layout->addWidget(buttons);

    connect(withSave, &QPushButton::clicked, this, [this]() {
        if (m_state) {
            m_state->endResult = RuntimeState::EndResult::WithSave;
            m_state->protocolNamePart1 = m_part1->text();
            m_state->protocolNamePart2 = m_part2->text();
            m_state->currentProtocolName = protocol_naming::buildProtocolName(
                m_state->protocolNamePart1,
                m_state->protocolNamePart2,
                m_state->bisectNumber);
        }
        accept();
    });
    connect(withoutSave, &QPushButton::clicked, this, [this]() {
        if (m_state) {
            m_state->endResult = RuntimeState::EndResult::WithoutSave;
            m_state->protocolNamePart1 = m_part1->text();
            m_state->protocolNamePart2 = m_part2->text();
            m_state->currentProtocolName = protocol_naming::buildProtocolName(
                m_state->protocolNamePart1,
                m_state->protocolNamePart2,
                m_state->bisectNumber);
        }
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
