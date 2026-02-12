#include "partition_report_dialog.h"

#include <QDialogButtonBox>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

#include "../core/runtime_state.h"
#include "../core/report_slots.h"

PartitionReportDialog::PartitionReportDialog(RuntimeState *state, QWidget *parent)
    : QDialog(parent), m_state(state) {
    setWindowTitle(QStringLiteral("Partition Report"));
    resize(500, 340);

    auto *layout = new QVBoxLayout(this);
    m_list = new QListWidget(this);
    layout->addWidget(m_list);

    auto *slotInfo = new QLabel(this);
    if (m_state) {
        const QString s1 = m_state->partisionRepName.isEmpty() ? QStringLiteral("<free>") : m_state->partisionRepName;
        const QString s2 = m_state->secondRepName.isEmpty() ? QStringLiteral("<free>") : m_state->secondRepName;
        const QString s3 = m_state->thirdRepName.isEmpty() ? QStringLiteral("<free>") : m_state->thirdRepName;
        slotInfo->setText(QStringLiteral("Slots: [1]=%1, [2]=%2, [3]=%3").arg(s1, s2, s3));
    }
    layout->addWidget(slotInfo);

    if (m_state) {
        for (const auto &section : m_state->reportCandidateSections) {
            m_list->addItem(section);
        }
    }

    auto *buttons = new QDialogButtonBox(this);
    auto *loadBtn = buttons->addButton(QStringLiteral("Load"), QDialogButtonBox::AcceptRole);
    buttons->addButton(QDialogButtonBox::Cancel);
    layout->addWidget(buttons);

    connect(loadBtn, &QPushButton::clicked, this, [this]() {
        if (!m_state || m_list->currentRow() < 0) {
            QMessageBox::warning(this, QStringLiteral("Partition Report"), QStringLiteral("Выберите раздел"));
            return;
        }

        const QString selected = m_list->currentItem()->text();
        if (m_state->openReports.contains(selected)) {
            QMessageBox::warning(this, QStringLiteral("Partition Report"), QStringLiteral("Этот отчет уже открыт"));
            return;
        }

        if (report_slots::freeSlotIndex(*m_state) < 0) {
            QMessageBox::warning(this,
                                 QStringLiteral("Partition Report"),
                                 QStringLiteral("Уже открыто 3 отчета"));
            return;
        }

        int assignedSlot = -1;
        report_slots::assignToFreeSlot(*m_state, selected, &assignedSlot);
        m_state->lastLoadedReport = selected;
        m_state->lastLoadedReportSlot = assignedSlot;
        accept();
    });

    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
