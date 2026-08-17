#ifndef CONFIGSHEMA_H
#define CONFIGSHEMA_H

#include <QString>
#include <QVariant>
#include <QMap>

namespace config_reader {
    enum class ParamType {
        String,
        StringList
    };

    struct ParamInfo
    {
        bool required = false;                  //обязательно ли параметр должен быть предоставлен
        QVariant default_value;                 //значение по умолчанию (если есть)
        ParamType type = ParamType::String;     //тип параметра
        QString reg_exp = "";                   //Рег. выражения для валидации (если нужно)
    };

    using ConfigSchema = QMap<QString, ParamInfo>;

    ConfigSchema createConfigShema();

    //Эти значения нужны, чтобы мы проверяли корректность пути для текущей ОС. Это понятнее и проще, чем писать регулярку
    //да и регуляркой существование пути проверить не получится
    const QString folder_path_reg_exp_const = "FOLDER_PATH";        //Значение ParamInfo::reg_exp, при котором мы должны проверить корректность пути к папке
    const QString file_path_reg_exp_const = "FILE_PATH";            //Значение ParamInfo::reg_exp, при котором мы должны проверить корректность пути к файлу
}

#endif // CONFIGSHEMA_H
