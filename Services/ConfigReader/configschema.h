#ifndef CONFIGSHEMA_H
#define CONFIGSHEMA_H

#include <QString>
#include <QVariant>
#include <QMap>

namespace config_reader {
    enum class ParamType {
        String,
        StringList,
        Int
    };

    struct ParamInfo
    {
        bool required = false;
        QVariant default_value;
        ParamType type = ParamType::String;
    };

    using ConfigSchema = QMap<QString, ParamInfo>;

    ConfigSchema createConfigShema();
}

#endif // CONFIGSHEMA_H
