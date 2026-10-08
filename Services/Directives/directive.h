#ifndef DIRECTIVE_H
#define DIRECTIVE_H

#include <QObject>
#include <QMap>
#include "Services/Directives/directivetypes.h"
#include "Services/Programs/programtypes.h"
#include "Services/GUI/guitypes.h"
#include "Services/PRIS/pristypes.h"
#include "Services/DirectiveExecutor/executiontypes.h"

class Direct : public QObject
{
    Q_OBJECT
public:
    enum class RESULT_DIRECTIVE_TYPES {
        OK,
        NOT_COMPLETED,
        ERROR
    };

    struct ResultDirective {
        RESULT_DIRECTIVE_TYPES type;
        QString message;
    };

    Direct(const DirectiveStruct& directive, QObject *parent = nullptr) : QObject(parent) {
        m_directive_ = directive;
    }

    virtual bool isValid() = 0;
    virtual const QStringList& getErrorValid() const {return errors_valid_;};
    virtual bool isRunned() const {
        return
                directive_state_ == DirectiveState::Running ||
                directive_state_ == DirectiveState::WaitingForGui ||
                directive_state_ == DirectiveState::WaitingForPris ||
                directive_state_ == DirectiveState::WaitingForProgram;
                                  };

    virtual void run() = 0;
    virtual DirectType getTypeDirect() const = 0;
    virtual ~Direct() = default;
signals:
    void showWindow(const WidgetTypes& types, const WidgetInfo& info);
    void sendMessageToPris(const PRISMessage& message);
    void requestStartProgram(const ExecuteParam& params);
    void finished(const ResultDirective& result);

    void failed(const QString& message);
public slots:
    virtual void onWindowResult(const WindowResultData& result) {
        Q_UNUSED(result);
    }
    virtual void onPRISResult(const PRISResultData& result) {
        Q_UNUSED(result);
    }
    virtual void onReqStartProgResult(const bool result) {
        Q_UNUSED(result);
    }
protected:
    virtual void parseDirective() = 0;
protected slots:
    virtual void onFiledDetected(const QString& message) {
        Q_UNUSED(message);
    }
    virtual void onFinishedDetected(const ResultDirective& result) {
        Q_UNUSED(result);
    }

    QString directStateToString() const {
        switch (directive_state_) {
        case (DirectiveState::Created) : return "Created";
        case (DirectiveState::Ready) : return "Ready";
        case (DirectiveState::Running) : return "Running";
        case (DirectiveState::WaitingForGui) : return "WaitingForGui";
        case (DirectiveState::WaitingForPris) : return "WaitingForPris";
        case (DirectiveState::WaitingForProgram) : return "WaitingForProgram";
        case (DirectiveState::Finished) : return "Finished";
        case (DirectiveState::Failed) : return "Failed";
        }
    }
protected:
    enum class DirectiveState
    {
        Created,
        Ready,
        Running,
        WaitingForGui,
        WaitingForPris,
        WaitingForProgram,
        Finished,
        Failed
    };
protected:
    DirectiveStruct m_directive_;

    QStringList errors_valid_;
    DirectiveState directive_state_ = DirectiveState::Created;
};

#endif // DIRECTIVE_H
