#include "title_dialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "../core/runtime_state.h"
#include "../core/protocol_naming.h"

TitleDialog::TitleDialog(RuntimeState *state, QWidget *parent) : QDialog(parent), m_state(state) {
    setWindowTitle(QStringLiteral("Title"));

    auto *root = new QVBoxLayout(this);
    auto *form = new QFormLayout();

    m_title = new QLineEdit(this);
    m_protocolPart1 = new QLineEdit(this);
    m_protocolPart2 = new QLineEdit(this);
    m_rap = new QLineEdit(this);
    m_opPUAIS = new QLineEdit(this);
    m_opAIS = new QLineEdit(this);
    m_opTKCK = new QLineEdit(this);
    m_opNTKSI = new QLineEdit(this);
    m_opBS = new QLineEdit(this);

    form->addRow(QStringLiteral("Partition title"), m_title);
    form->addRow(QStringLiteral("Protocol name part 1"), m_protocolPart1);
    form->addRow(QStringLiteral("Protocol name part 2"), m_protocolPart2);
    form->addRow(QStringLiteral("RAP"), m_rap);
    form->addRow(QStringLiteral("Operator PUAIS"), m_opPUAIS);
    form->addRow(QStringLiteral("Operator AIS"), m_opAIS);
    form->addRow(QStringLiteral("Operator TKCK"), m_opTKCK);
    form->addRow(QStringLiteral("Operator NTKSI"), m_opNTKSI);
    form->addRow(QStringLiteral("Operator BS"), m_opBS);
    root->addLayout(form);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_okButton = buttons->button(QDialogButtonBox::Ok);
    root->addWidget(buttons);

    if (m_state) {
        m_state->titleAccepted = false;
        m_title->setText(m_state->partitionAbbr);
        m_protocolPart1->setText(m_state->partitionAbbr);
        m_protocolPart2->setText(m_state->maxProtocolNumber);
    }

    connect(m_title, &QLineEdit::textChanged, this, [this](const QString &value) {
        m_protocolPart1->setText(value);
        updateOkState();
    });
    connect(m_protocolPart1, &QLineEdit::textChanged, this, [this]() { updateOkState(); });

    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (m_title->text().isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("Title"), QStringLiteral("Введите название раздела"));
            return;
        }

        if (m_state) {
            m_state->partitionTitle = m_title->text();
            m_state->protocolNamePart1 = m_protocolPart1->text();
            m_state->protocolNamePart2 = m_protocolPart2->text();
            m_state->rapName = m_rap->text();
            m_state->operatorPUAISName = m_opPUAIS->text();
            m_state->operatorAISName = m_opAIS->text();
            m_state->operatorTKCKName = m_opTKCK->text();
            m_state->operatorNTKSIName = m_opNTKSI->text();
            m_state->operatorBSName = m_opBS->text();
            m_state->titleAccepted = true;
            m_state->currentProtocolName = protocol_naming::buildProtocolName(
                m_state->protocolNamePart1,
                m_state->protocolNamePart2,
                m_state->bisectNumber);
        }

        accept();
    });

    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    updateOkState();
}

void TitleDialog::updateOkState() {
    if (!m_okButton) {
        return;
    }

    m_okButton->setEnabled(!m_title->text().isEmpty() && !m_protocolPart1->text().isEmpty());
}
