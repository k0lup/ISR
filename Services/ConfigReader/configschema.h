#ifndef CONFIGSHEMA_H
#define CONFIGSHEMA_H

#include <QString>
#include <QVariant>
#include <QMap>

namespace config_reader {
    enum class ParamType {
        String,
        StringList,
        Int,
        Empty
    };

    struct ParamInfo
    {
        bool required = false;                  //обязательно ли параметр должен быть предоставлен
        QVariant default_value;                 //значение по умолчанию (если есть)
        ParamType type = ParamType::String;     //тип параметра
    };

    using ConfigSchema = QMap<QString, ParamInfo>;

    ConfigSchema createConfigShema();
}

#endif // CONFIGSHEMA_H
