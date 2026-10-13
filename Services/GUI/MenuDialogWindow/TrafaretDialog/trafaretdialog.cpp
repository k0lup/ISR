#include "trafaretdialog.h"
#include "trafaretlineedit.h"
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

TrafaretDialog::TrafaretDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("Трафарет");
    resize(400, 300);
    createWindow();
}

void TrafaretDialog::createWindow()
{
    QLabel* traf_label = new QLabel("Трафарет:", this);
    traf_line_edit_ = new TrafaretLineEdit(this);

    ok_btn_ = new QPushButton("Ок", this);
    cancel_btn_ = new QPushButton("Отмена", this);

    QHBoxLayout *traf_line_layout = new QHBoxLayout();
    traf_line_layout->addWidget(traf_label);
    traf_line_layout->addWidget(traf_line_edit_);

    QHBoxLayout *btn_layout = new QHBoxLayout();
    btn_layout->addWidget(cancel_btn_);
    btn_layout->addWidget(ok_btn_);

    QVBoxLayout* wgt_layout = new QVBoxLayout();
    wgt_layout->addLayout(traf_line_layout);
    wgt_layout->addLayout(btn_layout);

    setLayout(wgt_layout);

    QObject::connect(ok_btn_, &QPushButton::clicked, this, &TrafaretDialog::onOkBtnClicked);
    QObject::connect(cancel_btn_, &QPushButton::clicked, this, &TrafaretDialog::onCancelBtnClicked);
}

QString TrafaretDialog::getTrafText() const
{
    return QString(traf_).replace(".", "");;
}

int TrafaretDialog::exec(const QString &traf)
{
    traf_ = traf;
    traf_line_edit_->setTrafaretTextText(traf);

    return QDialog::exec();
}

bool TrafaretDialog::isValid(QString text) const
{
    text.replace('.', "");
    QStringList valid_list = text.split("-");

    if (valid_list.count() != 2) {
        return false;
    }

    if (valid_list[0].isEmpty() || valid_list[1].isEmpty()) {
        return false;
    }

    return true;
}

void TrafaretDialog::onCancelBtnClicked()
{
    reject();
}

void TrafaretDialog::onOkBtnClicked()
{
    if (!isValid(traf_line_edit_->trafaretText())) {
        QMessageBox::warning(this, "Предупреждение", "Введенное значение некорректно. За информацией обращайтесь к эксплутационной документации");
        return;
    }

    traf_ = traf_line_edit_->trafaretText();
    accept();
}
