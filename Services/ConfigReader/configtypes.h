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
    QString sections_folder_path;   /*.ON*/                 /*РАЗМЕЩЕНИЕ РАЗДЕЛОВ=*/

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
    int ip_server;                  /*.ON_S284*/            /*IP_EXCHANGE_SERVER=*/
    int port_server;                /*.ON_S284*/            /*PORT_EXCHANGE_SERVER=*/
};

#endif // CONFIGTYPES_H
