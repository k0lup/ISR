#ifndef DIRECTIVE_H
#define DIRECTIVE_H

#include <QObject>
#include "setReader/diireader.h"

enum class TypeDirect {
    MAIN_OPERATION,
    VARIANT,
    START_SECTION,
    COMMAND,
    DOKLAD,
    INSTRUCTIONS,
    INVALID
};

enum class WidgetTypes {
    INFO,
    VARIANT,
    WARNING
};

struct WidgetInfo {
    QString title;
    QStringList information;

    void clear() {
        title.clear();
        information.clear();
    }
};

struct WindowResultData {
    bool status;
    QString data;
};

struct PRISResultData {
    bool status;
    QByteArray data;
};

struct PRISMessage {
    QString message;
};

class Direct : public QObject
{
    Q_OBJECT
public:
    enum class RESULT_DIRECTIVE_TYPES {
        OK,
        NOT_OK,
        NOT_COMPLETED,
        ERROR
    };

    struct ResultDirective {
        RESULT_DIRECTIVE_TYPES type;
        QString message;
    };

    Direct(const Command directive, QObject *parent = nullptr) : QObject(parent) {
        m_directive_ = directive;
    }

    virtual bool isValid(QStringList& errors) const = 0;
    //virtual RESULT_DIRECTIVE runDirect() = 0;
    virtual void start() = 0;
    virtual TypeDirect getTypeDirect() const = 0;
    virtual ~Direct() = default;
signals:
    void showWindow(const WidgetTypes& types, const WidgetInfo& info);
    void sendMessageToPris(const PRISMessage& message);
    void finished(const ResultDirective& result);
public slots:
    virtual void onWindowResult(const WindowResultData& result) {
        Q_UNUSED(result);
    }
    virtual void onPRISResult(const PRISResultData& result) {
        Q_UNUSED(result);
    }
protected:
    Command m_directive_;
};

#endif // DIRECTIVE_H
