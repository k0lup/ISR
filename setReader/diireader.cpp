#include "diireader.h"
#include <QFile>

static int col(ColumnName name) {
    return static_cast<int>(name);
}

static bool isNumber(const QString& str) {
    bool res{false};
    str.toInt(&res);
    return res;
}

bool DiiReader::isSectionLine(const QStringList& line) const {
    bool res{false};
    if (line.count() == 2 && line[0].count() == 2 && line[0][0] == "#" && isNumber(line[0][1])) {
        res = true;
    }

    return res;
}

bool DiiReader::isPassportLine(const QStringList &line) const {
    bool res{false};
    if (line.isEmpty()) {
        return false;
    }
    if (line[0] == "П" && line.size() == 3) {
        res = true;
    }

    return res;
}

DiiReader::DiiReader(QObject *parent)
    : QObject{parent}
{

}

void DiiReader::onReadFileRequested(const quint64 request_id, const QString& file_path) {
    QString error_message;
    QStringList file_data_lines = getAllDataOnFile(file_path, error_message);

    if (!error_message.isEmpty()) {
        emit failed(request_id, error_message);
        return;
    }

    QList<Line> file_lines = readLines(file_data_lines, error_message);
    if (!error_message.isEmpty()) {
        emit failed(request_id, error_message);
        return;
    }

    QStringList error_messages;
    DiiFile dii_file = parseLines(file_lines, error_messages);
    if (!error_messages.isEmpty()) {
        for (const auto& message : error_messages) {
            error_message.append(message + "\n");
        }
        emit failed(request_id, error_message);
        return;
    }

    emit fileReaded(request_id, dii_file);
}

QStringList DiiReader::getAllDataOnFile(const QString &file_path, QString& error_message) const {
    error_message.clear();
    QFile file(file_path);
    QStringList result;

    if (!file.open(QIODevice::Text | QIODevice::ReadOnly)) {
        error_message = "Не удалось открыть файл: '" + file.errorString() + "'";
        return result;
    }

    while (!file.atEnd()) {
        QString line = file.readLine();
        line = line.trimmed();
        result.append(line);
    }

    file.close();
    return result;
}

Line DiiReader::parseLine(const QString& line, QString& error) const {
    error.clear();
    Line result;
    QStringList line_split = line.split("|");

    for (auto& str : line_split) {
        str = str.trimmed();
    }

    if (!line_split.isEmpty() && line_split.last().isEmpty()) {
        line_split.removeLast();
    }

    if (!isPassportLine(line_split) && !isSectionLine(line_split) && line_split.size() != col(ColumnName::COUNT_COLUMNS)) {
        error = "НЕДОПУСТИМОЕ КОЛИЧЕСТВО СТОЛБЦОВ В СТРОКЕ";
        result.clear();
        return result;
    }

    if (line_split[col(ColumnName::NUMBER)].isEmpty()) {
        result.type = LineType::CONTINUE_COMMAND;
    } else if (line_split[col(ColumnName::NUMBER)].at(0) == "#") {
        result.type = LineType::SECTION;
    } else if (line_split[col(ColumnName::NUMBER)] == "П") {
        result.type = LineType::PASSPORT;
    } else if (isNumber(line_split[col(ColumnName::NUMBER)])) {
        result.type = LineType::START_COMMAND;
    } else {
        result.type = LineType::BAD_TYPE;
    }

    result.line = line_split;

    return result;
}

QList<Line> DiiReader::readLines(const QStringList &lines, QString &error) const {
    error.clear();
    QList<Line> result;
    if (lines.count() <= 6) {
        error = QString("ФАЙЛ НЕ СОДЕРЖИТ НИЧЕГО КРОМЕ ПАСПОРТА!");
        return result;
    }
    for (int num_line = 0; num_line < lines.count(); ++num_line) {
        if (lines[num_line].isEmpty()) {
            continue;
        }
        QString error_message;
        Line line = parseLine(lines[num_line], error_message);
        if (!error_message.isEmpty()) {
            error = QString("Ошибка чтения строки №%1: '%2'").arg(num_line + 1).arg(error_message);
            result.clear();
            return result;
        }

        if (line.type == LineType::BAD_TYPE) {
            error = QString("Некорректная строка №%1").arg(num_line + 1);
            result.clear();
            return result;
        }

        result.append(line);
    }

    return result;
}

DiiFile DiiReader::parseLines(const QList<Line>& lines, QStringList &error_messages) const {
    bool res{true};

    QList<Command> passport;
    QList<Chapter> chapters;

    Chapter chapter;
    Command command;

    bool has_start_command{false};
    bool has_start_section{false};
    bool has_active_block{false};

    for (int num_line = 0; num_line < lines.count(); ++num_line) {
        if (num_line < 6) {
            if (lines[num_line].type != LineType::PASSPORT) {
                error_messages.append(QString("Строка %1: ПЕРВЫЕ 6 СТРОК ДОЛЖНЫ БЫТЬ ПАСПОРТОМ").arg(num_line + 1));
                res = false;
                continue;
            }
            CommandLine command_line;
            command_line.line_type = LineType::PASSPORT;
            command_line.command_type = CommandType::PASSPORT;
            command_line.type = lines[num_line].line[0];
            command_line.command = lines[num_line].line[1];
            command_line.operation = lines[num_line].line[2];

            command.command_lines.append(command_line);
            passport.append(command);

            command.clear();

            continue;
        }
        if (lines[num_line].type == LineType::PASSPORT) {
            error_messages.append(QString("Строка %1: СТРОКА ПАСПОРТА ВСТРЕЧЕНА ПОСЛЕ ПЕРВЫХ 6 СТРОК!").arg(num_line + 1));
            res = false;
            continue;
        }

        if (lines[num_line].type == LineType::SECTION) {
            if (has_start_section) {
                if (has_active_block) {
                    error_messages.append(QString("Строка %1: НЕ ЗАКРЫТ БЛОК НШС %2 ПЕРЕД КОНЦОМ РАЗДЕЛА").arg(num_line + 1).arg(chapter.blocks.last().name));
                    res = false;
                    continue;
                }
                if (has_start_command) {
                    chapter.commands.append(command);
                    command.clear();
                    has_start_command = false;
                }
                if ((lines[num_line].line[col(ColumnName::NUMBER)] == "#1" && chapter.type != ChapterType::STATE) ||
                        (lines[num_line].line[col(ColumnName::NUMBER)] == "#2" && chapter.type != ChapterType::NON_STATE) ||
                        (lines[num_line].line[col(ColumnName::NUMBER)] == "#3" && chapter.type != ChapterType::ACCIDENT)) {
                    error_messages.append(QString("Строка %1: СТРОКА ЗАВЕРШЕНИЯ РАЗДЕЛА НЕ СООТВЕТСТВУЕТ ОТКРЫТОМУ РАЗДЕЛУ").arg(num_line + 1));
                    res = false;
                    chapter.type = ChapterType::INCORRECT;
                    chapters.append(chapter);
                    chapter.clear();
                    has_start_command = false;
                    has_start_section = false;
                    continue;
                }
                chapters.append(chapter);
                chapter.clear();

            } else {
                if (lines[num_line].line[col(ColumnName::NUMBER)] == "#1") {
                    chapter.type = ChapterType::STATE;
                } else if (lines[num_line].line[col(ColumnName::NUMBER)] == "#2") {
                    chapter.type = ChapterType::NON_STATE;
                } else if (lines[num_line].line[col(ColumnName::NUMBER)] == "#3") {
                    chapter.type = ChapterType::ACCIDENT;
                } else {
                    chapter.type = ChapterType::INCORRECT;
                    error_messages.append(QString("Строка %1: НЕ УДАЛОСЬ РАСПОЗНАТЬ ТИП РАЗДЕЛА!").arg(num_line + 1));
                    res = false;
                    continue;
                }
            }
            has_start_command = false;
            has_start_section = !has_start_section;
            continue;
        }

        if (lines[num_line].type == LineType::START_COMMAND) {
            if (!has_start_section) {
                error_messages.append(QString("Строка %1: ВСТРЕЧЕНА КОМАНДА ДО НАЧАЛА РАЗДЕЛА").arg(num_line + 1));
                res = false;
                continue;
            }
            if (has_start_command) {
                chapter.commands.append(command);
                if (has_active_block) {
                    chapter.blocks.last().num_dirs_for_block.append(chapter.commands.count() - 1);
                }
                command.clear();
            }
            has_start_command = true;
            CommandLine command_line;
            command_line.line_type = LineType::START_COMMAND;
            if (lines[num_line].line[col(ColumnName::TYPE)] == "К") {
                command_line.command_type = CommandType::COMMAND;
            } else if (lines[num_line].line[col(ColumnName::TYPE)] == "Д") {
                command_line.command_type = CommandType::REPORT;
            } else if (lines[num_line].line[col(ColumnName::TYPE)] == "П") {
                command_line.command_type = CommandType::INSTRUCTIONS;
            } else if (lines[num_line].line[col(ColumnName::TYPE)] == "В") {
                command_line.command_type = CommandType::OPTION;
            } else if (lines[num_line].line[col(ColumnName::TYPE)] == "И") {
                command_line.command_type = CommandType::SECTION_START;
            } else if (lines[num_line].line[col(ColumnName::TYPE)] == "О") {
                command_line.command_type = CommandType::MAIN_OPERATION;
            } else {
                error_messages.append(QString("Строка %1: НЕДОПУСТИМЫЙ ТИП СТРОКИ ДЛЯ НАЧАЛА КОМАНДЫ (1-ой СТРОКИ КОМАНДЫ)").arg(num_line + 1));
                res = false;
                continue;
            }

            command_line.command = lines[num_line].line[col(ColumnName::COMMAND)];
            command_line.operation = lines[num_line].line[col(ColumnName::OPERATION)];

            command.command_lines.append(command_line);
            continue;
        }

        if (lines[num_line].type == LineType::CONTINUE_COMMAND) {
            if (!has_start_section) {
                error_messages.append(QString("Строка %1: ВСТРЕЧЕНА КОМАНДА ДО НАЧАЛА РАЗДЕЛА").arg(num_line + 1));
                res = false;
                continue;
            }
            if (lines[num_line].line[col(ColumnName::TYPE)] == "М") {
                if (chapter.type != ChapterType::NON_STATE) {
                    error_messages.append(QString("Строка %1: НЕДОПУСТИМО ИСПОЛЬЗОВАТЬ БЛОК (МЕТКУ НШС) В РАЗДЕЛАХ КРОМЕ НШС").arg(num_line + 1));
                    res = false;
                    continue;
                }
                if (has_start_command) {
                    chapter.commands.append(command);
                    if (has_active_block) {
                        chapter.blocks.last().num_dirs_for_block.append(chapter.commands.count() - 1);
                    }
                    command.clear();
                    has_start_command = false;
                }
                QString block_name = lines[num_line].line[col(ColumnName::COMMAND)];
                if (block_name.startsWith("К_")) {
                    block_name = block_name.mid(2);
                    if (block_name.isEmpty()) {
                        error_messages.append(QString("Строка %1: ПУСТОЕ ИМЯ БЛОКА НШС").arg(num_line + 1));
                        res = false;
                        continue;
                    }
                    if (!has_active_block) {
                        error_messages.append(QString("Строка %1: ПОПЫТКА ЗАКРЫТЬ БЛОК %2, ХОТЯ ОН НЕ БЫЛ НАЧАТ").arg(num_line + 1).arg(block_name));
                        res = false;
                        continue;
                    }
                    if (chapter.blocks.last().name != block_name) {
                        error_messages.append(QString("Строка %1: ПОПЫТКА ЗАКРЫТЬ БЛОК %2, ХОТЯ ПОСЛЕДНИЙ БЛОК - %3").arg(num_line + 1).arg(block_name).arg(chapter.blocks.last().name));
                        res = false;
                        continue;
                    }
                    has_active_block = false;
                } else {
                    if (block_name.isEmpty()) {
                        error_messages.append(QString("Строка %1: ПУСТОЕ ИМЯ БЛОКА НШС").arg(num_line + 1));
                        res = false;
                        continue;
                    }
                    if (has_active_block) {
                        error_messages.append(QString("Строка %1: ПОПЫТКА НАЧАТЬ НОВЫЙ БЛОК %2 ВНУТРИ УЖЕ НАЧАТОГО БЛОКА - %3").arg(num_line + 1).arg(block_name).arg(chapter.blocks.last().name));
                        res = false;
                        continue;
                    }
                    if (chapter.containsBlock(block_name)) {
                        error_messages.append(QString("Строка %1: ПОВТОРНО ИСПОЛЬЗОВАН БЛОК %2").arg(num_line + 1).arg(block_name));
                        res = false;
                        continue;
                    }

                    Block block;
                    block.name = block_name;
                    chapter.blocks.append(block);
                    has_active_block = true;
                }
            } else {
                if (!has_start_command) {
                    error_messages.append(QString("Строка %1: ДОПОЛНИТЕЛЬНАЯ СТРОКА, КОТОРАЯ НЕ ОТНОСИТСЯ НИ К ОДНОЙ КОМАНДЕ!").arg(num_line + 1));
                    res = false;
                    continue;
                }
                QString type_str = lines[num_line].line[col(ColumnName::TYPE)];
                if (!type_str.isEmpty() && (!isNumber(type_str) || type_str.toInt() < 0 || type_str.toInt() > 9)) {
                    error_messages.append(QString("Строка %1: НЕДОПУСТИМОЕ ЗНАЧЕНИЕ В ПОЛЕ ТИП").arg(num_line + 1));
                    res = false;
                    continue;
                }
                CommandLine command_line;
                command_line.command = lines[num_line].line[col(ColumnName::COMMAND)];
                command_line.operation = lines[num_line].line[col(ColumnName::OPERATION)];
                command_line.line_type = LineType::CONTINUE_COMMAND;
                command_line.command_type = CommandType::ADDITIONAL;
                command.command_lines.append(command_line);
                continue;
            }
        }

        if (lines[num_line].type == LineType::BAD_TYPE) {
            error_messages.append(QString("Строка %1: НЕВЕРНЫЙ ТИП СТРОКИ").arg(num_line + 1));
            res = false;
            continue;
        }
    }
    if (chapters.count() != 3) {
        error_messages.append(QString("ДОЛЖНО БЫТЬ ОПИСАНО 3 РАЗДЕЛА!"));
        res = false;
    } else {
        if ((chapters[0].type != ChapterType::STATE && chapters[1].type != ChapterType::STATE && chapters[2].type != ChapterType::STATE) ||
                (chapters[0].type != ChapterType::NON_STATE && chapters[1].type != ChapterType::NON_STATE && chapters[2].type != ChapterType::NON_STATE) ||
                (chapters[0].type != ChapterType::ACCIDENT && chapters[1].type != ChapterType::ACCIDENT && chapters[2].type != ChapterType::ACCIDENT)) {
            error_messages.append(QString("ДОЖНО БЫТЬ ОПИСАНО 3 РАЗДЕЛА - ШТАТ, НШС, АВАР!"));
            res = false;
        }
    }

    DiiFile dii_file;
    if (!res) {
        return dii_file;
    }

    if (chapters[0].type == ChapterType::STATE) {
        dii_file.chapters.append(chapters[0]);
    } else if (chapters[1].type == ChapterType::STATE) {
        dii_file.chapters.append(chapters[1]);
    } else if (chapters[2].type == ChapterType::STATE) {
        dii_file.chapters.append(chapters[2]);
    }

    if (chapters[0].type == ChapterType::NON_STATE) {
        dii_file.chapters.append(chapters[0]);
    } else if (chapters[1].type == ChapterType::NON_STATE) {
        dii_file.chapters.append(chapters[1]);
    } else if (chapters[2].type == ChapterType::NON_STATE) {
        dii_file.chapters.append(chapters[2]);
    }

    if (chapters[0].type == ChapterType::ACCIDENT) {
        dii_file.chapters.append(chapters[0]);
    } else if (chapters[1].type == ChapterType::ACCIDENT) {
        dii_file.chapters.append(chapters[1]);
    } else if (chapters[2].type == ChapterType::ACCIDENT) {
        dii_file.chapters.append(chapters[2]);
    }

    dii_file.passport = passport;

    return dii_file;
}
