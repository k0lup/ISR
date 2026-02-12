#include "selection_report_dialog.h"

#include <QDialogButtonBox>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include "../core/runtime_state.h"

SelectionReportDialog::SelectionReportDialog(RuntimeState *state, QWidget *parent)
    : QDialog(parent), m_state(state) {
    setWindowTitle(QStringLiteral("Selection Report"));

    auto *layout = new QVBoxLayout(this);
    m_list = new QListWidget(this);
    layout->addWidget(m_list);

    if (m_state) {
        for (const auto &item : m_state->selectionReportItems) {
            m_list->addItem(item);
        }
    }

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (m_state && m_list->currentRow() >= 0) {
            m_state->lastSelectionReportItem = m_list->currentItem()->text();
        }
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
