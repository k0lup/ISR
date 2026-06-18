#ifndef DIIREADER_H
#define DIIREADER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>


enum class ColumnName {
    NUMBER = 0,
    TYPE = 1,
    COMMAND = 2,
    OPERATION = 3,
    COUNT_COLUMNS = 4
};

enum class LineType {
    PASSPORT,
    SECTION,
    START_COMMAND,
    CONTINUE_COMMAND,
    EMPTY,
    BAD_TYPE
};

enum class CommandType {
    COMMAND,
    REPORT,
    INSTRUCTIONS,
    OPTION,
    SECTION_START,
    MAIN_OPERATION,
    OPTION_NUMBER,
    LABEL,
    ADDITIONAL,
    PASSPORT
};

struct CommandLine {
    LineType line_type;
    CommandType command_type;
    QString command;
    QString operation;
    QString type;
};

struct Command {
    int number = -1;
    QList<CommandLine> command_lines;

    void clear() {
        number = -1;
        command_lines.clear();
    }
};

enum class ChapterType {
    STATE = 0,
    NON_STATE = 1,
    ACCIDENT = 2,
    INCORRECT = -1
};

struct Block {
    QString name;
    QList<int> num_dirs_for_block;
};

struct Chapter {
    ChapterType type = ChapterType::INCORRECT;
    QList<Command> commands;
    QList<Block> blocks;

    void clear() {
        commands.clear();
        blocks.clear();
        type = ChapterType::INCORRECT;
    }

    bool containsBlock(const QString& label) {
        for (const auto& block : blocks) {
            if (block.name == label) {
                return true;
            }
        }

        return false;
    }
};

struct Line
{
   QStringList line;
   LineType type = LineType::BAD_TYPE;

   void clear() {
       line.clear();
       type = LineType::BAD_TYPE;
   }
};

struct DiiFile {
    QList<Command> passport;
    QList<Chapter> chapters;
};

class DiiReader : public QObject
{
    Q_OBJECT
public:
    explicit DiiReader(QObject *parent = nullptr);

signals:
    void failed(const quint64 request_id, const QString& error_message);
    void fileReaded(const quint64 request_id, const DiiFile& data);
public slots:
    void onReadFileRequested(const quint64 request_id, const QString& file_path);
private:
    QStringList getAllDataOnFile(const QString& file_path, QString& error_message) const;
    Line parseLine(const QString& line, QString& error) const;
    QList<Line> readLines(const QStringList& lines, QString& error) const;
    DiiFile parseLines(const QList<Line>& lines, QStringList& error_messages) const;

    bool isPassportLine(const QStringList& line) const;
    bool isSectionLine(const QStringList& line) const;
};

#endif // DIIREADER_H
