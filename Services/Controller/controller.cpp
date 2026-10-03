#include "controller.h"

Controller::Controller(QObject* parent) : QObject(parent)
{
    setState(AppState::Loading);
}

void Controller::setState(const AppState app_state)
{
    app_state_ = app_state;
    emit stateChanged(app_state_);
}

void Controller::start()
{

    setState(AppState::Idle);
}

void Controller::stop()
{

    setState(AppState::Finished);
}

void Controller::onServicesStartWorked()
{

    setState(AppState::Working);
}

void Controller::onServisecFinishWorked()
{

    setState(AppState::Idle);
}

void Controller::onFailedDetected()
{

    setState(AppState::Failed);
}
