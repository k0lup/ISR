#ifndef PROGRAMTABLEMODEL_H
#define PROGRAMTABLEMODEL_H
#include <QAbstractTableModel>
#include <QSet>
#include <QVector>
#include "Services/DirectiveExecutor/executiontypes.h"
#include "Services/Programs/programtypes.h"

class ProgramTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum class Direction {
        Up,
        Down
    };
    enum Column {
        AutoModeColumn = 0,
        NumberColumn,
        TypeColumn,
        CommandColumn,
        OperationColumn,

        CountColumn
    };

    struct ProgramRow
    {
        int directive_index = -1;

        bool auto_mode;
        QString number;
        QString type;
        QString command;
        QString operation;

        const static int NoDirectiveIndex = -1;

        bool isEmpty() const {
            return
                    number.isEmpty() &&
                    type.isEmpty() &&
                    command.isEmpty() &&
                    operation.isEmpty();
        }

        QString toString() const {
            return QString("|%1|%2|%3|%4|%5|").arg(auto_mode ? "A" : "").arg(number).arg(type).arg(command).arg(operation);
        }
    };

    /*struct Program {
        QVector<ProgramRow> program_rows;

        SectionStruct section_struct;
        ChapterType chapter;
        int exec_frame_id = -1;
    };*/

public:
    explicit ProgramTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void setRows(const QVector<ProgramRow>& rows);
    void clear();

    void setActiveDirective(int directive_index);
    void setSelectedDirective(Direction direction);
    void setSelectedDirective(int index);
    void setRunnedDirective(int directive_index);
    void setRunnedDirective(int start_row_index, int end_row_index);
    void setCompletedDirective(int directive_index);
    void setFailedDirective(int directive_index);

    int directiveIndex(const QModelIndex& index) const;

    int getCurrentSelectDirectiveIndex() const;
    int getCurrentAcitveDirectiveIndex() const;

private:
    void updateDirective(int directive_index);
    void resetStatusDirective(int directive_index);

    int getLastDirectiveIndex() const;
private:
    /*struct ProgInfo {
        SectionStruct section_struct;
        ChapterType chapter;
        int exec_frame_id = -1;

        void clear() {
            section_struct.clear();
            chapter = ChapterType::STATE;
            exec_frame_id = -1;
        }
    };*/

    struct RunnedDirective {
        int start_row{ProgramRow::NoDirectiveIndex};
        int end_row{ProgramRow::NoDirectiveIndex};
        int index_directive{ProgramRow::NoDirectiveIndex};

        void clear() {
            start_row = ProgramRow::NoDirectiveIndex;
            end_row = ProgramRow::NoDirectiveIndex;
            index_directive = ProgramRow::NoDirectiveIndex;
        }

        bool isEmpty() const {
            return start_row == ProgramRow::NoDirectiveIndex;
        }

        bool contains(const int index) const {
            return start_row <= index && index <= end_row;
        }
    };

private:
    QVector<ProgramRow> rows_;
    //ProgInfo prog_info_;

    int selected_directive_ = -1;
    int active_directive_ = -1;
    RunnedDirective runned_directive_;
    QSet<int> completed_directives_;
    QSet<int> failed_directives_;
};

#endif // PROGRAMTABLEMODEL_H
