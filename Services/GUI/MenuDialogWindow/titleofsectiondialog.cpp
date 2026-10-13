#include "titleofsectiondialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include <Logger/logger.h>
#include "logging_categories.h"

TitleOfSectionDialog::TitleOfSectionDialog(QWidget* parent) : QDialog(parent)
{
    qCInfo(logGUI) << "Вызван конструктор TitleOfSectionDialog";

    setWindowTitle("Заголовок раздела");
    createWindow();
    resize(600, 900);

    qCInfo(logGUI) << "Завершен конструктор TitleOfSectionDialog";
}

void TitleOfSectionDialog::createWindow()
{
    qCInfo(logGUI) << "Вызван метод createWindow()";

    QLabel* title_of_section_label_      = new QLabel("Заголок раздела:", this);
    QLabel* name_of_prot_label_          = new QLabel("Имя протокола:", this);
    QLabel* traf_label_                  = new QLabel(".", this);
    QLabel* rep_label_                   = new QLabel("РЭП:", this);
    QLabel* operator_pu_ais_label_       = new QLabel("Оператор ПУ АИС:", this);
    QLabel* operator_ais_label_          = new QLabel("Оператор АИС:", this);
    QLabel* operator_tkck_ss_bvs_label_  = new QLabel("Оператор ТКЦК СС БВС:", this);
    QLabel* operator_ntk_si_label_       = new QLabel("Оператор НТК СИ:", this);
    QLabel* operator_bs_label_           = new QLabel("Оператор БС:", this);

    title_of_section_line_edit_         = new QLineEdit(this);
    name_of_prot_line_edit_             = new QLineEdit(this);
    traf_line_edit_                     = new QLineEdit(this);
    rep_line_edit_                      = new QLineEdit(this);
    operator_pu_ais_line_edit_          = new QLineEdit(this);
    operator_ais_line_edit_             = new QLineEdit(this);
    operator_tkck_ss_bvs_line_edit_     = new QLineEdit(this);
    operator_ntk_si_line_edit_          = new QLineEdit(this);
    operator_bs_line_edit_              = new QLineEdit(this);

    ok_btn_     = new QPushButton("Ок", this);
    cancel_btn  = new QPushButton("Отмена", this);

    traf_line_edit_->setFixedWidth(150);

    QGridLayout *grid_layout = new QGridLayout();
    grid_layout->setColumnStretch(0, 0);
    grid_layout->setColumnStretch(1, 1);
    grid_layout->setHorizontalSpacing(20);
    grid_layout->setVerticalSpacing(12);

    grid_layout->addWidget(title_of_section_label_, 0, 0);
    grid_layout->addWidget(title_of_section_line_edit_, 0, 1);

    QHBoxLayout* prot_name_layout = new QHBoxLayout();
    prot_name_layout->setContentsMargins(0, 0, 0, 0);
    prot_name_layout->setSpacing(8);
    prot_name_layout->addWidget(name_of_prot_line_edit_);
    prot_name_layout->addWidget(traf_label_);
    prot_name_layout->addWidget(traf_line_edit_);

    grid_layout->addWidget(name_of_prot_label_, 1, 0);
    grid_layout->addLayout(prot_name_layout, 1, 1);

    grid_layout->addWidget(rep_label_, 2, 0);
    grid_layout->addWidget(rep_line_edit_, 2, 1);

    grid_layout->addWidget(operator_pu_ais_label_, 3, 0);
    grid_layout->addWidget(operator_pu_ais_line_edit_, 3, 1);

    grid_layout->addWidget(operator_ais_label_, 4, 0);
    grid_layout->addWidget(operator_ais_line_edit_, 4, 1);

    grid_layout->addWidget(operator_tkck_ss_bvs_label_, 5, 0);
    grid_layout->addWidget(operator_tkck_ss_bvs_line_edit_, 5, 1);

    grid_layout->addWidget(operator_ntk_si_label_, 6, 0);
    grid_layout->addWidget(operator_ntk_si_line_edit_, 6, 1);

    grid_layout->addWidget(operator_bs_label_, 7, 0);
    grid_layout->addWidget(operator_bs_line_edit_, 7, 1);

    QHBoxLayout* btn_layout = new QHBoxLayout();
    btn_layout->addStretch();
    btn_layout->addWidget(cancel_btn);
    btn_layout->addWidget(ok_btn_);

    QVBoxLayout* wgt_layout = new QVBoxLayout();
    wgt_layout->addLayout(grid_layout);
    wgt_layout->addLayout(btn_layout);

    setLayout(wgt_layout);

    QObject::connect(cancel_btn, &QPushButton::clicked, this, &TitleOfSectionDialog::onCancelBtnClicked);
    QObject::connect(ok_btn_, &QPushButton::clicked, this, &TitleOfSectionDialog::onOkBtnClicked);

    qCInfo(logGUI) << "Завершен метод createWindow()";
}

int TitleOfSectionDialog::exec(const QString& title_section, const QString& traf)
{
    qCInfo(logGUI) << "Вызван метод createWindow()";

    description_.clear();
    clearAllLineEdits();

    title_of_section_line_edit_->setText(title_section);
    name_of_prot_line_edit_->setText(title_section);
    traf_line_edit_->setText(traf);

    qCInfo(logGUI) << "Передаем работу в метод QDialog::exec()";
    return QDialog::exec();
}

TitleOfSectionDescription TitleOfSectionDialog::getDescription() const
{
    qCInfo(logGUI) << "Вызван и завершен метод getDescription()";

    return description_;
}

void TitleOfSectionDialog::clearAllLineEdits()
{
    qCInfo(logGUI) << "Вызван метод createWindow()";

    title_of_section_line_edit_->clear();
    name_of_prot_line_edit_->clear();
    traf_line_edit_->clear();
    rep_line_edit_->clear();
    operator_pu_ais_line_edit_->clear();
    operator_ais_line_edit_->clear();
    operator_tkck_ss_bvs_line_edit_->clear();
    operator_ntk_si_line_edit_->clear();
    operator_bs_line_edit_->clear();

    qCInfo(logGUI) << "Завершен метод createWindow()";
}

void TitleOfSectionDialog::onCancelBtnClicked()
{
    qCInfo(logGUI) << "Вызван обработчик кнопки 'Отмена'";

    reject();

    qCInfo(logGUI) << "Завершен обработчик кнопки 'Отмена'";
}

void TitleOfSectionDialog::onOkBtnClicked()
{
    qCInfo(logGUI) << "Вызван обработчик кнопки 'Ок'";

    description_.title_of_section = title_of_section_line_edit_->text();
    description_.name_of_prot = name_of_prot_line_edit_->text();
    description_.traf = traf_line_edit_->text();
    description_.rep = rep_line_edit_->text();
    description_.operator_pu_ais = operator_pu_ais_line_edit_->text();
    description_.operator_ais = operator_ais_line_edit_->text();
    description_.operator_tkck_ss_bvs = operator_tkck_ss_bvs_line_edit_->text();
    description_.operator_ntk_si = operator_ntk_si_line_edit_->text();
    description_.operator_bs = operator_bs_line_edit_->text();

    accept();

    qCInfo(logGUI) << "Завершен обработчик кнопки 'Ок'";
}

