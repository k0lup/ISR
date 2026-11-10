#include "programviewwidget.h"
#include <QHeaderView>
#include <QTableView>
#include <QHBoxLayout>
#include "programtablemodel.h"
#include <QList>

#include <Logger/logger.h>
#include "logging_categories.h"

ProgramViewWidget::ProgramViewWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("ProgramView");
    setFocusPolicy(Qt::StrongFocus);
    qCInfo(logGUI) << "Вызван конструктор ProgramViewWidget";
    table_ = new QTableView(this);
    model_ = new ProgramTableModel(this);

    table_->setModel(model_);

    //table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::NoSelection);
    table_->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::StrongFocus);

    //table_->setSelectionMode(QAbstractItemView::SingleSelection);

    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    table_->verticalHeader()->hide();

    table_->horizontalHeader()->setStretchLastSection(false);

    table_->horizontalHeader()->setSectionResizeMode(ProgramTableModel::NumberColumn, QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(ProgramTableModel::TypeColumn, QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(ProgramTableModel::CommandColumn, QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(ProgramTableModel::OperationColumn, QHeaderView::Stretch);

    QObject::connect(table_, &QTableView::doubleClicked, this, &ProgramViewWidget::onDoubleClicked);

    QHBoxLayout *hbox_layout = new QHBoxLayout();
    hbox_layout->setContentsMargins(0, 0, 0, 0);
    hbox_layout->addWidget(table_);

    setLayout(hbox_layout);

    qCInfo(logGUI) << "Завершен конструктор ProgramViewWidget";
}

void ProgramViewWidget::onDoubleClicked(const QModelIndex& index)
{
    qCInfo(logGUI) << "Вызван обработчик двойного нажатия на директиву";
    const int directive_index = model_->directiveIndex(index);

    if (directive_index < 0) {
        return;
    }

    current_program_.active_dir_index[current_program_.active_chpater] = directive_index;
    qCInfo(logGUI) << "Отправляем сигнал directiveDoubleClicked()";
    emit directiveDoubleClicked(current_program_, RequestIdGenerator::NO_REQUEST_ID);
    qCInfo(logGUI) << "Завершен обработчик двойного нажатия на директиву";
}

void ProgramViewWidget::setSection(const ExecutionSnapshot& exec_snapshot)
{
    int select_dir_index{-1};
    if (exec_snapshot.section_name == current_program_.section_name && exec_snapshot.active_chpater == current_program_.active_chpater) {
        if (exec_snapshot.active_dir_index[exec_snapshot.active_chpater] == current_program_.active_dir_index[current_program_.active_chpater]) {
            select_dir_index = model_->getCurrentSelectDirectiveIndex();
        }
    }
    qCInfo(logGUI) << "Вызван метод setSection()";
    const QList<DirectiveStruct>& directives = exec_snapshot.section.get(exec_snapshot.active_chpater).directives;
    QVector<ProgramTableModel::ProgramRow> table_rows;


    for (int current_directive_index = 0; current_directive_index < directives.count(); ++current_directive_index) {
        const QList<DirectiveLine>& lines = directives[current_directive_index].lines;
        QVector<ProgramTableModel::ProgramRow> current_directive_rows;
        bool is_auto_mode_directive {false};
        if (current_directive_index >= exec_snapshot.auto_blocks[exec_snapshot.active_chpater].start_index &&
                current_directive_index <= exec_snapshot.auto_blocks[exec_snapshot.active_chpater].end_index) {
            is_auto_mode_directive = true;
        }
        for (int current_line_index = 0; current_line_index < lines.count(); ++current_line_index) {
            const DirectiveLine& line = lines[current_line_index];
            ProgramTableModel::ProgramRow current_row;
            current_row.auto_mode = is_auto_mode_directive;
            current_row.number = line.num_col;
            current_row.type = line.type_col;
            current_row.command = line.command_col;
            current_row.operation = line.operation_col;
            current_row.directive_index = current_directive_index;
            current_directive_rows.append(current_row);
        }

        table_rows.append(current_directive_rows);
    }

    model_->setRows(table_rows);

    for (int current_directive_index = 0; current_directive_index < directives.count(); ++current_directive_index) {
        const DirectiveStruct& directive = directives[current_directive_index];
        switch (directive.status) {
        case DirectStatus::CURRENT : model_->setActiveDirective(current_directive_index); continue;
        case DirectStatus::NOT_LOAD :
        case DirectStatus::FAILED : model_->setFailedDirective(current_directive_index); continue;
        case DirectStatus::FINISHED : model_->setCompletedDirective(current_directive_index); continue;
        case DirectStatus::RUNNING : model_->setRunnedDirective(current_directive_index); continue;
        case DirectStatus::LOAD : continue;
        }
    }

    if (select_dir_index != -1) {
        model_->setSelectedDirective(select_dir_index);
    }

    repaint();

    current_program_ = exec_snapshot;

    if (current_program_.auto_blocks[current_program_.active_chpater].start_index != -1) {
        qCInfo(logGUI) << "Отправляем сигнал autoBlockSwitched(true)";
        emit autoBlockSwitched(true);
    } else {
        qCInfo(logGUI) << "Отправляем сигнал autoBlockSwitched(false)";
        emit autoBlockSwitched(false);
    }

    qCInfo(logGUI) << "Завершен метод setSection()";
}

void ProgramViewWidget::onChapterChanged(const ChapterType &chapter)
{
    qCInfo(logGUI) << "Вызван метод onChapterChanged()";

    current_program_.active_chpater = chapter;
    setSection(current_program_);

    qCInfo(logGUI) << "Завершен метод onChapterChanged()";
}

void ProgramViewWidget::clear()
{
    qCInfo(logGUI) << "Вызван метод clear()";
    model_->clear();
    qCInfo(logGUI) << "Завершен метод clear()";
}

void ProgramViewWidget::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "Key Press Event";
    switch (event->key()) {
    case Qt::Key_Up : model_->setSelectedDirective(ProgramTableModel::Direction::Up); qDebug() << "UP EVENT"; event->accept(); break;
    case Qt::Key_Down : model_->setSelectedDirective(ProgramTableModel::Direction::Down); qDebug() << "DOWN EVENT"; event->accept(); break;
    default: break;
    }

    QWidget::keyPressEvent(event);
}

void ProgramViewWidget::onStartOrEndAutoBlock()
{
    qCInfo(logGUI) << "Вызван метод onStartOrEndAutoBlock()";

    int current_select_dir_index = model_->getCurrentSelectDirectiveIndex();
    if (current_select_dir_index == -1) {
        current_select_dir_index = model_->getCurrentAcitveDirectiveIndex();
        model_->setSelectedDirective(current_select_dir_index);
    }

    if (current_program_.auto_blocks[current_program_.active_chpater].start_index == -1) {
        qDebug() << QString("Start Index = -1! Set(%1;%2)").arg(current_select_dir_index).arg(current_select_dir_index);
        current_program_.auto_blocks[current_program_.active_chpater].setIndexes(current_select_dir_index, current_select_dir_index);
    } else if (current_program_.auto_blocks[current_program_.active_chpater].end_index == current_program_.auto_blocks[current_program_.active_chpater].start_index) {
        int index = current_program_.auto_blocks[current_program_.active_chpater].start_index;
        current_program_.auto_blocks[current_program_.active_chpater].setIndexes(index, current_select_dir_index);
        qDebug() << QString("Start Index != -1 && End Index != -1! Set(%1;%2)").arg(index).arg(current_select_dir_index);
    } else {
        qDebug() << QString("Start Index != -1 && End Index != -1! Set(%1;%2)").arg(current_select_dir_index).arg(current_select_dir_index);
        current_program_.auto_blocks[current_program_.active_chpater].setIndexes(current_select_dir_index, current_select_dir_index);
    }

    qCInfo(logCore) << "Отправляем сигнал autoBlockUpdate";
    emit autoBlockUpdate(current_program_, RequestIdGenerator::NO_REQUEST_ID);

    qCInfo(logGUI) << "Завершен метод onStartOrEndAutoBlock()";
}


void ProgramViewWidget::onRemoveAutoMark()
{
    qCInfo(logGUI) << "Вызван метод onRemoveAutoMark()";

    current_program_.auto_blocks[current_program_.active_chpater].clear();
    qCInfo(logGUI) << "Отправляем сигнал autoBlockUpdate";
    emit autoBlockUpdate(current_program_, RequestIdGenerator::NO_REQUEST_ID);

    qCInfo(logGUI) << "Завершен метод onRemoveAutoMark()";
}

void ProgramViewWidget::onRunAutoBlock()
{
    qCInfo(logGUI) << "Вызван метод onStartAutoBlock()";
    if (current_program_.auto_blocks[current_program_.active_chpater].start_index == -1 || current_program_.auto_blocks[current_program_.active_chpater].end_index == -1) {
        QString message = "Невозможно запустить БЛОК Авто, так как он не определен!";
        qCCritical(logGUI) << message;
        qCInfo(logGUI) << "Отпарвляем сигнал failed";
        emit failed(message);
        return;
    }

    qCInfo(logGUI) << "Отправляем сигнал autoModeSet::AUTO";
    emit autoModeSet(ExecuteMode::AUTO, RequestIdGenerator::NO_REQUEST_ID);
    current_program_.active_dir_index[current_program_.active_chpater] = current_program_.auto_blocks[current_program_.active_chpater].start_index;
    qCInfo(logGUI) << QString("Отправляем сигнал directiveDoubleClicked(№%1)").arg(current_program_.active_dir_index[current_program_.active_chpater] + 1);
    emit directiveDoubleClicked(current_program_, RequestIdGenerator::NO_REQUEST_ID);
    qCInfo(logGUI) << "Завершен метод onStartAutoBlock()";
}
