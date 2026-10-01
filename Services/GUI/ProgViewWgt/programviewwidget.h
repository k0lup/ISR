#ifndef PROGRAMVIEWWIDGET_H
#define PROGRAMVIEWWIDGET_H

#include <QWidget>
#include "Services/DirectiveExecutor/executiontypes.h"

class QTableView;
class ProgramTableModel;

class ProgramViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProgramViewWidget(QWidget *parent = nullptr);

    void setSection(const ExecutionSnapshot& exec_snapshot);

    void clear();
    void setActiveDirective(int directive_index);
    void setCompletedDirective(int directive_index);
signals:
    void directiveDoubleClicked(int directive_index);
private slots:
    void onDoubleClicked(const QModelIndex& index);

private:
    QTableView* table_ = nullptr;
    ProgramTableModel* model_ = nullptr;

};

#endif // PROGRAMVIEWWIDGET_H
