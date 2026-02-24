#include "config_service.h"

#include <QtGlobal>

ConfigService::ConfigService(ConfigLoader loader)
    : loader_(std::move(loader))
{
}

bool ConfigService::load(const QString& settingsPath,
                         const QString& configPath,
                         AppConfig& outConfig,
                         ConfigError* error) const
{
    RawConfig raw;
    ConfigError localErr;

    if (!settingsPath.isEmpty()) {
        if (!loader_.loadFile(settingsPath, raw, &localErr)) {
            if (error) *error = localErr;
            return false;
        }
    }

    if (!configPath.isEmpty()) {
        RawConfig raw2;
        if (!loader_.loadFile(configPath, raw2, &localErr)) {
            if (error) *error = localErr;
            return false;
        }
        mergeAppend(raw, raw2);
    }

    // ---- Map RawConfig -> AppConfig ----
    AppConfig cfg;

    // Пример: host обязателен
    QString errMsg;
    /*if (!readRequiredString(raw, "host", cfg.host, &errMsg)) {
        if (error) {
            error->filePath.clear();
            error->line = -1;
            error->message = errMsg;
        }
        return false;
    }*/

    // Пример: timeoutMs не обязателен, дефолт 1000, диапазон 1..600000
    const int defTimeout = cfg.timeoutMs; // 1000
    if (!readInt(raw, "timeoutMs", cfg.timeoutMs, /*required=*/false, &defTimeout,
                 /*min=*/1, /*max=*/600000, &errMsg)) {
        if (error) {
            error->filePath.clear();
            error->line = -1;
            error->message = errMsg;
        }
        return false;
    }

    // Пример: ports может быть пустым, но если хочешь сделать обязательным — проверь.
    cfg.ports = getList(raw, "port");

    // Дополнительная валидация, если нужно:
    // if (cfg.ports.isEmpty()) { ... }

    outConfig = cfg;
    return true;
}

void ConfigService::mergeAppend(RawConfig& dst, const RawConfig& src)
{
    for (auto it = src.constBegin(); it != src.constEnd(); ++it) {
        const QString& key = it.key();
        const QStringList& values = it.value();

        if (!dst.contains(key))
            dst.insert(key, QStringList{});

        if (!values.isEmpty())
            dst[key].append(values);
    }
}

bool ConfigService::hasKey(const RawConfig& raw, const QString& key)
{
    return raw.contains(key);
}

QStringList ConfigService::getList(const RawConfig& raw, const QString& key)
{
    return raw.value(key);
}

QString ConfigService::getFirst(const RawConfig& raw, const QString& key, const QString& def)
{
    const auto it = raw.constFind(key);
    if (it == raw.constEnd() || it->isEmpty())
        return def;
    return it->first();
}

bool ConfigService::readRequiredString(const RawConfig& raw, const QString& key, QString& out, QString* errMsg)
{
    const QString v = getFirst(raw, key, QString());
    if (v.isEmpty()) {
        if (errMsg) *errMsg = QString("Отсутствует обязательный ключ '%1' или его значение пустое").arg(key);
        return false;
    }
    out = v;
    return true;
}

bool ConfigService::readInt(const RawConfig& raw, const QString& key, int& out, bool required,
                            const int* defValue, int minValue, int maxValue, QString* errMsg)
{
    const QString s = getFirst(raw, key, QString());
    if (s.isEmpty()) {
        if (required) {
            if (errMsg) *errMsg = QString("Отсутствует обязательный ключ '%1'").arg(key);
            return false;
        }
        if (defValue) out = *defValue;
        return true;
    }

    bool ok = false;
    const int val = s.toInt(&ok);
    if (!ok) {
        if (errMsg) *errMsg = QString("Ключ '%1' должен быть целым числом, получено '%2'").arg(key, s);
        return false;
    }

    if (val < minValue || val > maxValue) {
        if (errMsg) *errMsg = QString("Ключ '%1' вне диапазона [%2..%3], получено %4")
                                  .arg(key).arg(minValue).arg(maxValue).arg(val);
        return false;
    }

    out = val;
    return true;
}
