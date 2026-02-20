#pragma once

#include <QString>
#include <QStringList>

// Пример типизированной конфигурации.
// Переименуй/добавь поля под свои ключи.
struct AppConfig
{
    QString host;            // key: host=...
    int timeoutMs = 1000;    // key: timeoutMs=...
    QStringList ports;       // key: port=8080;8081 (и/или повторяющиеся port=...)

    // Дополнительно можно добавить флаги/пути/режимы и т.п.
    // bool featureEnabled = false;
};
