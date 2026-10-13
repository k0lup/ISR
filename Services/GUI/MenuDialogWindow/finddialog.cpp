#include "finddialog.h"
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <Logger/logger.h>
#include "logging_categories.h"

FindDialog::FindDialog(QString title_window, QString label_name, QWidget* parent) : QDialog(parent)
{
    qCInfo(logGUI) << "Вызван конструктор FindDialog";

    createWindow();

    resize(400, 300);
    setWindowTitle(title_window);
    label_->setText(label_name);

    qCInfo(logGUI) << "Завершен конструктор FindDialog";
}

void FindDialog::createWindow()
{
    qCInfo(logGUI) << "Вызван метод createWindow()";

    label_ = new QLabel(this);
    line_edit_ = new QLineEdit(this);
    cancel_button_ = new QPushButton("Отмена", this);
    ok_btn_ = new QPushButton("Ок", this);

    QHBoxLayout* search_lines_layout = new QHBoxLayout();
    QHBoxLayout* btn_layout = new QHBoxLayout();
    QVBoxLayout* layout = new QVBoxLayout();

    search_lines_layout->addWidget(label_);
    search_lines_layout->addWidget(line_edit_);

    btn_layout->addWidget(cancel_button_);
    btn_layout->addWidget(ok_btn_);

    layout->addLayout(search_lines_layout);
    layout->addLayout(btn_layout);

    setLayout(layout);

    QObject::connect(ok_btn_, &QPushButton::clicked, this, &FindDialog::onOkBtnClicked);
    QObject::connect(cancel_button_, &QPushButton::clicked, this, &FindDialog::onCancelBtnClicked);
    QObject::connect(line_edit_, &QLineEdit::textChanged, this, &FindDialog::onTxtInLineEditChanged);

    qCInfo(logGUI) << "Завершен метод createWindow()";
}

int FindDialog::exec()
{
    qCInfo(logGUI) << "Вызван метод exec()";

    txt_for_search_.clear();
    line_edit_->clear();
    ok_btn_->setEnabled(false);

    qCInfo(logGUI) << "Передаем работу в метод QDialog::exec()";
    return QDialog::exec();
}

QString FindDialog::getTxtForSearch() const
{
    qCInfo(logGUI) << "Вызван и завершен метод getTxtForSearch()";
    return txt_for_search_;
}

void FindDialog::onCancelBtnClicked()
{
    qCInfo(logGUI) << "Вызван обработчик кнопки 'Отмена'";
    reject();
    qCInfo(logGUI) << "Завершен обработчик кнопки 'Отмена'";
}

void FindDialog::onOkBtnClicked()
{
    qCInfo(logGUI) << "Вызван обработчик кнопки 'Ок'";
    QString text_in_line_edit = line_edit_->text();

    if (text_in_line_edit.isEmpty()) {
        return;
    }

    txt_for_search_ = text_in_line_edit;
    accept();
    qCInfo(logGUI) << "Завершен обработчик кнопки 'Ок'";
}

void FindDialog::onTxtInLineEditChanged(const QString& text)
{
    qCInfo(logGUI) << "Вызван обработчик события изменения текста в поле ввода";
    ok_btn_->setEnabled(!text.isEmpty());
    qCDebug(logGUI) << "Кнопка ок переключена в состояние: " + (ok_btn_->isEnabled() ? QString("Enabled") : QString("Disabled"));
    qCInfo(logGUI) << "Завершен обработчик события изменения текста в поле ввода";
}
