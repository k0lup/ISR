#ifndef PROGRAMLINES_H
#define PROGRAMLINES_H
#include <QString>
#include <QVector>

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

    static bool isValid(const QVector<QString>& row) {
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

    static bool isValid(const QVector<QString>& row) {
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

    static bool isValid(const QVector<QString>& row) {
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

    bool operator==(const DirectiveLine& directive_line_for_equal) const
    {
        if ((num_col == directive_line_for_equal.num_col) &&
                (type_col == directive_line_for_equal.type_col) &&
                (command_col == directive_line_for_equal.command_col) &&
                (operation_col == directive_line_for_equal.operation_col)) {
            return true;
        }
        return false;
    }

    bool operator!=(const DirectiveLine& directive_line_for_equal) const {
        return !(*this == directive_line_for_equal);
    }
};
#endif // PROGRAMLINES_H
