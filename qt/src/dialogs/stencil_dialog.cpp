#include "stencil_dialog.h"

#include <QDialogButtonBox>
#include <QFile>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

#include "../core/runtime_state.h"
#include "../core/stencil_rules.h"

StencilDialog::StencilDialog(RuntimeState *state, QWidget *parent) : QDialog(parent), m_state(state) {
    setWindowTitle(QStringLiteral("Stencil"));

    auto *layout = new QVBoxLayout(this);
    m_stencil = new QLineEdit(this);
    layout->addWidget(m_stencil);

    if (m_state && m_state->stencilExists) {
        m_stencil->setText(m_state->stencil);
    }

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (!m_state) {
            reject();
            return;
        }

        const QString value = m_stencil->text().trimmed();
        QString validationError;
        if (!stencil_rules::isValid(value, &validationError)) {
            QMessageBox::warning(this, QStringLiteral("Stencil"), validationError);
            return;
        }

        if (!m_state->stencilFilePath.isEmpty()) {
            QFile::remove(m_state->stencilFilePath);
            QFile f(m_state->stencilFilePath);
            if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                QMessageBox::critical(this,
                                      QStringLiteral("Stencil"),
                                      QStringLiteral("Не удалось записать файл трафарета"));
                return;
            }
            const QByteArray utf8 = value.toUtf8();
            if (f.write(utf8) != utf8.size()) {
                QMessageBox::critical(this,
                                      QStringLiteral("Stencil"),
                                      QStringLiteral("Ошибка записи трафарета"));
                return;
            }
        }

        m_state->stencil = value;
        m_state->stencilExists = true;
        accept();
    });

    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
