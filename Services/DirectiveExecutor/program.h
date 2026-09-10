#ifndef PROGRAM_H
#define PROGRAM_H

#include <QString>
#include <QVector>
#include <QMap>

using RequestId = quint64;


enum class LineType
{
    PASSPORT,
    SECTION,
    DIRECTIVE,
    UNKOWN
};

struct LineStruct
{
    LineType type;

    QVector<QString> cols_data;
};

struct PassportLine
{
    QString type_col;
    QString key_col;
    QString val_col;

    void clear() {
        type_col.clear();
        key_col.clear();
        val_col.clear();
    }

    bool isValid(const QVector<QString>& row) {
        if (row.count() != 3 || row[0] != "П") {
            return false;
        }
        return true;
    }

    PassportLine(LineStruct input_struct) {
        if (!isValid(input_struct.cols_data)) {
            clear();
        } else {
            type_col = input_struct.cols_data[0];
            key_col = input_struct.cols_data[1];
            val_col = input_struct.cols_data[2];
        }
    }
};

struct SectionLine
{
    QString num_col;
    QString section_name_col;

    void clear() {
        num_col.clear();
        section_name_col.clear();
    }

    bool isValid(const QVector<QString>& row) {
        if (row.count() != 2) {
            return false;
        }
        if (!row[0].isEmpty()) {
            if (row[0].count() < 2 || row[0].startsWith("#")) {
                return false;
            }
            bool ok{false};
            int num = row[0].mid(1).toInt(&ok);
            if (!ok || num <= 0) {
                return false;
            }
        } else {
            if (!row[1].isEmpty()) {
                return false;
            }
        }
        return true;
    }

    SectionLine(LineStruct input_struct) {
        if (!isValid(input_struct.cols_data)) {
            clear();
        } else {
            num_col = input_struct.cols_data[0];
            section_name_col = input_struct.cols_data[1];
        }
    }
};

struct DirectiveLine
{
    QString num_col;
    QString type_col;
    QString command_col;
    QString operation_col;

    void clear() {
        num_col.clear();
        type_col.clear();
        command_col.clear();
        operation_col.clear();
    }

    bool isValid(const QVector<QString> row) {
        if (row.count() != 4) {
            return false;
        }

        return true;
    }

    DirectiveLine(LineStruct input_struct) {
        if (!isValid(input_struct.cols_data)) {
            clear();
        } else {
            num_col = input_struct.cols_data[0];
            type_col = input_struct.cols_data[1];
            command_col = input_struct.cols_data[2];
            operation_col = input_struct.cols_data[3];
        }
    }
};

enum class DirectType
{

};

struct DirectiveStruct
{
    int number;
    DirectType type;

    QList<DirectiveLine> lines;
};

enum class ChapterType
{
    STATE,
    NOT_STATE
};

struct ChapterStruct
{
    ChapterType type;
    QString name;
    QMap<QString, int> labels;

    QList<DirectiveStruct> directives;
};

struct PassportStruct
{
    QString checksum;
    QString doc_type;
    QString content;
    QString developer;
    QString product_type;
    QString doc_index;

    void clear() {
        checksum.clear();
        doc_type.clear();
        content.clear();
        developer.clear();
        product_type.clear();
        doc_index.clear();
    }
};

struct SectionStruct
{
    PassportStruct passport;

    QList<ChapterStruct> chapters;

    void clear() {
        passport.clear();
        chapters.clear();
    }

    ChapterStruct get(ChapterType type) {
        for (const auto& chapter : chapters) {
            if (chapter.type == type) {
                return chapter;
            }
        }

        return ChapterStruct();
    }
};


struct SectionForGUI
{
    SectionStruct section;

    ChapterType active_chpater;
    int active_dir_index;
};

#endif // PROGRAM_H
