#pragma once

#include "app_config.h"
#include "config_loader.h"

class ConfigService
{
public:
    explicit ConfigService(ConfigLoader loader = ConfigLoader{});

    // settingsPath / configPath могут быть пустыми (тогда файл пропускается)
    bool load(const QString& settingsPath,
              const QString& configPath,
              AppConfig& outConfig,
              ConfigError* error = nullptr) const;

private:
    ConfigLoader loader_;

    static void mergeAppend(RawConfig& dst, const RawConfig& src);

    // ---- Raw access helpers ----
    static bool hasKey(const RawConfig& raw, const QString& key);
    static QStringList getList(const RawConfig& raw, const QString& key);
    static QString getFirst(const RawConfig& raw, const QString& key, const QString& def = QString());

    // ---- Typed helpers ----
    static bool readRequiredString(const RawConfig& raw, const QString& key, QString& out, QString* errMsg);
    static bool readInt(const RawConfig& raw, const QString& key, int& out, bool required,
                        const int* defValue, int minValue, int maxValue, QString* errMsg);
};
