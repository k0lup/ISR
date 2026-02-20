#pragma once

#include "config_types.h"

class ConfigLoader
{
public:
    struct Options
    {
        bool allowComments;      // поддержка комментариев (# и //)
        bool strict;             // если true: любая плохая строка -> ошибка и false
        bool keepEmptyValues;    // если true: "a=;" добавит пустые значения

        Options()
            : allowComments(true)
            , strict(false)
            , keepEmptyValues(false)
        {}
    };

    explicit ConfigLoader(Options opt = Options());

    // Читает файл и дописывает в out (append по ключам).
    // Возвращает false при фатальной ошибке (не открыть файл / strict-ошибка парсинга)
    bool loadFile(const QString& path, RawConfig& out, ConfigError* error = nullptr) const;

private:
    Options opt_;

    bool parseLine(const QString& line,
                   int lineNo,
                   const QString& filePath,
                   RawConfig& out,
                   ConfigError* error) const;

    bool isCommentLine(const QString& trimmedLine) const;

    static void setError(ConfigError* error,
                         const QString& filePath,
                         int lineNo,
                         const QString& message);

    static QString trimmed(const QString& s);
};
