#ifndef EXECUTOR_H
#define EXECUTOR_H
#include <QObject>
#include <QVector>
#include "setReader/sectionsloader.h"
#include "Directives/directive.h"


class Executor : public QObject
{
    Q_OBJECT
public:
    enum class ExecutorMode {
        STEP,
        AUTO,
        ERROR
    };

    enum class ExecutorState {
        IDLE,
        RUNNING
    };

    struct CallStackObject {
        Section section;
        int cur_index;
        ExecutorMode mode;
        ExecutorState state;

        QVector<QMetaObject::Connection> last_connections_;
    };

public:
    Executor(QObject* parent = nullptr);

    //void addSection(const QVector<Direct*>& directives);
    //void addSection(const Section section);
    void clearSections();


    int currentIndex() const;
    ExecutorMode mode() const;
    ExecutorState state() const;
signals:
    void directiveFinished(const Direct::ResultDirective result, const int index);
    void curDirectiveChanged(const int new_cur_index);
    /*
     * void showWindow(const WidgetTypes& types, const WidgetInfo& info);
    void sendMessageToPris(const PRISMessage& message);
    void requestStartProgram(const START_SECTION_PARAMS& params);
     */
    void requestShowWindow(const WidgetTypes& type, const WidgetInfo& info);
    void requestSendDataToPris(const PRISMessage& message);
    void requestStartProgram(const START_SECTION_PARAMS& params);

    void windowResponse(const WindowResultData& data);
    void PRISResponse(const PRISResultData& data);
    void progStarted(const bool status);

    void failed(const QString& failed_message);

    void showSection(const Section& section);
public slots:
    void startFrom(const int index, const ExecutorMode mode);
    void addSection(const Section& section);
private slots:
    void onDirectiveFinished(const Direct::ResultDirective& result);
private:
    void startDirective();
private:

    //DELETE START
    QVector<Direct*> directives_;
    int cur_index_;
    ExecutorMode mode_;
    ExecutorState state_;

    QVector<QMetaObject::Connection> last_connections_;
    //DELETE END

    QVector<CallStackObject> call_stack;

};

#endif // EXECUTOR_H
