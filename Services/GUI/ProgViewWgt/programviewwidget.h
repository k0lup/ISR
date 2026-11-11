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
    void autoModeSet(ExecuteMode mode, RequestId request_id);
    void autoBlockUpdate(const ExecutionSnapshot& snapshot, RequestId request_id);

    void autoBlockSwitched(bool contains);                                                      //Если есть блок авто в активном разделе - true. Нет - false;
    void failed(const QString message);
public slots:
    void onRunOperationRequest();
    void onStartOrEndAutoBlock();
    void onRemoveAutoMark();
    void onRunAutoBlock();
protected:
    void keyPressEvent(QKeyEvent *event) override;
private slots:
    void onDoubleClicked(const QModelIndex& index);
private:
    QTableView* table_ = nullptr;
    ProgramTableModel* model_ = nullptr;

    ExecutionSnapshot current_program_;
};

#endif // PROGRAMVIEWWIDGET_H
