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
    RawConfig raw_setting;
    RawConfig raw_config;
    ConfigError localErr;

    if (!settingsPath.isEmpty()) {
        if (!loader_.loadFile(settingsPath, raw_setting, &localErr)) {
            if (error) *error = localErr;
            return false;
        }
    }

    if (!configPath.isEmpty()) {
        if (!loader_.loadFile(configPath, raw_config, &localErr)) {
            if (error) *error = localErr;
            return false;
        }
        //mergeAppend(raw, raw2);
    }

    // ---- Map RawConfig -> AppConfig ----
    AppConfig cfg;

    QStringList programs_list = getList(raw_setting, "ПРОГРАММЫ");
    if (programs_list.isEmpty()) {
        if (error) {
            error->filePath.clear();
            error->line = -1;
            error->message = "НЕ НАЙДЕН ОБЯЗАТЕЛЬНЫЙ ПАРАМЕТР ПРОГРАММЫ";
        }
        return false;
    }
    cfg.programs_paths = programs_list;

    QStringList sections_list = getList(raw_setting, "РАЗДЕЛЫ");
    if (programs_list.isEmpty()) {
        if (error) {
            error->filePath.clear();
            error->line = -1;
            error->message = "НЕ НАЙДЕН ОБЯЗАТЕЛЬНЫЙ ПАРАМЕТР РАЗДЕЛЫ";
        }
        return false;
    }
    cfg.sections_paths = sections_list;


    QString errMsg;
    if (!readRequiredString(raw_setting, "РАЗМЕЩЕНИЕ_СПО", cfg.spo_path, &errMsg) ||
        !readRequiredString(raw_setting, "РАЗМЕЩЕНИЕ_НАСТРОЕК", cfg.settings_path, &errMsg) ||
        !readRequiredString(raw_setting, "РАЗМЕЩЕНИЕ_ФАЙЛОВ", cfg.files_path, &errMsg) ||
        !readRequiredString(raw_setting, "РАЗМЕЩЕНИЕ_РАЗДЕЛОВ", cfg.sections_folder_path, &errMsg) ||
        !readRequiredString(raw_setting, "ПРОТОКОЛ", cfg.files_path, &errMsg) ||
        !readRequiredString(raw_setting, "НШС_ИЗД", cfg.nshc_isd, &errMsg) ||
        !readRequiredString(raw_setting, "АВАР_ИЗД", cfg.avar_isd, &errMsg) ||
        !readRequiredString(raw_setting, "ПРИ_ИЗД", cfg.pri_isd, &errMsg) ||
        !readRequiredString(raw_setting, "НШС_РЭП", cfg.nshc_rep, &errMsg) ||
        !readRequiredString(raw_setting, "ПРИ_РЭП", cfg.pri_rep, &errMsg) ||
        !readRequiredString(raw_setting, "АВАР_РЭП", cfg.avar_rep, &errMsg))
    {
        if (error) {
            error->filePath.clear();
            error->line = -1;
            error->message = errMsg;
        }
        return false;
    }
    /*if (!readRequiredString(raw, "host", cfg.host, &errMsg)) {
        if (error) {
            error->filePath.clear();
            error->line = -1;
            error->message = errMsg;
        }
        return false;
    }*/

    // Пример: timeoutMs не обязателен, дефолт 1000, диапазон 1..600000
    //const int defTimeout = cfg.timeoutMs; // 1000
    //if (!readInt(raw, "timeoutMs", cfg.timeoutMs, /*required=*/false, &defTimeout,
    //             /*min=*/1, /*max=*/600000, &errMsg)) {
    //    if (error) {
    //        error->filePath.clear();
    //        error->line = -1;
    //        error->message = errMsg;
    //    }
    //    return false;
    //}

    // Пример: ports может быть пустым, но если хочешь сделать обязательным — проверь.
    //cfg.ports = getList(raw, "port");

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
