#include "findwgt.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "logger/logging_categories.h"
#include <QLoggingCategory>

FindWgt::FindWgt(QWidget* parent) :
    QDialog(parent)
{
    qCDebug(logCore) << "Инициализация окна FindWgt";
    text_for_search_.clear();
    setWindowTitle("Поиск");
    setModal(true);

    QLabel* label = new QLabel("Аббревиатура:", this);
    name_for_search_text_line_ = new QLineEdit(this);


    QPushButton* cancelBtn = new QPushButton("Отмена", this);
    QPushButton* okBtn = new QPushButton("Ок", this);

    QHBoxLayout *text_layout = new QHBoxLayout;
    text_layout->addWidget(label);
    text_layout->addWidget(name_for_search_text_line_);

    QHBoxLayout *btn_layout = new QHBoxLayout;
    btn_layout->addWidget(cancelBtn);
    btn_layout->addWidget(okBtn);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(text_layout);
    layout->addLayout(btn_layout);

    setLayout(layout);

    QObject::connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    QObject::connect(okBtn, &QPushButton::clicked, this, &FindWgt::onOkClicked);

    qCDebug(logCore) << "Завершена инициализация окна FindWgt";
}

int FindWgt::exec() {
    qCDebug(logCore) << "Отображено окно FindWgt";

    text_for_search_.clear();
    name_for_search_text_line_->setText("");

    int result = QDialog::exec();

    qCDebug(logCore) << "Закрыто окно FindWgt";
    return result;
}

void FindWgt::onOkClicked() {
    text_for_search_ = name_for_search_text_line_->text();
    qCInfo(logCore) << QString("Запрошен поиск строки '%1'").arg(text_for_search_);
    emit searchTextEntered(text_for_search_);
    accept();
}

QString FindWgt::getNameForSearch() const {
    return text_for_search_;
}

//Вынести чтение файлов настройки .set в отдельный поток и проверить виджет загрузки ProgressBar
