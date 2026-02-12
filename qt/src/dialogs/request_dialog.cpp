#include "request_dialog.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include "../core/runtime_state.h"

namespace {
QString captionForType(const RuntimeState *s) {
    if (!s) {
        return QStringLiteral("Request");
    }

    switch (s->currentType) {
    case RuntimeState::OperationType::Report:
        return s->reportName;
    case RuntimeState::OperationType::Command:
        return s->commandName;
    case RuntimeState::OperationType::Message:
        return s->messageName;
    case RuntimeState::OperationType::DipolOperation:
        return s->dipolOperationName;
    case RuntimeState::OperationType::ISROperation:
        return s->isrOperationName;
    }

    return QStringLiteral("Request");
}
} // namespace

RequestDialog::RequestDialog(RuntimeState *state, QWidget *parent) : QDialog(parent), m_state(state) {
    setWindowTitle(captionForType(m_state));

    auto *layout = new QVBoxLayout(this);
    m_memo = new QTextEdit(this);
    m_memo->setReadOnly(true);
    if (m_state) {
        m_memo->setPlainText(m_state->requestOperationLines.join('\n'));
        m_state->makeOperation = false;
    }
    layout->addWidget(m_memo);

    auto *buttons = new QDialogButtonBox(this);
    auto *makeBtn = buttons->addButton(QStringLiteral("Make"), QDialogButtonBox::AcceptRole);
    auto *cancelBtn = buttons->addButton(QDialogButtonBox::Cancel);
    Q_UNUSED(cancelBtn);

    connect(makeBtn, &QPushButton::clicked, this, [this]() {
        if (m_state) {
            m_state->makeOperation = true;
        }
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    layout->addWidget(buttons);
}
