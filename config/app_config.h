#pragma once

#include <QString>
#include <QStringList>

// Пример типизированной конфигурации.
// Переименуй/добавь поля под свои ключи.
struct AppConfig
{
    QString spo_path;
    QString settings_path;
    QString files_path;
    QString sections_path;

    QString prot_path;
    QStringList programs_paths;
    QStringList sections_paths;

    QString nshc_isd;
    QString avar_isd;
    QString pri_isd;

    QString nshc_rep;
    QString avar_rep;
    QString pri_rep;

    // Дополнительно можно добавить флаги/пути/режимы и т.п.
    // bool featureEnabled = false;
};
