#include "headerdialog.h"

#include <QDialogButtonBox>
#include <QLineEdit>
#include <QVBoxLayout>

HeaderDialog::HeaderDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Заголовок раздела"));

    auto *layout = new QVBoxLayout(this);
    m_edit = new QLineEdit(this);
    m_edit->setMaxLength(64);
    layout->addWidget(m_edit);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

QString HeaderDialog::headerText() const
{
    return m_edit->text().trimmed();
}
