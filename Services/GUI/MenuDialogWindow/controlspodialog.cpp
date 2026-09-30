#include "controlspodialog.h"
#include <QCheckBox>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

ControlSpoDialog::ControlSpoDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("Контроль СПО");
    resize(400, 300);
    createWindow();
}

void ControlSpoDialog::createWindow()
{
    settings_check_box_ = new QCheckBox("Файл настройки раздела", this);
    struct_check_box_ = new QCheckBox("Файл структуры раздела", this);
    prog_check_box_ = new QCheckBox("Файлы программ раздела в ЦПКУ", this);

    settings_check_box_->setChecked(true);
    struct_check_box_->setChecked(true);
    prog_check_box_->setChecked(true);

    close_btn_ = new QPushButton("Закрыть", this);
    check_btn_ = new QPushButton("Проверить", this);

    QGroupBox* group_param_check = new QGroupBox("Проверять:", this);

    QVBoxLayout* check_layout = new QVBoxLayout();
    check_layout->addWidget(settings_check_box_);
    check_layout->addWidget(struct_check_box_);
    check_layout->addWidget(prog_check_box_);

    group_param_check->setLayout(check_layout);

    QHBoxLayout *btn_layout = new QHBoxLayout();
    btn_layout->addStretch();
    btn_layout->addWidget(close_btn_);
    btn_layout->addWidget(check_btn_);

    QVBoxLayout* widget_layout = new QVBoxLayout();
    widget_layout->addWidget(group_param_check);
    widget_layout->addLayout(btn_layout);
    setLayout(widget_layout);

    QObject::connect(close_btn_, &QPushButton::clicked, this, &ControlSpoDialog::onCloseBtnClicked);
    QObject::connect(check_btn_, &QPushButton::clicked, this, &ControlSpoDialog::onCheckBtnClicked);
    QObject::connect(settings_check_box_, &QCheckBox::toggled, this, &ControlSpoDialog::onCheckOptionsChanges);
    QObject::connect(struct_check_box_, &QCheckBox::toggled, this, &ControlSpoDialog::onCheckOptionsChanges);
    QObject::connect(prog_check_box_, &QCheckBox::toggled, this, &ControlSpoDialog::onCheckOptionsChanges);
}

void ControlSpoDialog::onCheckBtnClicked()
{
    check_params_.need_check_settins = settings_check_box_->isChecked();
    check_params_.need_check_struct = struct_check_box_->isChecked();
    check_params_.need_check_progs = prog_check_box_->isChecked();

    if (check_params_.need_check_settins) {
        emit requestSettingsFileCheck();
    } else {
        onSettingFileChecked(true);
    }
}

void ControlSpoDialog::onSettingFileChecked(const bool status, const QString& errors)
{
    if (!status) {
        QMessageBox::warning(this, "Ошибка", "Проверка файла настройки провалена:\n" + errors);
        return;
    }

    if (check_params_.need_check_struct) {
        emit requestStructFileCheck();
    } else {
        onStructFileChecked(true);
    }
}

void ControlSpoDialog::onStructFileChecked(const bool status, const QString& errors)
{
    if (!status) {
        QMessageBox::warning(this, "Ошибка", "Проверка файла структуры провалена:\n" + errors);
        return;
    }

    if (check_params_.need_check_progs) {
        emit requestProgFilesCheck();
    } else {
        onProgFilesChecked(true);
    }
}

void ControlSpoDialog::onProgFilesChecked(const bool status, const QString& errors)
{
    if (!status) {
        QMessageBox::warning(this, "Ошибка", "Проверка файлов программ провалена:\n" + errors);
        return;
    }

    QMessageBox::information(this, "Успех!", "Контроль СПО для раздела в соотетствии с заданными критерями прошел успешно");
}

void ControlSpoDialog::onCloseBtnClicked()
{
    reject();
}

void ControlSpoDialog::onCheckOptionsChanges()
{
    bool has_active_check_box =
            settings_check_box_->isChecked() ||
            struct_check_box_->isChecked() ||
            prog_check_box_->isChecked();

    if (!has_active_check_box) {
        check_btn_->setEnabled(false);
    } else {
        check_btn_->setEnabled(true);
    }
}
