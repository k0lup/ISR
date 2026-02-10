#include "sectionlistdialog.h"

#include <QHBoxLayout>
#include <QInputDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

SectionListDialog::SectionListDialog(const QVector<SectionInfo> &sections, QWidget *parent)
    : QDialog(parent)
    , m_sections(sections)
{
    setWindowTitle(QStringLiteral("Список разделов"));
    resize(400, 300);

    auto *layout = new QVBoxLayout(this);
    m_listWidget = new QListWidget(this);
    for (const SectionInfo &section : m_sections) {
        m_listWidget->addItem(section.abbreviation);
    }
    layout->addWidget(m_listWidget);

    auto *buttonLayout = new QHBoxLayout;
    auto *selectBtn = new QPushButton(QStringLiteral("Выбрать"), this);
    auto *loadBtn = new QPushButton(QStringLiteral("Загрузить"), this);
    loadBtn->setEnabled(false);
    auto *findBtn = new QPushButton(QStringLiteral("Найти"), this);

    connect(selectBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(findBtn, &QPushButton::clicked, this, &SectionListDialog::onFind);

    buttonLayout->addWidget(selectBtn);
    buttonLayout->addWidget(loadBtn);
    buttonLayout->addWidget(findBtn);
    layout->addLayout(buttonLayout);
}

QString SectionListDialog::selectedSection() const
{
    auto *item = m_listWidget->currentItem();
    return item ? item->text() : QString();
}

void SectionListDialog::onFind()
{
    bool ok = false;
    const QString text = QInputDialog::getText(this, QStringLiteral("Поиск"), QStringLiteral("Префикс:"), QLineEdit::Normal, {}, &ok);
    if (!ok || text.trimmed().isEmpty()) {
        return;
    }

    for (int i = 0; i < m_listWidget->count(); ++i) {
        if (m_listWidget->item(i)->text().toUpper().startsWith(text.toUpper())) {
            m_listWidget->setCurrentRow(i);
            return;
        }
    }
    QMessageBox::information(this, QStringLiteral("Поиск"), QStringLiteral("Раздел не найден"));
}
