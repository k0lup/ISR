#include "child_window_dialog.h"

#include <QDialogButtonBox>
#include <QHeaderView>
#include <QTableWidget>
#include <QVBoxLayout>

#include "../core/runtime_state.h"

ChildWindowDialog::ChildWindowDialog(RuntimeState *state, QWidget *parent)
    : QDialog(parent), m_state(state) {
    setWindowTitle(QStringLiteral("Child Window"));
    resize(760, 420);

    auto *layout = new QVBoxLayout(this);
    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels(
        {QStringLiteral("Section"), QStringLiteral("Cmd"), QStringLiteral("Status"), QStringLiteral("Comment")});
    m_table->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(m_table);

    if (m_state) {
        m_table->setRowCount(m_state->childWindowRows.size());
        for (int r = 0; r < m_state->childWindowRows.size(); ++r) {
            const auto &row = m_state->childWindowRows[r];
            for (int c = 0; c < row.size() && c < 4; ++c) {
                m_table->setItem(r, c, new QTableWidgetItem(row[c]));
            }
        }
    }

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}
