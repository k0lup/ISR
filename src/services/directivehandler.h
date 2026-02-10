#ifndef DIRECTIVEHANDLER_H
#define DIRECTIVEHANDLER_H

#include <QObject>

#include "../net/pormessage.h"

class IsrController;

class DirectiveHandler : public QObject {
    Q_OBJECT
public:
    explicit DirectiveHandler(IsrController *controller, QObject *parent = nullptr);

public slots:
    void onPorReceived(const PorMessage &message);

private:
    void handleNaSr(const PorMessage &message);
    void handleEd(const PorMessage &message);
    void handlePuskRazd(const PorMessage &message);

    IsrController *m_controller = nullptr;
};

#endif // DIRECTIVEHANDLER_H
