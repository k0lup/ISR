#include "simple_dialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

SimpleDialog::SimpleDialog(const QString &title, const QString &description, QWidget *parent)
    : QDialog(parent) {
    setWindowTitle(title);
    resize(480, 220);

    auto *layout = new QVBoxLayout(this);
    m_label = new QLabel(description, this);
    m_label->setWordWrap(true);
    layout->addWidget(m_label);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    layout->addWidget(buttons);
}
