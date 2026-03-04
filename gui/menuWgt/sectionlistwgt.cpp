#include "sectionlistwgt.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include "logger/logging_categories.h"
#include <QLoggingCategory>

SectionListWgt::SectionListWgt(const QStringList& sections, QWidget* parent) :
    SectionListWgt(parent)
{
    list_of_sections_->addItems(sections);
}

SectionListWgt::SectionListWgt(QWidget* parent) :
    QDialog(parent)
{
    qCDebug(logCore) << "Инициализируем окно SectionListWgt";

    setWindowTitle("Выбор раздела");
    setModal(true);

    list_of_sections_ = new QListWidget(this);
    find_wgt_ = new FindWgt(this);

    QPushButton* select_btn = new QPushButton("Выбрать", this);
    QPushButton* search_btn = new QPushButton("Найти", this);

    QVBoxLayout *btn_layout = new QVBoxLayout;
    btn_layout->addWidget(select_btn);
    btn_layout->addWidget(search_btn);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(list_of_sections_);
    layout->addLayout(btn_layout);

    setLayout(layout);

    //используем QDialog подход в SectionListWgt::onSearchBtnClicked();
    //QObject::connect(find_wgt_, &FindWgt::searchTextEntered, this, &SectionListWgt::searchSection);
    QObject::connect(search_btn, &QPushButton::clicked, this, &SectionListWgt::onSearhBtnClicked);
    QObject::connect(select_btn, &QPushButton::clicked, this, &SectionListWgt::onSelectBtnClicked);

    qCDebug(logCore) << "Закончили инициализацию окна SectionListWgt";
}

void SectionListWgt::setSections(const QStringList& sections) {
    qCInfo(logCore) << "Выполнено обновления списка разделов";
    list_of_sections_->clear();
    list_of_sections_->addItems(sections);
}

int SectionListWgt::exec() {
    qCInfo(logCore) << "Вызвали окно SectionListWgt";
    selected_section_.clear();
    list_of_sections_->clearSelection();
    list_of_sections_->setCurrentRow(-1);

    int result = QDialog::exec();
    qCInfo(logCore) << "Закрыли окно SectionListWgt";

    return result;
}

void SectionListWgt::onSearhBtnClicked() {
    qCDebug(logCore) << "Нажали на кнопку Найти в Выборе раздела";
    if (find_wgt_->exec() == QDialog::Accepted) {
        searchSection(find_wgt_->getNameForSearch());
    }
    return;
}

void SectionListWgt::onSelectBtnClicked() {
    qCDebug(logCore) << "Нажали на кнопку Выбрать в Выборе раздела";
    QListWidgetItem *item = list_of_sections_->currentItem();
    if (item) {
        selected_section_ = item->text();
        qCInfo(logCore) << QString("Выбран раздел '%1'").arg(selected_section_);
        accept();
    } else {
        QString message = QString("Кнопка 'Выбрать' нажата, но не удалось найти ни одного раздела");
        qCInfo(logCore) << message;
        QMessageBox::warning(nullptr, "Предупреждение", message);
    }
}

void SectionListWgt::searchSection(const QString& section_name) {
    qCInfo(logCore) << QString("Запрошен поиск раздела '%1'").arg(section_name);
    list_of_sections_->clearSelection();
    list_of_sections_->setCurrentRow(-1);
    bool isFind{false};
    for (int i = 0; i < list_of_sections_->count(); ++i) {
        auto *it = list_of_sections_->item(i);
        if (it->text().contains(section_name, Qt::CaseInsensitive)) {
            it->setSelected(true);
            list_of_sections_->setCurrentItem(it);
            list_of_sections_->scrollToItem(it, QAbstractItemView::PositionAtCenter);
            isFind = true;
            qCInfo(logCore) << QString("Удалось найти раздел '%1' в списке разделов").arg(section_name);
            break;
        }
    }
    if (!isFind) {
        QString message = QString("Не удалось найти раздел '%1' в списке разделов").arg(section_name);
        qCInfo(logCore) << message;
        QMessageBox::warning(nullptr, "Предупреждение", message);
    }
}

QString SectionListWgt::getSelectedSection() const {
    return selected_section_;
}
