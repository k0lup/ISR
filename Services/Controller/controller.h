#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <QObject>
#include "Services/ConfigReader/configtypes.h"
#include "appstate.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(const AppConfig& cfg, QObject* parent = nullptr);
signals:
    void stateChanged(const AppState app_state);
public slots:
    void start();
    void stop();

    void onServicesStartWorked();
    void onServisecFinishWorked();
    void onFailedDetected();
private:
    void setState(const AppState app_state);
private:
    AppState app_state_;

    const AppConfig& cfg_;
};

#endif // CONTROLLER_H
