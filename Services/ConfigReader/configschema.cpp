#include "Services/ConfigReader/configschema.h"
#include "Services/ConfigReader/configtypes.h"

config_reader::ConfigSchema config_reader::createConfigShema() {
    config_reader::ConfigSchema config_schema = {
        /*Название параметра*/                          /*required*/   /*default_value*/                       /*type*/
        /*РАЗМЕЩЕНИЕ_СПО*/
        {app_config_names::spo_path_name,               {true,          QVariant(),                             ParamType::Empty}},
        /*РАЗМЕЩЕНИЕ_НАСТРОЕК*/
        {app_config_names::settings_path_name,          {true,          QVariant(),                             ParamType::Empty}},
        /*РАЗМЕЩЕНИЕ_ФАЙЛОВ*/
        {app_config_names::files_paths_name,            {true,          QVariant(),                             ParamType::Empty}},
        /*РАЗМЕЩЕНИЕ_РАЗДЕЛОВ*/
        {app_config_names::sections_folder_path_name,   {true,          QVariant(),                             ParamType::Empty}},
        /*ПРОТОКОЛ*/
        {app_config_names::prot_path_name,              {true,          QVariant(),                             ParamType::Empty}},
        /*ПРОГРАММЫ*/
        {app_config_names::programs_paths_name,         {true,          QVariant(),                             ParamType::Empty}},
        /*РАЗДЕЛЫ*/
        {app_config_names::sections_paths_name,         {true,          QVariant(),                             ParamType::Empty}},
        /*НШС_ИЗД*/
        {app_config_names::nshc_isd_name,               {true,          QVariant(),                             ParamType::Empty}},
        /*АВАР_ИЗД*/
        {app_config_names::avar_isd_name,               {true,          QVariant(),                             ParamType::Empty}},
        /*ПРИ_ИЗД*/
        {app_config_names::pri_isd_name,                {true,          QVariant(),                             ParamType::Empty}},
        /*НСШС_РЭП*/
        {app_config_names::nshc_rep_name,               {true,          QVariant(),                             ParamType::Empty}},
        /*АВАР_РЭП*/
        {app_config_names::avar_rep_name,               {true,          QVariant(),                             ParamType::Empty}},
        /*ПРИ_РЭП*/
        {app_config_names::pri_rep_name,                {true,          QVariant(),                             ParamType::Empty}},
        /*ISR_PORT*/
        {app_config_names::isr_port_name,               {false,         QVariant(1),                            ParamType::Int}},
        /*IP_EXCHANGE_SERVER*/
        {app_config_names::ip_server_name,              {false,         QVariant(1),                            ParamType::Int}},
        /*PORT_EXCHANGE_SERVER*/
        {app_config_names::port_server_name,            {false,         QVariant(1),                            ParamType::Int}},
    };

    return config_schema;
}
