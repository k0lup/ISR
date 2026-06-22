#ifndef DIIVIEWER_H
#define DIIVIEWER_H

#include <QWidget>
#include <QTableWidget>
#include <QColor>
#include "setReader/sectionsloader.h"

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
    void setSection(const Section& section);
private slots:
    void onCellClicked(int row, int);
private:
    struct CommandInfo {
        int first_row;
        int last_row;

        bool is_active = false;
    };
private:
    QTableWidget *table_wgt_ = nullptr;
    QVector<CommandInfo> commands_;
private:
    void paintRow(int row_index, const QColor& color_row, const QColor& color_text);
    void clearRowColor(int row_index);
};

#endif // DIIVIEWER_H
