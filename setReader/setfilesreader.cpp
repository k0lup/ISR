#include "setReader/setfilesreader.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include "logger/logging_categories.h"
#include <QLoggingCategory>


bool SetFilesReader::readFile(const QString& file_path, ErrorReadFile& error) {
    source_lines_.clear();

    error.filePath = file_path;
    error.line = -1;
    error.message = "";

    qCInfo(logCore) << QString("Открываем файл настройки: ") + file_path;
    QFile file(file_path);
    if (!file.exists()) {
        error.message = "Не удалось найти указанный файл";
        qCWarning(logCore) << error.toString();
        return false;
    }

    if (QFileInfo(file).suffix().toUpper() != "SET") {
        error.message = "Расширение файла отличается от .set";
        qCWarning(logCore) << error.toString();
        return false;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error.message = QString("Не удалось открыть файл: ") + file.errorString();
        qCWarning(logCore) << error.toString();
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        source_lines_.append(in.readLine());
    }

    if (!isValidCheckSum()) {
        error.message = "Не совпадает контрольная сумма файла!";
        qCWarning(logCore) << error.toString();
        return false;
    }

    file_data_ = parseFile(error, file_path);
    if (!error.message.isEmpty()) {
        qCWarning(logCore) << error.toString();
        file_data_ = SetFileData{};
        return false;
    }

    qCInfo(logCore) << QString("Файл настройки %1 успешно прочитан").arg(file_path);
    return true;
}

SetFileData SetFilesReader::getFileData() const {
    return file_data_;
}


enum class ColumnName {
    TYPE = 0,
    ABBREVIATION = 1,
    DIRECTORY = 2,
    STRUCTURE = 3,
    SPO_PROCESSING = 4,
    SPO_ASSESSMEN = 5,
    TOTAL_COUNT_COLUMN = 7
};

static constexpr int idx(ColumnName c) { return static_cast<int>(c); }

SetFileData SetFilesReader::parseFile(ErrorReadFile& error, const QString& file_path) const {
    qCInfo(logCore) << QString("Парсим файл настройки %1").arg(file_path);
    error.filePath = file_path;
    SetFileData file_data;

    if (source_lines_.count() <= 6) {
        error.line = -1;
        error.message = "В файле менее 6 строк! Файл должен содержать какие-либо данные кроме паспорта";
        return file_data;
    }

    for (int line = 6; line < source_lines_.count(); ++line) {
        QStringList curLine = source_lines_[line].split("|", Qt::KeepEmptyParts);
        if (curLine.count() != idx(ColumnName::TOTAL_COUNT_COLUMN)) {
            error.line = line + 1;
            error.message = QString("Строка не соотвествует ТФ (меньше столбцов, чем требуется)!");
            return file_data;
        }

        if (curLine[idx(ColumnName::TYPE)] != "П" || curLine[idx(ColumnName::TYPE)].isEmpty()) {
            continue;
        }

        if (!curLine[idx(ColumnName::ABBREVIATION)].isEmpty()) {
            if (file_data.abbreviation.contains(curLine[idx(ColumnName::ABBREVIATION)])) {
                error.line = line + 1;
                error.message = QString("Повтороно объявлена аббревиатура: ") + curLine[idx(ColumnName::ABBREVIATION)];
                return file_data;
            }
            file_data.abbreviation.append(curLine[idx(ColumnName::ABBREVIATION)]);
        }

        if (!curLine[idx(ColumnName::DIRECTORY)].isEmpty()) {
            if (file_data.abbreviation.isEmpty()) {
                error.line = line + 1;
                error.message = "Поле директория не соответствует ни одной аббревиатуре";
                return file_data;
            }
            QStringList list_directories = file_data.directory[file_data.abbreviation.last()];
            if (list_directories.contains(curLine[idx(ColumnName::DIRECTORY)])) {
                error.line = line + 1;
                error.message = QString("Повтороно указана диретория (%1) для аббревиатуры (%2)")
                        .arg(curLine[idx(ColumnName::DIRECTORY)])
                        .arg(file_data.abbreviation.last());
                return file_data;
            }
            list_directories.append(curLine[idx(ColumnName::DIRECTORY)]);
            file_data.directory[file_data.abbreviation.last()] = list_directories;
        }

        if (!curLine[idx(ColumnName::STRUCTURE)].isEmpty()) {
            if (curLine[idx(ColumnName::ABBREVIATION)].isEmpty()) {
                error.line = line + 1;
                error.message = "Поле структура не соотвествует ни одной аббревиатуре";
                return file_data;
            }
            if (!file_data.structure[curLine[idx(ColumnName::ABBREVIATION)]].isEmpty()) {
                error.line = line + 1;
                error.message = "Для этой аббревиатуры уже была задана структура";
                return file_data;
            }
            file_data.structure[curLine[idx(ColumnName::ABBREVIATION)]] = curLine[idx(ColumnName::STRUCTURE)];
        }
    }

    if (file_data.abbreviation.isEmpty()) {
        error.line = -1;
        error.message = "В файле настройки не найдено ни одной аббревиатуры";
        return file_data;
    }

    qCInfo(logCore) << QString("Успешно распарсили файл настройки %1").arg(file_path);
    return file_data;
}

bool SetFilesReader::isValidCheckSum() const {
    return true;
}

CheckSumType SetFilesReader::calculateCheckSum(const QStringList& file_data) const {
    return 0;
}

CheckSumType SetFilesReader::getCheckSumFromString(const QStringList& file_data) const {
    return 0;
}
