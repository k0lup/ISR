#include "stencildialog.h"

#include <QDialogButtonBox>
#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QVBoxLayout>

StencilDialog::StencilDialog(const QString &currentStencil, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Трафарет"));

    auto *layout = new QVBoxLayout(this);
    m_edit = new QLineEdit(this);
    m_edit->setText(currentStencil);
    auto *validator = new QRegularExpressionValidator(QRegularExpression(QStringLiteral("^[A-Za-zА-Яа-я0-9]+-[0-9]{4}$")), m_edit);
    m_edit->setValidator(validator);
    layout->addWidget(m_edit);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

QString StencilDialog::stencil() const
{
    return m_edit->text().trimmed();
}
