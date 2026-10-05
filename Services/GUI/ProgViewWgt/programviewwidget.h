#ifndef PROGRAMVIEWWIDGET_H
#define PROGRAMVIEWWIDGET_H

#include <QWidget>
#include "Services/DirectiveExecutor/executiontypes.h"
#include "Services/RequestIDGenerator/requestIDGenerator.h"

class QTableView;
class ProgramTableModel;

class ProgramViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProgramViewWidget(QWidget *parent = nullptr);

    void setSection(const ExecutionSnapshot& exec_snapshot);

    void clear();

    void onChapterChanged(const ChapterType& chapter);
signals:
    void directiveDoubleClicked(const ExecutionSnapshot& snapshot, RequestId request_id);
private slots:
    void onDoubleClicked(const QModelIndex& index);
private:
    QTableView* table_ = nullptr;
    ProgramTableModel* model_ = nullptr;

    ExecutionSnapshot current_program_;
};

#endif // PROGRAMVIEWWIDGET_H
