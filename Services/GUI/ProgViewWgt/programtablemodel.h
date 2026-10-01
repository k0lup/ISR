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
    enum Column {
        NumberColumn = 0,
        TypeColumn,
        CommandColumn,
        OperationColumn,

        CountColumn
    };

    struct ProgramRow
    {
        int directive_index = -1;

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
    void setRunnedDirective(int directive_index);
    void setCompletedDirective(int directive_index);
    void setFailedDirective(int directive_index);

    int directiveIndex(const QModelIndex& index) const;

private:
    void updateDirective(int directive_index);
    void resetStatusDirective(int directive_index);
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
private:
    QVector<ProgramRow> rows_;
    //ProgInfo prog_info_;

    int active_directive_;
    int runned_directive_ = -1;
    QSet<int> completed_directives_;
    QSet<int> failed_directives_;
};

#endif // PROGRAMTABLEMODEL_H
