#ifndef DIRECTIVEEXECUTOR_H
#define DIRECTIVEEXECUTOR_H

#include <QObject>
#include <QVector>
#include <QPointer>
#include <QSet>

#include "Services/Programs/programtypes.h"
#include "Services/DirectiveExecutor/executiontypes.h"
#include "Services/RequestIDGenerator/requestIDGenerator.h"
#include "Services/Directives/directive.h"
#include "Services/Directives/startsectiondirective.h"

class DirectiveExecutor : public QObject
{
    Q_OBJECT
public:
    explicit DirectiveExecutor(QObject* parent = nullptr);

public slots:
    void setSection(SectionStruct& section_struct, const RequestId request_id, StartSectionDirective* caller = nullptr);
    void startFrom(const ExecutionSnapshot& section_data, const RequestId request_id);

    void setExecuteMode(const ExecuteMode& execute_mode, const RequestId request_id);

    void onWindowResponse(const WindowResultData& result, const RequestId source_request_id);
    void onPRISResponse(const PRISResultData& result, const RequestId source_request_id);
    void onLoadSectionResponse(SectionStruct& section_data,  const QStringList& error_messages, const RequestId source_request_id);

    void onDirectiveFinished(const Direct::ResultDirective& result, Direct* direct);
    void onDirectiveFailed(const QString& message, Direct* direct);
signals:
    void sectionChanged(const ExecutionSnapshot& section);

    void requestCompleted(const RequestId source_request_id);
    void requestFailed(const RequestId source_request_id, const QString& message);

    void warning(const QString& message);
    void failed(const QString& message);

    void requestShowWindow(const WidgetTypes& types, const WidgetInfo& info, const RequestId request_id);
    void requestSendDataToPris(const PRISMessage& message, const RequestId request_id);
    void requestLoadSection(const ExecuteParam& params, const RequestId request_id);
private:
    struct ExecuteFrame
    {
        SectionStruct section_struct;
        QMap<ChapterType, QVector<Direct*>> directives_for_execute;

        ChapterType active_chapter;
        int current_directive_index = 0;
        StartSectionDirective* caller = nullptr;

        int id_program = 0;

        const ExecutionSnapshot toGUI() {
            ExecutionSnapshot result;
            result.active_chpater = active_chapter;
            result.active_dir_index = current_directive_index;
            result.section = section_struct;
            result.exec_frame_id = id_program;

            return result;
        }
    };

private:
    QVector<Direct*> createDirectiveFromChapter(const ChapterStruct& chapter, QStringList& error_messages);
    Direct* createDirect(const DirectiveStruct& directive);
    ExecuteParam getParamFromStartDirective(StartSectionDirective* directive);
    void continueExecute();
    QPointer<Direct> getCurrentExecuteDirect();
    int getNextID() {
        id_last_prog += 1;
        return id_last_prog;
    }
private:
    ExecuteMode execute_mode_ = ExecuteMode::STEP;
    QVector<ExecuteFrame> sections_;

    QMap<RequestId, QPointer<Direct>> request_directives_;

    int id_last_prog = 0;
};

#endif // DIRECTIVEEXECUTOR_H
