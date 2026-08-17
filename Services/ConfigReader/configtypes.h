#ifndef CONFIGTYPES_H
#define CONFIGTYPES_H

#include <QString>
#include <QStringList>
#include <QMap>

// ФАЙЛ .CFG - входный файл конфигурации
// ФАЙЛ .ON - входный файл настроек пользователя
// ФАЙЛ .ON_S284 - входной файл тех. настроек программы. Используется для упрощения отладки. Редактируется только разработчиками

struct AppConfig {
    /*ПАРАМЕТР*/                    /*ТИП ФАЙЛА*/           /*Наименование параметра*/
    QString spo_path;               /*.ON*/                 /*РАЗМЕЩЕНИЕ_СПО=*/
    QString settings_path;          /*.ON*/                 /*РАЗМЕЩЕНИЕ_НАСТРОЕК=*/
    QString files_paths;            /*.ON*/                 /*РАЗМЕЩЕНИЕ_ФАЙЛОВ=*/
    QString sections_folder_path;   /*.ON*/                 /*РАЗМЕЩЕНИЕ_РАЗДЕЛОВ=*/

    QString prot_path;              /*.ON*/                 /*ПРОТОКОЛ=*/
    QStringList programs_paths;     /*.ON*/                 /*ПРОГРАММЫ=*/
    QStringList sections_paths;     /*.ON*/                 /*РАЗДЕЛЫ=*/

    QString nshc_isd;               /*.ON*/                 /*НШС_ИЗД=*/
    QString avar_isd;               /*.ON*/                 /*АВАР_ИЗД=*/
    QString pri_isd;                /*.ON*/                 /*ПРИ_ИЗД=*/

    QString nshc_rep;               /*.ON*/                 /*НСШС_РЭП=*/
    QString avar_rep;               /*.ON*/                 /*АВАР_РЭП=*/
    QString pri_rep;                /*.ON*/                 /*ПРИ_РЭП=*/

    int isr_port;                   /*.ON_S284*/            /*ISR_PORT=*/
    QString ip_server;              /*.ON_S284*/            /*IP_EXCHANGE_SERVER=*/
    int port_server;                /*.ON_S284*/            /*PORT_EXCHANGE_SERVER=*/
};

//Названия используемых параметров

namespace app_config_names {
    const QString spo_path_name                 =   "РАЗМЕЩЕНИЕ_СПО";
    const QString settings_path_name            =   "РАЗМЕЩЕНИЕ_НАСТРОЕК";
    const QString files_paths_name              =   "РАЗМЕЩЕНИЕ_ФАЙЛОВ";
    const QString sections_folder_path_name     =   "РАЗМЕЩЕНИЕ_РАЗДЕЛОВ";

    const QString prot_path_name                =   "ПРОТОКОЛ";
    const QString programs_paths_name           =   "ПРОГРАММЫ";
    const QString sections_paths_name           =   "РАЗДЕЛЫ";

    const QString nshc_isd_name                 =   "НШС_ИЗД";
    const QString avar_isd_name                 =   "АВАР_ИЗД";
    const QString pri_isd_name                  =   "ПРИ_ИЗД";

    const QString nshc_rep_name                 =   "НСШС_РЭП";
    const QString avar_rep_name                 =   "АВАР_РЭП";
    const QString pri_rep_name                  =   "ПРИ_РЭП";

    const QString isr_port_name                 =   "ISR_PORT";
    const QString ip_server_name                =   "IP_EXCHANGE_SERVER";
    const QString port_server_name              =   "PORT_EXCHANGE_SERVER";
}

#endif // CONFIGTYPES_H
