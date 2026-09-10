#ifndef DIRECTIVEEXECUTOR_H
#define DIRECTIVEEXECUTOR_H

#include <QObject>
#include <QVector>
#include <QPointer>
#include <QSet>

#include "Services/DirectiveExecutor/program.h"

class Direct;

class DirectiveExecutor : public QObject
{
    Q_OBJECT
public:
    static const quint64 NO_REQUEST_ID = 0;

    enum class ExecutorState
    {
        IDLE,
        RUNNING,
        WAITING_GUI,
        WAITING_NETWORK,
        WAITING_PROGRAM,
        ERROR
    };

    enum class ExecutorMode
    {
        STEP,
        AUTO,
        NO_MODE
    };

public:
    explicit DirectiveExecutor(QObject *parent = nullptr);

signals:
    void directiveStarted(const SectionForGUI& frame);

    void directiveFinished(const SectionForGUI& frame);

    void requestShowWindow(RequestId request_id/**/);
    void requestSendDataToPris(RequestId request_id/**/);
    void requestStartProgram(RequestId request_id/**/);

    void programFinished(bool success); //А НУЖНО ЛИ?

    void failed(const QString& message);

    void sectionSetted(const SectionForGUI& prog_info);

public slots:
    void startFrom(const int index, const ExecutorMode mode);

    void continueExecution(); //ДУМАЮ, ЧТО МОЖНО УБРАТЬ

    void onWindowResponse(RequestId request_id/**/);
    void onPrisResponse(RequestId request_id/**/);

    void onProgramReady(RequestId request_id/**/);

    void onFailedDetected(RequestId request_id, const QString& message);

    void closeCurrentProgram();

    void setSection(SectionStruct section, const RequestId request_id = 0);

private:
    enum class RequestType
    {
        GUI,
        PRIS,
        PROGRAM
    };

    struct ExecutionPosition
    {
        ChapterType active_chapter = ChapterType::STATE;
        int active_direct_index = -1;;
        int stack_index = -1;
        Direct* actitve_direct = nullptr;
    };

    struct ExecutionFrame
    {
        SectionStruct section;

        ExecutorMode mode = ExecutorMode::STEP;

        ExecutionPosition execution_position;

        RequestId caller_request_id = 0;

        SectionForGUI toGUI() {
            SectionForGUI res;
            res.active_chpater = execution_position.active_chapter;
            res.active_dir_index = execution_position.active_direct_index;
            res.section = section;
            return res;
        }
    };

    struct PendingRequest
    {
        RequestType type;

        ExecutionPosition execution_position;
    };

    struct InfoAboutRequestedProgram
    {
        ChapterType chapter;
        ExecutorMode mode;
        QString label_nshs;
    };

private:
    void registerDirectives(const QVector<Direct*> directives);

    void startCurrentDirective();

    void onDirectiveFinished(
            const ExecutionPosition& exec_pos
            /*const Direct::ResultDirective& result*/);
    void onFiledDetected();

    void onShowWindowRequested(
            const ExecutionPosition& exec_pos
            /*const WidgetTypes& type,*/
            /*const WidgetInfo& info*/);

    void onPrisRequested(
            const ExecutionPosition& exec_pos
            /*const PRISMessage& message*/);

    void onProgramRequested(
            const ExecutionPosition& exec_pos
            /*const START_SECTION_PARAMS& params*/);

    RequestId createRequest(
            RequestType type,
            const ExecutionPosition& exec_pos);

    Direct* takeRequest(
            RequestId requestId,
            RequestType expectedType);

    void cancelRequests(const ExecutionPosition& exec_pos);

    ExecutionFrame* currentFrame();
    const ExecutionFrame* currentFrame() const;

    Direct* currentDirective() const;

    void finishCurrentProgram(bool success);

private:
    QVector<ExecutionFrame> stack_;

    QHash<RequestId, PendingRequest> pendingRequests_;

    QSet<Direct*> registeredDirectives_;

    RequestId nextRequestId_ = 1;

    ExecutorState state_ = ExecutorState::IDLE;

    QHash<RequestId, InfoAboutRequestedProgram> info_about_req_progs_;
};

#endif // DIRECTIVEEXECUTOR_H
