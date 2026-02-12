#include "checkspo_dialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileInfo>
#include <QGroupBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include "../core/runtime_state.h"

CheckSpoDialog::CheckSpoDialog(RuntimeState *state, QWidget *parent) : QDialog(parent), m_state(state) {
    setWindowTitle(QStringLiteral("Check SPO"));
    resize(640, 420);

    auto *root = new QVBoxLayout(this);

    auto *group = new QGroupBox(QStringLiteral("Проверки"), this);
    auto *checksLayout = new QVBoxLayout(group);

    m_adjustment = new QCheckBox(QStringLiteral("Проверить файл подстройки"), group);
    m_structure = new QCheckBox(QStringLiteral("Проверить файл структуры"), group);
    m_cpkuPrograms = new QCheckBox(QStringLiteral("Проверить программы ЦПКУ"), group);
    m_ssbvsPrograms = new QCheckBox(QStringLiteral("Проверить программы ССБВС"), group);

    checksLayout->addWidget(m_adjustment);
    checksLayout->addWidget(m_structure);
    checksLayout->addWidget(m_cpkuPrograms);
    checksLayout->addWidget(m_ssbvsPrograms);

    root->addWidget(group);

    m_report = new QTextEdit(this);
    m_report->setReadOnly(true);
    root->addWidget(m_report);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    m_checkButton = buttons->addButton(QStringLiteral("Проверить"), QDialogButtonBox::ActionRole);
    root->addWidget(buttons);

    const auto onChanged = [this]() { updateCheckButtonState(); };
    connect(m_adjustment, &QCheckBox::clicked, this, onChanged);
    connect(m_structure, &QCheckBox::clicked, this, onChanged);
    connect(m_cpkuPrograms, &QCheckBox::clicked, this, onChanged);
    connect(m_ssbvsPrograms, &QCheckBox::clicked, this, onChanged);

    connect(m_checkButton, &QPushButton::clicked, this, [this]() { runChecks(); });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    if (m_state) {
        m_adjustment->setChecked(m_state->checkSpoAdjustmentEnabled);
        m_structure->setChecked(m_state->checkSpoStructureEnabled);
        m_cpkuPrograms->setChecked(m_state->checkSpoCpkuEnabled);
        m_ssbvsPrograms->setChecked(m_state->checkSpoSsbvsEnabled);
    }

    updateCheckButtonState();
}

void CheckSpoDialog::updateCheckButtonState() {
    if (!m_checkButton) {
        return;
    }

    const bool enabled = m_adjustment->isChecked() || m_structure->isChecked() ||
                         m_cpkuPrograms->isChecked() || m_ssbvsPrograms->isChecked();
    m_checkButton->setEnabled(enabled);
}

void CheckSpoDialog::runChecks() {
    if (!m_state) {
        return;
    }

    m_state->checkSpoAdjustmentEnabled = m_adjustment->isChecked();
    m_state->checkSpoStructureEnabled = m_structure->isChecked();
    m_state->checkSpoCpkuEnabled = m_cpkuPrograms->isChecked();
    m_state->checkSpoSsbvsEnabled = m_ssbvsPrograms->isChecked();

    m_report->clear();
    int errors = 0;

    auto appendCheck = [this, &errors](const QString &label, const QString &path) {
        const bool exists = QFileInfo::exists(path);
        if (exists) {
            m_report->append(QStringLiteral("[OK] %1: %2").arg(label, path));
        } else {
            m_report->append(QStringLiteral("[ERROR] %1: %2").arg(label, path));
            ++errors;
        }
    };

    if (m_adjustment->isChecked()) {
        appendCheck(QStringLiteral("Adjustment"), m_state->adjustmentFilePath);
    }
    if (m_structure->isChecked()) {
        appendCheck(QStringLiteral("Structure"), m_state->structureFilePath);
    }
    if (m_cpkuPrograms->isChecked()) {
        appendCheck(QStringLiteral("CPKU programs dir"), m_state->cpkuProgramsPath);
    }
    if (m_ssbvsPrograms->isChecked()) {
        appendCheck(QStringLiteral("SSBVS programs dir"), m_state->ssbvsProgramsPath);
    }

    m_state->checkSpoErrorCount = errors;
    m_report->append(QStringLiteral("Итог: ошибок %1").arg(errors));
}
