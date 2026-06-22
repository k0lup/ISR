#include "diiviewer.h"
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <QSignalBlocker>

static int col(ColumnViewTableName name) {
    return static_cast<int>(name);
}

static int col(ChapterType name) {
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
    table_wgt_->setSelectionMode(QAbstractItemView::NoSelection);
    table_wgt_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QObject::connect(table_wgt_, &QTableWidget::cellClicked, this, &DiiViewer::onCellClicked);
}

void DiiViewer::clearAllRows() {
    table_wgt_->setRowCount(0);
    commands_.clear();
}

void DiiViewer::setSection(const Section &section) {
    commands_.clear();

    int chapter_index = col(section.active_chapter_type);
    const Chapter& chapter = section.dii_file.chapters.at(chapter_index);

    int last_row = table_wgt_->rowCount();

    for (int command_index = 0; command_index < chapter.commands.count(); ++command_index) {
        CommandInfo command_info;
        const QList<CommandLine> lines = chapter.commands.at(command_index).command_lines;
        int index_first_row = last_row;
        command_info.first_row = index_first_row;
        for (int line_index = 0; line_index < lines.count(); ++line_index) {
            table_wgt_->insertRow(last_row);

            table_wgt_->setItem(last_row, col(ColumnViewTableName::TYPE), new QTableWidgetItem(QString(lines[line_index].type)));
            table_wgt_->setItem(last_row, col(ColumnViewTableName::COMMAND), new QTableWidgetItem(QString(lines[line_index].command)));
            table_wgt_->setItem(last_row, col(ColumnViewTableName::OPERATION), new QTableWidgetItem(QString(lines[line_index].operation)));

            last_row += 1;
        }

        table_wgt_->setItem(index_first_row, col(ColumnViewTableName::NUMBER), new QTableWidgetItem(QString::number(chapter.commands[command_index].number)));
        command_info.last_row = last_row - 1;
        commands_.append(command_info);
    }

    /*const int new_row_index = table_wgt_->rowCount();
    table_wgt_->insertRow(new_row_index);

    table_wgt_->setItem(new_row_index, col(ColumnViewTableName::NUMBER), new QTableWidgetItem(QString(command.number)));

    for (int i = 0; i < command.command_lines.count(); ++i) {
        int cur_row_index = new_row_index + i;
        table_wgt_->setItem(cur_row_index, col(ColumnViewTableName::TYPE), new QTableWidgetItem(command.command_lines[i].type));
        table_wgt_->setItem(cur_row_index, col(ColumnViewTableName::COMMAND), new QTableWidgetItem(command.command_lines[i].command));
    }*/
}

void DiiViewer::onCellClicked(int row, int) {
    for (CommandInfo& cmd : commands_) {
        if (cmd.is_active) {
            if (row >= cmd.first_row && row <= cmd.last_row) {
                return;
            }

            for (int r = cmd.first_row; r <= cmd.last_row; ++r) {
                clearRowColor(r);
            }
            cmd.is_active = false;
        }
    }

    for (CommandInfo& cmd : commands_) {
        if (row >= cmd.first_row  && row <= cmd.last_row) {
            for (int r = cmd.first_row; r <= cmd.last_row; ++r) {
                paintRow(r, Qt::yellow, Qt::black);
            }
            cmd.is_active = true;
            break;
        }
    }
}

void DiiViewer::paintRow(int row_index, const QColor& color_row, const QColor& color_text) {
    for (int col = 0; col < table_wgt_->columnCount(); ++col) {
        QTableWidgetItem *item = table_wgt_->item(row_index, col);

        if (!item) {
            item = new QTableWidgetItem();
            table_wgt_->setItem(row_index, col, item);
        }

        item->setBackground(color_row);
        item->setForeground(color_text);
    }
}

void DiiViewer::clearRowColor(int row_index) {
    for (int col = 0; col < table_wgt_->columnCount(); ++col) {
        if (auto* item = table_wgt_->item(row_index, col)) {
            item->setBackground(QBrush());
            item->setForeground(QBrush());
        }
    }
}
