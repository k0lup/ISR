#include "settingsstore.h"

SettingsStore::SettingsStore()
{

}

void SettingsStore::setAppCfg(const AppConfig &app_cfg)
{
    QReadLocker locker(&lock_);

    app_cfg_ = app_cfg;
}

const AppConfig& SettingsStore::snapshot() const
{
    QReadLocker locker(&lock_);

    return app_cfg_;
}

void SettingsStore::setManualMode(bool is_allowed)
{
    QWriteLocker locker(&lock_);

    app_cfg_.manual_start_direct_mode = is_allowed;
}

void SettingsStore::setTraf(const QString &traf)
{
    QWriteLocker locker(&lock_);

    app_cfg_.traf = traf;
}
