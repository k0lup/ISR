#include "titlesectionwgt.h"
#include <QMessageBox>
#include "logger/logging_categories.h"
#include <QLoggingCategory>
#include <QFile>
#include <QDir>
#include <QApplication>
#include <QFileInfo>
#include <QTextStream>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

static const QString TRAF_FILE_NAME = "Stencil.dat";


TitleSectionWgt::TitleSectionWgt(QWidget* parent) :
    QDialog(parent)
{
    qCDebug(logCore) << "Инициализация окна пункта меню заголовок раздела";
    setWindowTitle("Заголовок раздела");
    setModal(true);

    QLabel* title_section_lbl = new QLabel("Заголовок раздела:", this);
    QLabel* name_prot_lbl = new QLabel("Имя протокола:", this);
    QLabel* rep_lbl = new QLabel("РЭП:", this);
    QLabel* operator_pu_ais_lbl = new QLabel("Оператор ПУ АИС:", this);
    QLabel* operator_ais_lbl = new QLabel("Оператор АИС:", this);
    QLabel* operator_tkck_ss_bvs_lbl = new QLabel("Оператор ТКЦК СС БВС:", this);
    QLabel* operator_ntk_si_lbl = new QLabel("Оператор НТК СИ:", this);
    QLabel* operator_bs_lbl = new QLabel("Оператор БС:", this);

    name_section_field_          = new QLineEdit(this);
    name_prot_field_             = new QLineEdit(this);
    traf_prot_field_             = new QLineEdit(this);
    rep_field_                   = new QLineEdit(this);
    operator_pu_ais_field_       = new QLineEdit(this);
    operator_ais_field_          = new QLineEdit(this);
    operator_tkck_ss_bvs_field_  = new QLineEdit(this);
    operator_ntk_si_field_       = new QLineEdit(this);
    operator_bs_field_           = new QLineEdit(this);

    QPushButton* cancel_btn = new QPushButton("Отмена", this);
    QPushButton* ok_btn = new QPushButton("Ок", this);

    QLabel* split_name_and_traf_prot_lbl = new QLabel(".", this);

    QVBoxLayout *lbl_layout = new QVBoxLayout;
    QVBoxLayout *field_layout = new QVBoxLayout;
    QHBoxLayout *name_prot_field_layout = new QHBoxLayout;
    QHBoxLayout *field_section_layout = new QHBoxLayout;
    QHBoxLayout *btn_layout = new QHBoxLayout;
    QVBoxLayout *layout = new QVBoxLayout;

    lbl_layout->addWidget(title_section_lbl);
    lbl_layout->addWidget(name_prot_lbl);
    lbl_layout->addWidget(rep_lbl);
    lbl_layout->addWidget(operator_pu_ais_lbl);
    lbl_layout->addWidget(operator_ais_lbl);
    lbl_layout->addWidget(operator_tkck_ss_bvs_lbl);
    lbl_layout->addWidget(operator_ntk_si_lbl);
    lbl_layout->addWidget(operator_bs_lbl);

    field_layout->addWidget(name_section_field_);
    name_prot_field_layout->addWidget(name_prot_field_);
    name_prot_field_layout->addWidget(split_name_and_traf_prot_lbl);
    name_prot_field_layout->addWidget(traf_prot_field_);
    field_layout->addLayout(name_prot_field_layout);
    field_layout->addWidget(rep_field_);
    field_layout->addWidget(operator_pu_ais_field_);
    field_layout->addWidget(operator_ais_field_);
    field_layout->addWidget(operator_tkck_ss_bvs_field_);
    field_layout->addWidget(operator_ntk_si_field_);
    field_layout->addWidget(operator_bs_field_);

    field_section_layout->addLayout(lbl_layout);
    field_section_layout->addLayout(field_layout);

    btn_layout->addStretch();
    btn_layout->addWidget(cancel_btn);
    btn_layout->addWidget(ok_btn);

    layout->addLayout(field_section_layout);
    layout->addLayout(btn_layout);

    setLayout(layout);


    QString application_dir_path = QCoreApplication::applicationDirPath();
    QString traf_file_path = QDir(application_dir_path).filePath(TRAF_FILE_NAME);

    QFile traf_file(traf_file_path);
    if (!traf_file.exists()) {
        QString message = QString("Не удалось найти файл с трафаретом по пути: '%1'").arg(traf_file_path);
        qCWarning(logCore) << message;
        QMessageBox::warning(nullptr, "Предупреждение", message);
    } else {
        if (!traf_file.open(QIODevice::Text | QIODevice::ReadOnly)) {
            QString message = QString("Не удалось открыть файл с трафаретом ('%1'): %2").arg(traf_file_path).arg(traf_file.errorString());
            qCWarning(logCore) << message;
            QMessageBox::warning(nullptr, "Предупреждение", message);
        }
        QTextStream in(&traf_file);
        //title_of_sections_.traf = in.readLine();
        QString traf = in.readLine();
        if (traf.isEmpty()) {
            QString message = QString("Трафарет в файле ('%1') пустой!").arg(traf_file_path);
            qCWarning(logCore) << message;
            QMessageBox::warning(nullptr, "Предупреждение", message);
        } else {
            qCInfo(logCore) << QString("Задан трафарет: %1").arg(traf);
            traf_prot_field_->setText(traf);
            traf_prot_field_->setReadOnly(true);
        }
    }

    //title_of_sections_.setSectionName(name_section);

    QObject::connect(name_section_field_, &QLineEdit::textChanged, this, &TitleSectionWgt::setSectionName);
    QObject::connect(name_prot_field_, &QLineEdit::textChanged, this, &TitleSectionWgt::setSectionName);

    QObject::connect(cancel_btn, &QPushButton::clicked, this, &QDialog::reject);
    QObject::connect(ok_btn, &QPushButton::clicked, this, &TitleSectionWgt::onOkBtnClicked);

    qCDebug(logCore) << "Завершена инициализация окна пункта меню заголовок раздела";
}

TitleSectionWgt::TitleSectionWgt(const QString& name_section, QWidget* parent) :
    TitleSectionWgt(parent)
{
    setSectionName(name_section);
}

void TitleSectionWgt::setSectionName(const QString &section_name) {
    bool has_changed{false};
    if (name_section_field_->text() != section_name) {
        has_changed = true;
        name_section_field_->setText(section_name);
    }

    if (name_prot_field_->text() != section_name) {
        has_changed = true;
        name_prot_field_->setText(section_name);
    }

    if (has_changed) {
        qCDebug(logCore) << QString("Задано/изменено имя раздела: %1").arg(section_name);
    }
}

TitleSection TitleSectionWgt::getTitleOfSection() const {
    TitleSection result{};
    result.name_section = name_section_field_->text();
    result.name_of_prot = name_prot_field_->text();
    result.traf = traf_prot_field_->text();
    result.operator_pu_ais = operator_pu_ais_field_->text();
    result.operator_ais = operator_ais_field_->text();
    result.operator_tkck_ss_bvs = operator_tkck_ss_bvs_field_->text();
    result.operator_ntk_si = operator_ntk_si_field_->text();
    result.operator_bs = operator_bs_field_->text();

    return result;
}

int TitleSectionWgt::exec(const QString& section_name) {
    rep_field_ = nullptr;
    rep_field_->setText("");

    qCDebug(logCore) << "Запрошен exec пункта меню 'Заголовок раздела'";
    clearWgt();
    name_section_field_->setText(section_name);
    name_prot_field_->setText(section_name);

    int result = QDialog::exec();
    qCDebug(logCore) << "Завершен exec пункта меню 'Заголовок раздела'";
    return result;
}

void TitleSectionWgt::clearWgt() {
    //name_section_field_->setText("");
    //name_prot_field_->setText("");
    //traf_prot_field_->setText("");
    rep_field_->setText("");
    operator_pu_ais_field_->setText("");
    operator_ais_field_->setText("");
    operator_tkck_ss_bvs_field_->setText("");
    operator_ntk_si_field_->setText("");
    operator_bs_field_->setText("");
}

void TitleSectionWgt::onOkBtnClicked() {
    TitleSection title_section = getTitleOfSection();
    qCInfo(logCore) << QString("Был введен следующий заголовок раздела:\n%1").arg(title_section.getString());
    accept();
}
