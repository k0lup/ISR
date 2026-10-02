#ifndef SETFILESREADER_H
#define SETFILESREADER_H
#include <QString>
#include <QMap>
#include <QStringList>
#include "ErrorReadFileStruct.h"

struct SetFileData {
    QStringList abbreviation;
    QMap<QString, QStringList> directory;
    QMap<QString, QString> structure;
};

using CheckSumType = int;

class SetFilesReader
{
public:
    SetFilesReader() = default;

    bool readFile(const QString& file_path, ErrorReadFile& error);
    SetFileData getFileData() const;

    bool isValidCheckSum() const;
private:
    CheckSumType calculateCheckSum(const QStringList& file_data) const;
    CheckSumType getCheckSumFromString(const QStringList& file_data) const;

    SetFileData parseFile(ErrorReadFile& error, /*Для вывода имени файла в error*/const QString& filePath) const;

    QStringList source_lines_;
    SetFileData file_data_;
};

#endif // SETFILESREADER_H
