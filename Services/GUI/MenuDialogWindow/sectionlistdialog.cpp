#include "sectionlistdialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QMessageBox>

#include "finddialog.h"

#include <Logger/logger.h>
#include "logging_categories.h"

SectionListDialog::SectionListDialog(QWidget* parent) : QDialog(parent)
{
    qCInfo(logGUI) << "Вызван конструктор SectionListDialog";
    setWindowTitle("Список разделов");
    resize(650, 550);

    createWidgets();
    createLayouts();
    createConnections();
    qCInfo(logGUI) << "Завершен конструктор SectionListDialog";
}

void SectionListDialog::createWidgets()
{
    qCInfo(logGUI) << "Вызван метод createWidgets()";

    section_list_ = new QListWidget(this);

    section_list_->setSelectionMode(QAbstractItemView::SingleSelection);

    select_button_  = new QPushButton("Выбрать");
    load_button_    = new QPushButton("Загрузить");
    find_button_    = new QPushButton("Найти");
    cancel_button_  = new QPushButton("Отмена");

    select_button_->setEnabled(false);
    select_button_->setEnabled(false);

    const int button_width = 140; //чтобы кнопки были одного размера

    select_button_->setFixedWidth(button_width);
    load_button_->setFixedWidth(button_width);
    find_button_->setFixedWidth(button_width);
    cancel_button_->setFixedWidth(button_width);

    qCInfo(logGUI) << "Завершен метод createWidgets()";
}

void SectionListDialog::createLayouts()
{
    qCInfo(logGUI) << "Вызван метод createLayouts()";

    QHBoxLayout* h_box_layout = new QHBoxLayout();
    QVBoxLayout* btn_layout = new QVBoxLayout();

    btn_layout->addWidget(select_button_);
    btn_layout->addWidget(load_button_);
    btn_layout->addWidget(find_button_);
    btn_layout->addWidget(cancel_button_);

    h_box_layout->addWidget(section_list_);
    h_box_layout->addLayout(btn_layout);

    setLayout(h_box_layout);

    qCInfo(logGUI) << "Завершен метод createLayouts()";
}

void SectionListDialog::createConnections()
{
    qCInfo(logGUI) << "Вызван метод createConnections()";

    QObject::connect(select_button_, &QPushButton::clicked, this, &SectionListDialog::onSelectButtonClicked);
    QObject::connect(load_button_, &QPushButton::clicked, this, &SectionListDialog::onLoadButtonClicked);
    QObject::connect(find_button_, &QPushButton::clicked, this, &SectionListDialog::onFindButtonClicked);
    QObject::connect(cancel_button_, &QPushButton::clicked, this, &SectionListDialog::onCancelButtonClicked);

    qCInfo(logGUI) << "Завершен метод createConnections()";
}

void SectionListDialog::onSelectButtonClicked()
{
    qCInfo(logGUI) << "Вызван обработчик кнопки 'Выбрать'";

    const auto *item = section_list_->currentItem();

    if (!item) {
        return;
    }

    selected_section_ = item->text();

    accept();

    qCInfo(logGUI) << "Завершен обработчик кнопки 'Выбрать'";
}

void SectionListDialog::onLoadButtonClicked()
{
    qCInfo(logGUI) << "Вызван обработчик кнопки 'Загрузить'";

    const auto *item = section_list_->currentItem();

    if (!item) {
        return;
    }

    selected_section_ = item->text();

    accept();

    qCInfo(logGUI) << "Завершен обработчик кнопки 'Загрузить'";
}

//Проработать вопрос вложенного QDialog::FIND!
void SectionListDialog::onFindButtonClicked()
{
    qCInfo(logGUI) << "Вызван обработчик кнопки 'Найти'";

    FindDialog find_dialog("Найти раздел", "Аббревиатура:");
    if (find_dialog.exec() == QDialog::Accepted) {
        findAndSelect(find_dialog.getTxtForSearch());
    }

    qCInfo(logGUI) << "Завершен обработчик кнопки 'Найти'";
}

void SectionListDialog::onCancelButtonClicked()
{
    qCInfo(logGUI) << "Вызван обработчик кнопки 'Отмена'";

    reject();

    qCInfo(logGUI) << "Завершен обработчик кнопки 'Отмена'";
}

int SectionListDialog::exec(const SectionListDialog::Mode mode)
{
    qCInfo(logGUI) << "Вызван метод exec()";
    selected_section_.clear();

    if (mode == Mode::SELECT) {
        select_button_->setEnabled(true);
        load_button_->setEnabled(false);
        qCInfo(logGUI) << "Режим окна - 'Выбрать'. Кнпока выбрать включена, кнопка загрузить отключена";
    } else {
        select_button_->setEnabled(false);
        load_button_->setEnabled(true);
        qCInfo(logGUI) << "Режим окна - 'Загрузить'. Кнпока выбрать отключена, кнопка загрузить включена";
    }

    qCInfo(logGUI) << "Передаем работу в метод QDialog::exec()";
    return QDialog::exec();
}

void SectionListDialog::findAndSelect(const QString& abbrev)
{
    qCInfo(logGUI) << "Вызван метод findAndSelect()";

    const QList<QListWidgetItem*> items = section_list_->findItems(abbrev, Qt::MatchContains);

    if (items.isEmpty()) {
        QMessageBox::warning(nullptr, "Предупреждение", "Совпадений не найдено!");
        qCInfo(logGUI) << "Завершен метод findAndSelect() - совпадений нет";
        return;
    }

    QListWidgetItem* item = items.first();

    section_list_->setCurrentItem(item);
    section_list_->scrollToItem(item);

    qCInfo(logGUI) << "Завершен метод findAndSelect()";
}

void SectionListDialog::setSections(const QStringList& sections_list)
{
    QStringList sorted_section_list = sections_list;
    sorted_section_list.sort();
    section_list_->addItems(sorted_section_list);

    if (section_list_->count() > 0) {
        section_list_->setCurrentRow(0);
    }
}

QString SectionListDialog::getSelectedSection()
{
    return selected_section_;
}
