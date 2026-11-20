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


    current_program_ = exec_snapshot;
    /*if (current_program_.active_variant[current_program_.active_chpater] != -1) {
        setVariantSelection();
    }*/
    repaint();

    if (current_program_.auto_blocks[current_program_.active_chpater].start_index != -1) {
        qCInfo(logGUI) << "Отправляем сигнал autoBlockSwitched(true)";
        emit autoBlockSwitched(true);
    } else {
        qCInfo(logGUI) << "Отправляем сигнал autoBlockSwitched(false)";
        emit autoBlockSwitched(false);
    }

    qCInfo(logGUI) << "Завершен метод setSection()";
}

void ProgramViewWidget::onVarSelected(int var_id)
{
    DirectiveStruct cur_directive = current_program_.section.get(current_program_.active_chpater).directives[current_program_.active_dir_index[current_program_.active_chpater]];
    if (cur_directive.type != DirectType::VARIANT) {
        QString chapter_name;
        switch (current_program_.active_chpater) {
        case ChapterType::STATE : chapter_name = "ШТАТ"; break;
        case ChapterType::NOT_STATE : chapter_name = "НШС"; break;
        case ChapterType::PRI : chapter_name = "ПРИ"; break;
        }

        QString error_message = QString("Выбран вариант, хотя текущая активная директива - %1 (секция '%2' раздел '%3'")
                .arg(current_program_.active_dir_index[current_program_.active_chpater] + 1)
                .arg(current_program_.section_name)
                .arg(chapter_name);

        qCCritical(logGUI) << error_message;
        qCInfo(logGUI) << "Отправляем сигнал failed()";
        emit failed(error_message);
        return;
    }

    if (!cur_directive.sub_directives.contains(var_id) && var_id != -1) {
        QString chapter_name;
        switch (current_program_.active_chpater) {
        case ChapterType::STATE : chapter_name = "ШТАТ"; break;
        case ChapterType::NOT_STATE : chapter_name = "НШС"; break;
        case ChapterType::PRI : chapter_name = "ПРИ"; break;
        }

        QStringList variants;
        variants.reserve(cur_directive.sub_directives.size());
        for (const auto& key : cur_directive.sub_directives.keys()) {
            variants.append("'" + QString::number(key) + "'");
        }

        QString error_message = QString("Выбранного варианта (%4) не существует - директива №%1 (секция '%2' раздел '%3')\nДоступные варианты: %5")
                .arg(current_program_.active_dir_index[current_program_.active_chpater] + 1)
                .arg(current_program_.section_name)
                .arg(chapter_name)
                .arg(var_id)
                .arg(variants.join(" "));

        qCCritical(logGUI) << error_message;
        qCInfo(logGUI) << "Отправляем сигнал failed()";
        emit failed(error_message);
        return;
    }

    current_program_.active_variant[current_program_.active_chpater] = var_id;
    qCInfo(logGUI) << "Отправляем сигнал varSelected()";
    emit varSelected(current_program_, RequestIdGenerator::NO_REQUEST_ID);
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

void ProgramViewWidget::onRunOperationRequest()
{
    qCInfo(logGUI) << "Вызван метод onRunOperationRequest()";

    int current_select_directive_ = model_->getCurrentSelectDirectiveIndex();
    if (current_select_directive_ == ProgramTableModel::ProgramRow::NoDirectiveIndex) {
        qCInfo(logGUI) << "Нет выделенной директивы, поэтому запустим активную";
        current_select_directive_ = model_->getCurrentAcitveDirectiveIndex();
    }

    current_program_.active_dir_index[current_program_.active_chpater] = current_select_directive_;
    qCInfo(logGUI) << QString("Отправляем сигнал на запуск директивы №%1").arg(current_select_directive_ + 1);
    emit directiveDoubleClicked(current_program_, RequestIdGenerator::NO_REQUEST_ID);


    qCInfo(logGUI) << "Завершен метод onRunOperationRequest()";
}

void ProgramViewWidget::setVariantSelection()
{
    qCInfo(logGUI) << "Вызван метод setVariantSelection()";

    int start_select_index{-1};
    int end_select_index{-1};
    const int cur_dir_index = current_program_.active_dir_index[current_program_.active_chpater];
    const DirectiveStruct& cur_dir_struct = current_program_.section.get(current_program_.active_chpater).directives[cur_dir_index];
    const int var_id = current_program_.active_variant[current_program_.active_chpater];
    if (!cur_dir_struct.sub_directives.contains(var_id)) {
        QString error_message = "Запрошенного варианта не существует в текущей активной директиве!";
        qCCritical(logGUI) << error_message;

        qCInfo(logGUI) << "Отправляем сигнал failed()";
        emit failed(error_message);
        return;
    }

    bool convert_status1{false}, convert_status2{false};
    start_select_index = cur_dir_struct.sub_directives[var_id].lines.first().num_col.toInt(&convert_status1);
    end_select_index = cur_dir_struct.sub_directives[var_id].lines.last().num_col.toInt(&convert_status2);
    if (!convert_status1 || !convert_status2) {
        QString error_message = "Не удалось получить номер строки из структуры директивы!";
        qCCritical(logGUI) << error_message;

        qCInfo(logGUI) << "Отправляем сигнал failed()";
        emit failed(error_message);
        return;
    }

    qDebug() << "start: " << start_select_index;
    qDebug() << "end: " << end_select_index;

    model_->setRunnedDirective(start_select_index, end_select_index);
    qCInfo(logGUI) << "Завершен метод setVariantSelection()";
}
