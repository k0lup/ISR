#ifndef SETTINGSSTORE_H
#define SETTINGSSTORE_H
#include "Services/ConfigReader/configtypes.h"
#include <QReadWriteLock>

class SettingsStore
{
public:
    SettingsStore();
    const AppConfig& snapshot() const;

    void setAppCfg(const AppConfig& app_cfg);
    void setManualMode(const bool is_allowed);
    void setTraf(const QString& traf);
private:
    AppConfig app_cfg_;
    mutable QReadWriteLock lock_;
};

#endif // SETTINGSSTORE_H
