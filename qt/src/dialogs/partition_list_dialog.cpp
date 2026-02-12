#include "partition_list_dialog.h"

#include <QDialogButtonBox>
#include <QInputDialog>
#include <QListWidget>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "../core/runtime_state.h"

PartitionListDialog::PartitionListDialog(RuntimeState *state, QWidget *parent)
    : QDialog(parent), m_state(state) {
    setWindowTitle(QStringLiteral("Partitions List"));
    resize(520, 420);

    auto *layout = new QVBoxLayout(this);
    m_list = new QListWidget(this);
    layout->addWidget(m_list);

    auto *buttons = new QDialogButtonBox(this);
    m_choose = buttons->addButton(QStringLiteral("Choose"), QDialogButtonBox::AcceptRole);
    m_load = buttons->addButton(QStringLiteral("Load"), QDialogButtonBox::ActionRole);
    auto *find = buttons->addButton(QStringLiteral("Find"), QDialogButtonBox::ActionRole);
    buttons->addButton(QDialogButtonBox::Cancel);
    layout->addWidget(buttons);

    refreshItems();

    connect(m_list, &QListWidget::itemDoubleClicked, this, [this]() {
        if (!m_choose->isEnabled()) {
            if (m_load->isEnabled()) {
                m_load->click();
            }
            return;
        }
        m_choose->click();
    });

    connect(m_choose, &QPushButton::clicked, this, [this]() {
        if (!m_state || m_list->currentRow() < 0) {
            return;
        }
        m_state->selectedPartitionAbbr = m_list->currentItem()->text();
        m_state->partitionListResult = RuntimeState::PartitionListResult::Choose;
        accept();
    });

    connect(m_load, &QPushButton::clicked, this, [this]() {
        if (!m_state || m_list->currentRow() < 0) {
            return;
        }
        m_state->selectedPartitionAbbr = m_list->currentItem()->text();
        m_state->partitionListResult = RuntimeState::PartitionListResult::Load;
        accept();
    });

    connect(find, &QPushButton::clicked, this, [this]() {
        if (!m_state) {
            return;
        }
        bool ok = false;
        const QString abbr = QInputDialog::getText(this,
                                                   QStringLiteral("Find"),
                                                   QStringLiteral("Abbreviation"),
                                                   QLineEdit::Normal,
                                                   m_state->findAbbr,
                                                   &ok);
        if (!ok || abbr.isEmpty()) {
            return;
        }
        m_state->findAbbr = abbr;

        for (int i = 0; i < m_list->count(); ++i) {
            const auto text = m_list->item(i)->text();
            if (text.startsWith(abbr, Qt::CaseSensitive)) {
                m_list->setCurrentRow(i);
                return;
            }
        }

        QMessageBox::warning(this, QStringLiteral("Find"), QStringLiteral("Раздел не найден"));
    });

    connect(buttons, &QDialogButtonBox::rejected, this, [this]() {
        if (m_state) {
            m_state->partitionListResult = RuntimeState::PartitionListResult::Cancel;
        }
        reject();
    });
}

void PartitionListDialog::refreshItems() {
    m_list->clear();

    if (!m_state) {
        return;
    }

    for (const auto &abbr : m_state->allPartitions) {
        if (!m_state->hiddenPartitions.contains(abbr)) {
            m_list->addItem(abbr);
        }
    }

    if (m_list->count() > 0) {
        m_list->setCurrentRow(0);
    }

    const bool canChoose = m_state->partitionCount == 0;
    m_choose->setEnabled(canChoose);
    m_load->setEnabled(!canChoose);
    m_state->partitionListResult = RuntimeState::PartitionListResult::Cancel;
}
