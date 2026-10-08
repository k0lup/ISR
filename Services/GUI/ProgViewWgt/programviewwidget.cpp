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
    qCInfo(logGUI) << "Вызван конструктор ProgramViewWidget";
    table_ = new QTableView(this);
    model_ = new ProgramTableModel(this);

    table_->setModel(model_);

    table_->setSelectionBehavior(QAbstractItemView::SelectRows);

    table_->setSelectionMode(QAbstractItemView::SingleSelection);

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
    qCInfo(logGUI) << "Вызван метод setSection()";
    const QList<DirectiveStruct>& directives = exec_snapshot.section.get(exec_snapshot.active_chpater).directives;
    QVector<ProgramTableModel::ProgramRow> table_rows;


    for (int current_directive_index = 0; current_directive_index < directives.count(); ++current_directive_index) {
        const QList<DirectiveLine>& lines = directives[current_directive_index].lines;
        QVector<ProgramTableModel::ProgramRow> current_directive_rows;
        for (int current_line_index = 0; current_line_index < lines.count(); ++current_line_index) {
            const DirectiveLine& line = lines[current_line_index];
            ProgramTableModel::ProgramRow current_row;
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

    repaint();

    current_program_ = exec_snapshot;

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
