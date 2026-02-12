#include "about_dialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>

#include "../core/runtime_state.h"

AboutDialog::AboutDialog(RuntimeState *state, QWidget *parent) : QDialog(parent), m_state(state) {
    setWindowTitle(QStringLiteral("About"));
    resize(420, 260);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(QStringLiteral("ISR"), this));
    layout->addWidget(new QLabel(QStringLiteral("Version: %1").arg(m_state ? m_state->versionString : QStringLiteral("unknown")), this));
    layout->addWidget(new QLabel(QStringLiteral("Copyright: ISR legacy migration"), this));
    layout->addWidget(new QLabel(QStringLiteral("Comments: Qt 5.15 migration build"), this));

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    layout->addWidget(buttons);

    if (m_state && m_state->aboutAutoCloseMs > 0) {
        m_autoCloseTimer = new QTimer(this);
        m_autoCloseTimer->setSingleShot(true);
        connect(m_autoCloseTimer, &QTimer::timeout, this, &QDialog::accept);
        m_autoCloseTimer->start(m_state->aboutAutoCloseMs);
    }
}
