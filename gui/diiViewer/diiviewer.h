#ifndef DIIVIEWER_H
#define DIIVIEWER_H

#include <QWidget>
#include <QTableWidget>
#include <QColor>
#include "setReader/sectionsloader.h"
#include "Directives/executor.h"

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
    void startDirectiveRequested(int index, Executor::ExecutorMode mode);
    void failed(const QString& message);

    void windowResponse(const WindowResultData& data);
    void PRISResponse(const PRISResultData& data);
    void startProgResponse(const bool status);
public slots:
    void clearAllRows();
    void setSection(const Section& section);


    void onCurDirChanged(const int index);
    void onDirFinished(const Direct::ResultDirective& result, const int index);
    void onRequestShowWindow(const WidgetTypes& type, const WidgetInfo& info);
    void onRequestSendDataToPRIS(const PRISMessage& message);
    void onRequestStartProgram(const START_SECTION_PARAMS& params);
private slots:
    void onCellClicked(int row, int);

    void showInfoWgt(const QString& title, const QString& data);
    void ShowWarningWgt(const QString& title, const QString& data);
    void ShowVarWgt(const QString& title, const QStringList& variants);

    void showErrorMessage(const QString& message);
    void showInfoMessage(const QString& message);
private:
    struct CommandInfo {
        int first_row;
        int last_row;

        bool is_active = false;
    };
private:
    QTableWidget *table_wgt_ = nullptr;
    QVector<CommandInfo> commands_;
    Executor* executor_= nullptr;
private:
    void paintRow(int row_index, const QColor& color_row, const QColor& color_text);
    void clearRowColor(int row_index);
};

#endif // DIIVIEWER_H
