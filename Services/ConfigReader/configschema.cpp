#include "Services/ConfigReader/configschema.h"

config_reader::ConfigSchema config_reader::createConfigShema() {
    config_reader::ConfigSchema config_schema = {
        /*Название параметра*/       /*required*/   /*default_value*/                       /*type*/
        {"РАЗМЕЩЕНИЕ_СПО",          {true,          QVariant(),                             ParamType::Empty}},
        {"РАЗМЕЩЕНИЕ_НАСТРОЕК",     {true,          QVariant(),                             ParamType::Empty}},
        {"РАЗМЕЩЕНИЕ_ФАЙЛОВ",       {true,          QVariant(),                             ParamType::Empty}},
        {"РАЗМЕЩЕНИЕ_РАЗДЕЛОВ",     {true,          QVariant(),                             ParamType::Empty}},
        {"ПРОТОКОЛ",                {true,          QVariant(),                             ParamType::Empty}},
        {"ПРОГРАММЫ",               {true,          QVariant(),                             ParamType::Empty}},
        {"РАЗДЕЛЫ",                 {true,          QVariant(),                             ParamType::Empty}},
        {"НШС_ИЗД",                 {true,          QVariant(),                             ParamType::Empty}},
        {"АВАР_ИЗД",                {true,          QVariant(),                             ParamType::Empty}},
        {"ПРИ_ИЗД",                 {true,          QVariant(),                             ParamType::Empty}},
        {"НСШС_РЭП",                {true,          QVariant(),                             ParamType::Empty}},
        {"АВАР_РЭП",                {true,          QVariant(),                             ParamType::Empty}},
        {"ПРИ_РЭП",                 {true,          QVariant(),                             ParamType::Empty}},
        {"ISR_PORT",                {false,         QVariant(1),                            ParamType::Int}},
        {"IP_EXCHANGE_SERVER",      {false,         QVariant(1),                            ParamType::Int}},
        {"PORT_EXCHANGE_SERVER",    {false,         QVariant(1),                            ParamType::Int}},
    };

    return config_schema;
}
