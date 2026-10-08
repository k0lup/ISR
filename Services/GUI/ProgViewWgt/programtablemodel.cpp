#include "programtablemodel.h"
#include <QBrush>
#include <QColor>


ProgramTableModel::ProgramTableModel(QObject* parent) : QAbstractTableModel(parent)
{

}

int ProgramTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return rows_.size();
}

int ProgramTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return Column::CountColumn;
}

QVariant ProgramTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const ProgramRow& row = rows_.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case Column::NumberColumn       : return row.number;
        case Column::TypeColumn         : return row.type;
        case Column::CommandColumn      : return row.command;
        case Column::OperationColumn    : return row.operation;
        }
        return {};
    }

    if (role == Qt::BackgroundRole) {
        if (row.directive_index == active_directive_) {
            return QBrush(QColor(170, 230, 170));
        }

        if (completed_directives_.contains(row.directive_index)) {
            return QBrush(QColor(190, 220, 245));
        }
        if (row.directive_index == runned_directive_) {
            return QBrush(QColor(0, 0, 128));
        }
        if (row.directive_index == ProgramRow::NoDirectiveIndex) {
            return QBrush(QColor(75, 75, 80));
        }
        if (failed_directives_.contains(row.directive_index)) {
            return QBrush(Qt::red);
        }

        return QBrush(QColor(255, 255, 255));
    }

    if (role == Qt::ForegroundRole) {
        if (row.directive_index == runned_directive_) {
            return QBrush(QColor(255, 255, 0));
        }
        if (failed_directives_.contains(row.directive_index)) {
            return QBrush(Qt::white);
        }

        return QBrush(Qt::black);
    }

    if (role == Qt::TextAlignmentRole) {
        if (index.column() == NumberColumn || index.column() == TypeColumn) {
            return static_cast<int>(Qt::AlignTop | Qt::AlignHCenter);
        }

        return static_cast<int>(Qt::AlignTop | Qt::AlignLeft);
    }

    return {};
}

QVariant ProgramTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
        return {};
    }

    switch (section) {
    case Column::NumberColumn       : return QStringLiteral("№");
    case Column::TypeColumn         : return QStringLiteral("Т");
    case Column::CommandColumn      : return QStringLiteral("Команды, доклады, пояснения");
    case Column::OperationColumn    : return QStringLiteral("Операции по управлению и контролю");
    }
    return {};
}

Qt::ItemFlags ProgramTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void ProgramTableModel::setRows(const QVector<ProgramRow>& rows)
{
    beginResetModel();

    rows_ = std::move(rows);
    completed_directives_.clear();
    failed_directives_.clear();
    active_directive_ = ProgramRow::NoDirectiveIndex;
    runned_directive_ = ProgramRow::NoDirectiveIndex;

    endResetModel();
}

void ProgramTableModel::clear()
{
    beginResetModel();

    rows_.clear();
    active_directive_ = ProgramRow::NoDirectiveIndex;
    completed_directives_.clear();

    endResetModel();
}

int ProgramTableModel::directiveIndex(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return ProgramRow::NoDirectiveIndex;
    }

    if (rows_.at(index.row()).isEmpty()) {
        return ProgramRow::NoDirectiveIndex;
    }

    return rows_.at(index.row()).directive_index;
}

void ProgramTableModel::setActiveDirective(int directive_index)
{
    const int old = active_directive_;

    if (old == directive_index) {
        return;
    }

    resetStatusDirective(directive_index);

    active_directive_ = directive_index;

    updateDirective(old);
    updateDirective(directive_index);
}

void ProgramTableModel::setCompletedDirective(int directive_index)
{
    if (completed_directives_.contains(directive_index)) {
        return;
    }

    resetStatusDirective(directive_index);

    completed_directives_.insert(directive_index);

    updateDirective(directive_index);
}

void ProgramTableModel::setRunnedDirective(int directive_index)
{
    if (runned_directive_ == directive_index) {
        return;
    }

    resetStatusDirective(directive_index);

    runned_directive_ = directive_index;
    const int old_active_directive =  active_directive_;
    active_directive_ = -1;

    updateDirective(old_active_directive);
    updateDirective(runned_directive_);
}

void ProgramTableModel::setFailedDirective(int directive_index)
{
    if (failed_directives_.contains(directive_index)) {
        return;
    }

    resetStatusDirective(directive_index);

    failed_directives_.insert(directive_index);

    updateDirective(directive_index);
}

void ProgramTableModel::updateDirective(int directive_index)
{
    if (directive_index < 0) {
        return;
    }

    int first = -1;
    int last = -1;

    for (int i = 0; i < rows_.size(); ++i) {
        if (rows_[i].directive_index != directive_index) {
            continue;
        }

        if (first == -1) {
            first = i;
        }

        last = i;
    }

    if (first == -1) {
        return;
    }

    emit dataChanged(index(first, 0), index(last, Column::CountColumn - 1), {Qt::BackgroundColorRole});
}

void ProgramTableModel::resetStatusDirective(int directive_index)
{
    if (active_directive_ == directive_index) {
        active_directive_ = ProgramRow::NoDirectiveIndex;
    }

    if (runned_directive_ == directive_index) {
        runned_directive_ = ProgramRow::NoDirectiveIndex;
    }

    if (completed_directives_.contains(directive_index)) {
        completed_directives_.remove(directive_index);
    }

    if (failed_directives_.contains(directive_index)) {
        failed_directives_.remove(directive_index);
    }
}
