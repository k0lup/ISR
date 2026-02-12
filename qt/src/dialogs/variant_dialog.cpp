#include "variant_dialog.h"

#include <QDialogButtonBox>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include "../core/runtime_state.h"

VariantDialog::VariantDialog(RuntimeState *state, QWidget *parent) : QDialog(parent), m_state(state) {
    setWindowTitle(QStringLiteral("Variant"));

    auto *layout = new QVBoxLayout(this);
    m_list = new QListWidget(this);
    layout->addWidget(m_list);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_choose = buttons->button(QDialogButtonBox::Ok);
    m_choose->setText(QStringLiteral("Choose"));
    layout->addWidget(buttons);

    const int count = m_state ? m_state->variantCount : 0;
    for (int i = 1; i <= count; ++i) {
        m_list->addItem(QString::number(i));
    }

    if (m_state) {
        m_state->chosenVariant = -1;
    }

    connect(m_list, &QListWidget::itemDoubleClicked, this, [this]() {
        if (m_list->currentRow() >= 0) {
            if (m_state) {
                m_state->chosenVariant = m_list->currentRow() + 1;
            }
            accept();
        }
    });

    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (m_list->currentRow() >= 0) {
            if (m_state) {
                m_state->chosenVariant = m_list->currentRow() + 1;
            }
            accept();
        }
    });

    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
