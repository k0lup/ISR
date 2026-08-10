/*
 * Передаем структуру раздела в Executor, а он уже отправляет данные для отображения в GUI
 * Executor хранит что-то вроде стека вызовов, позволяя переходить между вызовами (сможем сделать НАЧРАЗД)
 * А Дальше нужна связь с ПРИС. Можно попробовать поставить с диска.
*/

#include "diiviewer.h"
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <QSignalBlocker>
#include <QVector>
#include <QMessageBox>
#include <QInputDialog>
#include <QLoggingCategory>

#include "Directives/commanddirective.h"
#include "Directives/directive.h"
#include "Directives/dokladdirective.h"
#include "Directives/insructionsdirective.h"
#include "Directives/mainoperdirective.h"
#include "Directives/startsectiondirective.h"
#include "Directives/variantdirective.h"

#include "logger/logging_categories.h"

static int col(ColumnViewTableName name) {
    return static_cast<int>(name);
}

static int col(ChapterType name) {
    return static_cast<int>(name);
}

DiiViewer::DiiViewer(QWidget *parent)
    : QWidget{parent}
{
    executor_ = new Executor(this);
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
    QObject::connect(this, &DiiViewer::failed, this, &DiiViewer::showErrorMessage);
    QObject::connect(this, &DiiViewer::startDirectiveRequested, executor_, &Executor::startFrom);
    QObject::connect(this, &DiiViewer::windowResponse, executor_, &Executor::windowResponse);
    QObject::connect(this, &DiiViewer::PRISResponse, executor_, &Executor::PRISResponse);
    //QObject::connect(this, &DiiViewer::startProgResponse, executor_, &Executor::StartProgResponse);
    QObject::connect(executor_, &Executor::requestSendDataToPris, this, &DiiViewer::onRequestSendDataToPRIS);
    QObject::connect(executor_, &Executor::requestShowWindow, this, &DiiViewer::onRequestShowWindow);
    QObject::connect(executor_, &Executor::requestStartProgram, this, &DiiViewer::onRequestStartProgram);
    QObject::connect(executor_, &Executor::directiveFinished, this, &DiiViewer::onDirFinished);
    QObject::connect(executor_, &Executor::curDirectiveChanged, this, &DiiViewer::onCurDirChanged);
    QObject::connect(executor_, &Executor::failed, this, &DiiViewer::failed);
}

void DiiViewer::clearAllRows() {
    table_wgt_->setRowCount(0);
    commands_.clear();
}

void DiiViewer::setSection(const Section &section) {
    clearAllRows();
    QVector<Direct*> vec_dirs;
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
}

void DiiViewer::showErrorMessage(const QString& error_message) {
    QMessageBox::critical(this, "Критическая ошибка", error_message);
}

void DiiViewer::onCurDirChanged(const int index) {
    if (index < 0 || index >= commands_.count()) {
        emit failed("НЕДОПУСТИМЫЙ ИНДЕКС ДИРЕКТИВЫ");
        return;
    }

    for (CommandInfo& cmd : commands_) {
        if (cmd.is_active) {
            for (int r = cmd.first_row; r <= cmd.last_row; ++r) {
                clearRowColor(r);
            }
            cmd.is_active = false;
        }
    }

    auto& cmd = commands_[index];
    for (int r = cmd.first_row; r <= cmd.last_row; ++r) {
        paintRow(r, Qt::yellow, Qt::black);
    }
    cmd.is_active = true;
}

void DiiViewer::onCellClicked(int row, int) {
    /*for (CommandInfo& cmd : commands_) {
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
    }*/

    for (int index = 0; index < commands_.count(); ++index) {
        const CommandInfo& cmd = commands_.at(index);
        if (row >= cmd.first_row && row <= cmd.last_row) {
            qCDebug(logCore) << QString("Отправили сигнал о выборе команды %1").arg(index);
            emit startDirectiveRequested(index, Executor::ExecutorMode::STEP);
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

void DiiViewer::onDirFinished(const Direct::ResultDirective& result, const int index) {
    qCDebug(logCore) << QString("Получили сигнал о завершении директивы %1").arg(index);
    if (index < 0 || index >= commands_.count()) {
        emit failed("НЕДОПУСТИМЫЙ ИНДЕКС ДИРЕКТИВЫ");
        return;
    }
    auto& cmd = commands_[index];
    cmd.is_active = false;
   if (result.type == Direct::RESULT_DIRECTIVE_TYPES::ERROR) {
       emit failed(result.message);
       for (int r = cmd.first_row; r <= cmd.last_row; ++r) {
           paintRow(r, Qt::red, Qt::white);
       }
   } else {
       if (!result.message.isEmpty()) {
           showInfoMessage(result.message);
       }
       for (int r = cmd.first_row; r <= cmd.last_row; ++r) {
           clearRowColor(r);
       }
   }
}

void DiiViewer::onRequestSendDataToPRIS(const PRISMessage& message) {
    Q_UNUSED(message);
    PRISResultData result_data;
    result_data.status = true;
    emit PRISResponse(result_data);
}

void DiiViewer::onRequestStartProgram(const QString& params) {
    Q_UNUSED(params);
    emit startProgResponse(true);
}

void DiiViewer::onRequestShowWindow(const WidgetTypes& type, const WidgetInfo& info) {
    if (type == WidgetTypes::WARNING) {
        QString title = (info.title.isEmpty() ? "Предупреждение" : info.title);
        ShowWarningWgt(title, info.information.join("\n"));
    } else if (type == WidgetTypes::INFO) {
        showInfoWgt(info.title, info.information.join("\n"));
    } else if (type == WidgetTypes::VARIANT) {
        ShowVarWgt(info.title, info.information);
    } else {
        emit failed("ЗАПРОШЕНО НЕДОПУСТИМОЕ ОКНО!");
    }
}

void DiiViewer::showInfoWgt(const QString& title, const QString& data) {
    WindowResultData result_data;
    QMessageBox::StandardButton result = QMessageBox::question(
        this,
        title,
        data,
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel // кнопка по умолчанию
    );

    if (result == QMessageBox::Ok) {
        result_data.status = true;
    } else if (result == QMessageBox::Cancel) {
        result_data.status = false;
    }

    emit windowResponse(result_data);
}

void DiiViewer::ShowWarningWgt(const QString& title, const QString& data) {
    QMessageBox::warning(this, title, data);
}

void DiiViewer::ShowVarWgt(const QString& title, const QStringList& variants) {

    bool ok = false;

    QString selected = QInputDialog::getItem(
        this,
        title,
        "Выберите вариант:",
        variants,
        0,      // индекс выбранного по умолчанию элемента
        false,  // запрещаем пользователю вводить собственный текст
        &ok
    );

    WindowResultData result_data;
    result_data.status = ok;
    if (!ok) {
        emit windowResponse(result_data);
        return;
    }

    result_data.data = selected;
    emit windowResponse(result_data);
}

void DiiViewer::showInfoMessage(const QString& message) {
    QMessageBox::information(this, "Информация", message);
}
