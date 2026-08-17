#include "Services/ConfigReader/configschema.h"
#include "Services/ConfigReader/configtypes.h"
#include <QString>


//ПРОВЕРИТЬ РЕГУЛЯРКИ
static const QString reg_exp_ip = QString(R"((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))");
static const QString reg_exp_port = QString(R"(^(?:[0-9]+|0x[0-9A-Fa-f]+)$)");

config_reader::ConfigSchema config_reader::createConfigShema() {
    config_reader::ConfigSchema config_schema = {
        /*Название параметра*/                          /*required*/   /*default_value*/                       /*type_def*/
        /*РАЗМЕЩЕНИЕ_СПО*/
        {app_config_names::spo_path_name,               {true,          QVariant(),                             ParamType::String,          "FOLDER"}},
        /*РАЗМЕЩЕНИЕ_НАСТРОЕК*/
        {app_config_names::settings_path_name,          {true,          QVariant(),                             ParamType::String,          "FOLDER"}},
        /*РАЗМЕЩЕНИЕ_ФАЙЛОВ*/
        {app_config_names::files_paths_name,            {true,          QVariant(),                             ParamType::String,          "FOLDER"}},
        /*РАЗМЕЩЕНИЕ_РАЗДЕЛОВ*/
        {app_config_names::sections_folder_path_name,   {true,          QVariant(),                             ParamType::String,          "FOLDER"}},
        /*ПРОТОКОЛ*/
        {app_config_names::prot_path_name,              {true,          QVariant(),                             ParamType::String,          "FOLDER"}},
        /*ПРОГРАММЫ*/
        {app_config_names::programs_paths_name,         {true,          QVariant(),                             ParamType::StringList,      "FOLDER"}},
        /*РАЗДЕЛЫ*/
        {app_config_names::sections_paths_name,         {true,          QVariant(),                             ParamType::StringList,      ""}},
        /*НШС_ИЗД*/
        {app_config_names::nshc_isd_name,               {true,          QVariant(),                             ParamType::String,          ""}},
        /*АВАР_ИЗД*/
        {app_config_names::avar_isd_name,               {true,          QVariant(),                             ParamType::String,          ""}},
        /*ПРИ_ИЗД*/
        {app_config_names::pri_isd_name,                {true,          QVariant(),                             ParamType::String,          ""}},
        /*НСШС_РЭП*/
        {app_config_names::nshc_rep_name,               {true,          QVariant(),                             ParamType::String,          ""}},
        /*АВАР_РЭП*/
        {app_config_names::avar_rep_name,               {true,          QVariant(),                             ParamType::String,          ""}},
        /*ПРИ_РЭП*/
        {app_config_names::pri_rep_name,                {true,          QVariant(),                             ParamType::String,          ""}},
        /*ISR_PORT*/
        {app_config_names::isr_port_name,               {false,         QVariant(1),                            ParamType::String,          reg_exp_port}},
        /*IP_EXCHANGE_SERVER*/
        {app_config_names::ip_server_name,              {false,         QVariant(QString("127.0.0.1")),         ParamType::String,          reg_exp_ip}},
        /*PORT_EXCHANGE_SERVER*/
        {app_config_names::port_server_name,            {false,         QVariant(1),                            ParamType::String,          reg_exp_port}}
    };

    return config_schema;
}
