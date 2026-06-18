#include "diiviewer.h"
#include <QHBoxLayout>
#include <QTableWidgetItem>

static int col(ColumnViewTableName name) {
    return static_cast<int>(name);
}

DiiViewer::DiiViewer(QWidget *parent)
    : QWidget{parent}
{
    table_wgt_ = new QTableWidget(this);
    QHBoxLayout *h_box_l = new QHBoxLayout();
    h_box_l->addWidget(table_wgt_);
    this->setLayout(h_box_l);

    table_wgt_->setColumnCount(5);
    QStringList headers;
    headers << "" << "№" << "Т" << "Команды, доклады, пояснения" << "операции по управлению и контролю";

    table_wgt_->setHorizontalHeaderLabels(headers);
}

void DiiViewer::clearAllRows() {
    table_wgt_->clear();
}

void DiiViewer::addNewCommand(const Command &command) {
    const int new_row_index = table_wgt_->rowCount();
    table_wgt_->insertRow(new_row_index);

    table_wgt_->setItem(new_row_index, col(ColumnViewTableName::NUMBER), new QTableWidgetItem(QString(command.number)));

    for (int i = 0; i < command.command_lines.count(); ++i) {
        int cur_row_index = new_row_index + i;
        table_wgt_->setItem(cur_row_index, col(ColumnViewTableName::TYPE), new QTableWidgetItem(command.command_lines[i].type));
        table_wgt_->setItem(cur_row_index, col(ColumnViewTableName::COMMAND), new QTableWidgetItem(command.command_lines[i].command));
    }
}
