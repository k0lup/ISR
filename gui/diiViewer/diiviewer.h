#ifndef DIIVIEWER_H
#define DIIVIEWER_H

#include <QWidget>
#include <QTableWidget>
#include "setReader/diireader.h"

enum class ColumnViewTableName {
    EMPTY = 0,
    NUMBER = 1,
    TYPE = 2,
    COMMAND = 3,
    OPERATION = 4
};

class DiiViewer : public QWidget
{
    Q_OBJECT
public:
    explicit DiiViewer(QWidget *parent = nullptr);
signals:
public slots:
    void clearAllRows();
    void addNewCommand(const Command& command);
private:
    QTableWidget *table_wgt_ = nullptr;
};

#endif // DIIVIEWER_H
