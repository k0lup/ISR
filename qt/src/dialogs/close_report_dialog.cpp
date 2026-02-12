#include "close_report_dialog.h"

#include <QDialogButtonBox>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "../core/runtime_state.h"
#include "../core/report_slots.h"

CloseReportDialog::CloseReportDialog(RuntimeState *state, QWidget *parent)
    : QDialog(parent), m_state(state) {
    setWindowTitle(QStringLiteral("Close Report"));
    resize(460, 320);

    auto *layout = new QVBoxLayout(this);
    m_list = new QListWidget(this);
    layout->addWidget(m_list);

    if (m_state) {
        for (const auto &rep : m_state->openReports) {
            m_list->addItem(rep);
        }
    }

    auto *buttons = new QDialogButtonBox(this);
    auto *closeBtn = buttons->addButton(QStringLiteral("Close selected"), QDialogButtonBox::AcceptRole);
    buttons->addButton(QDialogButtonBox::Cancel);
    layout->addWidget(buttons);

    connect(closeBtn, &QPushButton::clicked, this, [this]() {
        if (!m_state || m_list->currentRow() < 0) {
            QMessageBox::warning(this, QStringLiteral("Close Report"), QStringLiteral("Выберите отчет"));
            return;
        }

        const QString reportName = m_list->currentItem()->text();
        m_state->lastClosedReport = reportName;
        m_state->lastClosedReportSlot = report_slots::clearReport(*m_state, reportName);
        m_list->takeItem(m_list->currentRow());
        accept();
    });

    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
