#include "diireader.h"
#include <QFile>
#include <QDebug>
#include "Services/Programs/programtypes.h"
#include "Services/Programs/programlines.h"

static int col(dii_struct::ColumnName name) {
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

    if (line.count() == 2 && line[0].isEmpty()) {
        res = true;
    }

    return res;
}

bool DiiReader::isPassportLine(const QStringList &line) const {
    bool res{false};
    if (line.isEmpty()) {
        return false;
    }
    if (line[0] == "П" && line.count() == 2) {
        res = true;
    }

    return res;
}

DiiReader::DiiReader(QObject *parent)
    : QObject{parent}
{

}

void DiiReader::onReadFileRequested(const quint64 request_id, const QString& file_path) {
    qDebug() << "I'm HERE";
    QString error_message;
    QStringList file_data_lines = getAllDataOnFile(file_path, error_message);

    if (!error_message.isEmpty()) {
        emit failed(request_id, error_message);
        return;
    }

    QList<dii_struct::Line> file_lines = readLines(file_data_lines, error_message);
    if (!error_message.isEmpty()) {
        emit failed(request_id, error_message);
        return;
    }

    QStringList error_messages;
    dii_struct::DiiFile dii_file = parseLines(file_lines, error_messages);
    if (!error_messages.isEmpty()) {
        for (const auto& message : error_messages) {
            error_message.append(message + "\n");
        }
        emit failed(request_id, error_message);
        return;
    }

    QString errors;
    SectionStruct section_struct = convertDii(dii_file, errors);
    if (!errors.isEmpty()) {
        emit failed(request_id, errors);
        return;
    }
    emit fileReaded(request_id, section_struct);
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

dii_struct::Line DiiReader::parseLine(const QString& line, QString& error) const {
    error.clear();
    dii_struct::Line result;
    QStringList line_split = line.split("|");

    for (auto& str : line_split) {
        str = str.trimmed();
    }

    if (!line_split.isEmpty() && line_split.last().isEmpty()) {
        line_split.removeLast();
    }

    if (!isPassportLine(line_split) && !isSectionLine(line_split) && line_split.size() != col(dii_struct::ColumnName::COUNT_COLUMNS)) {
        error = "НЕДОПУСТИМОЕ КОЛИЧЕСТВО СТОЛБЦОВ В СТРОКЕ";
        result.clear();
        return result;
    }

    if (line_split[col(dii_struct::ColumnName::NUMBER)].isEmpty()) {
        if (line_split.count() == 2) {
            result.type = dii_struct::LineType::CONTINUE_SECTION;
        } else {
            result.type = dii_struct::LineType::CONTINUE_COMMAND;
        }
    } else if (line_split[col(dii_struct::ColumnName::NUMBER)].at(0) == "#") {
        result.type =dii_struct:: LineType::SECTION;
    } else if (line_split[col(dii_struct::ColumnName::NUMBER)] == "П") {
        result.type = dii_struct::LineType::PASSPORT;
    } else if (isNumber(line_split[col(dii_struct::ColumnName::NUMBER)])) {
        result.type = dii_struct::LineType::START_COMMAND;
    } else {
        result.type = dii_struct::LineType::BAD_TYPE;
    }

    result.line = line_split;

    return result;
}

QList<dii_struct::Line> DiiReader::readLines(const QStringList &lines, QString &error) const {
    error.clear();
    QList<dii_struct::Line> result;
    if (lines.count() <= 6) {
        error = QString("ФАЙЛ НЕ СОДЕРЖИТ НИЧЕГО КРОМЕ ПАСПОРТА!");
        return result;
    }
    for (int num_line = 0; num_line < lines.count(); ++num_line) {
        if (lines[num_line].isEmpty()) {
            continue;
        }
        QString error_message;
        dii_struct::Line line = parseLine(lines[num_line], error_message);
        if (!error_message.isEmpty()) {
            error = QString("Ошибка чтения строки №%1: '%2'").arg(num_line + 1).arg(error_message);
            result.clear();
            return result;
        }

        if (line.type == dii_struct::LineType::BAD_TYPE) {
            error = QString("Некорректная строка №%1").arg(num_line + 1);
            result.clear();
            return result;
        }

        result.append(line);
    }

    return result;
}

dii_struct::DiiFile DiiReader::parseLines(const QList<dii_struct::Line>& lines, QStringList &error_messages) const {
    bool res{true};

    QList<dii_struct::Command> passport;
    QList<dii_struct::Chapter> chapters;

    dii_struct::Chapter chapter;
    dii_struct::Command command;

    bool has_start_command{false};
    bool has_start_section{false};
    bool has_active_block{false};

    for (int num_line = 0; num_line < lines.count(); ++num_line) {
        if (num_line < 6) {
            if (lines[num_line].type != dii_struct::LineType::PASSPORT) {
                error_messages.append(QString("Строка %1: ПЕРВЫЕ 6 СТРОК ДОЛЖНЫ БЫТЬ ПАСПОРТОМ").arg(num_line + 1));
                res = false;
                continue;
            }
            dii_struct::CommandLine command_line;
            command_line.line_type = dii_struct::LineType::PASSPORT;
            command_line.command_type = dii_struct::CommandType::PASSPORT;
            command_line.type = lines[num_line].line[0];
            command_line.command = lines[num_line].line[1];
            //command_line.operation = lines[num_line].line[2];

            command.command_lines.append(command_line);
            passport.append(command);

            command.clear();

            continue;
        }
        if (lines[num_line].type == dii_struct::LineType::PASSPORT) {
            error_messages.append(QString("Строка %1: СТРОКА ПАСПОРТА ВСТРЕЧЕНА ПОСЛЕ ПЕРВЫХ 6 СТРОК!").arg(num_line + 1));
            res = false;
            continue;
        }

        if (lines[num_line].type == dii_struct::LineType::SECTION) {
            if (has_start_section) {
                if (has_active_block) {
                    error_messages.append(QString("Строка %1: НЕ ЗАКРЫТ БЛОК НШС %2 ПЕРЕД КОНЦОМ РАЗДЕЛА").arg(num_line + 1).arg(chapter.blocks.last().name));
                    res = false;
                    continue;
                }
                if (has_start_command) {
                    if (chapter.commands.isEmpty()) {
                        command.number = 0;
                    } else {
                        command.number = chapter.commands.last().number + 1;
                    }
                    chapter.commands.append(command);
                    command.clear();
                    has_start_command = false;
                }
                if ((lines[num_line].line[col(dii_struct::ColumnName::NUMBER)] == "#1" && chapter.type != dii_struct::ChapterType::STATE) ||
                        (lines[num_line].line[col(dii_struct::ColumnName::NUMBER)] == "#2" && chapter.type != dii_struct::ChapterType::NON_STATE) ||
                        (lines[num_line].line[col(dii_struct::ColumnName::NUMBER)] == "#3" && chapter.type != dii_struct::ChapterType::ACCIDENT) ||
                        (lines[num_line].line[col(dii_struct::ColumnName::NUMBER)] == "#4" && chapter.type != dii_struct::ChapterType::PRILOSHENIE)) {
                    error_messages.append(QString("Строка %1: СТРОКА ЗАВЕРШЕНИЯ РАЗДЕЛА НЕ СООТВЕТСТВУЕТ ОТКРЫТОМУ РАЗДЕЛУ").arg(num_line + 1));
                    res = false;
                    chapter.type = dii_struct::ChapterType::INCORRECT;
                    chapters.append(chapter);
                    chapter.clear();
                    has_start_command = false;
                    has_start_section = false;
                    continue;
                }
                chapters.append(chapter);
                chapter.clear();

            } else {
                if (lines[num_line].line[col(dii_struct::ColumnName::NUMBER)] == "#1") {
                    chapter.type = dii_struct::ChapterType::STATE;
                } else if (lines[num_line].line[col(dii_struct::ColumnName::NUMBER)] == "#2") {
                    chapter.type = dii_struct::ChapterType::NON_STATE;
                } else if (lines[num_line].line[col(dii_struct::ColumnName::NUMBER)] == "#3") {
                    chapter.type = dii_struct::ChapterType::ACCIDENT;
                } else if (lines[num_line].line[col(dii_struct::ColumnName::NUMBER)] == "#4") {
                    chapter.type = dii_struct::ChapterType::PRILOSHENIE;
                } else {
                    chapter.type = dii_struct::ChapterType::INCORRECT;
                    error_messages.append(QString("Строка %1: НЕ УДАЛОСЬ РАСПОЗНАТЬ ТИП РАЗДЕЛА!").arg(num_line + 1));
                    res = false;
                    continue;
                }
            }
            has_start_command = false;
            has_start_section = !has_start_section;
            continue;
        }

        if (lines[num_line].type == dii_struct::LineType::START_COMMAND) {
            if (!has_start_section) {
                error_messages.append(QString("Строка %1: ВСТРЕЧЕНА КОМАНДА ДО НАЧАЛА РАЗДЕЛА").arg(num_line + 1));
                res = false;
                continue;
            }
            if (has_start_command) {
                if (chapter.commands.isEmpty()) {
                    command.number = 0;
                } else {
                    command.number = chapter.commands.last().number + 1;
                }
                chapter.commands.append(command);
                if (has_active_block) {
                    chapter.blocks.last().num_dirs_for_block.append(chapter.commands.count() - 1);
                }
                command.clear();
            }
            has_start_command = true;
            dii_struct::CommandLine command_line;
            command_line.line_type = dii_struct::LineType::START_COMMAND;
            if (lines[num_line].line[col(dii_struct::ColumnName::TYPE)] == "К") {
                command_line.command_type = dii_struct::CommandType::COMMAND;
            } else if (lines[num_line].line[col(dii_struct::ColumnName::TYPE)] == "Д") {
                command_line.command_type = dii_struct::CommandType::REPORT;
            } else if (lines[num_line].line[col(dii_struct::ColumnName::TYPE)] == "П") {
                command_line.command_type = dii_struct::CommandType::INSTRUCTIONS;
            } else if (lines[num_line].line[col(dii_struct::ColumnName::TYPE)] == "В") {
                command_line.command_type = dii_struct::CommandType::OPTION;
            } else if (lines[num_line].line[col(dii_struct::ColumnName::TYPE)] == "И") {
                command_line.command_type =dii_struct:: CommandType::SECTION_START;
            } else if (lines[num_line].line[col(dii_struct::ColumnName::TYPE)] == "О") {
                command_line.command_type = dii_struct::CommandType::MAIN_OPERATION;
            } else if (chapter.type == dii_struct::ChapterType::PRILOSHENIE) {
                command_line.command_type = dii_struct::CommandType::PRILOSHENIE;
            } else {
                error_messages.append(QString("Строка %1: НЕДОПУСТИМЫЙ ТИП СТРОКИ ДЛЯ НАЧАЛА КОМАНДЫ (1-ой СТРОКИ КОМАНДЫ)").arg(num_line + 1));
                res = false;
                continue;
            }

            command_line.command = lines[num_line].line[col(dii_struct::ColumnName::COMMAND)];
            command_line.operation = lines[num_line].line[col(dii_struct::ColumnName::OPERATION)];
            command_line.type = lines[num_line].line[col(dii_struct::ColumnName::TYPE)];

            command.command_lines.append(command_line);
            continue;
        }

        if (lines[num_line].type == dii_struct::LineType::CONTINUE_COMMAND) {
            if (!has_start_section) {
                error_messages.append(QString("Строка %1: ВСТРЕЧЕНА КОМАНДА ДО НАЧАЛА РАЗДЕЛА").arg(num_line + 1));
                res = false;
                continue;
            }
            if (lines[num_line].line[col(dii_struct::ColumnName::TYPE)] == "М") {
                if (chapter.type != dii_struct::ChapterType::NON_STATE) {
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
                QString block_name = lines[num_line].line[col(dii_struct::ColumnName::COMMAND)];
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

                    dii_struct::Block block;
                    block.name = block_name;
                    chapter.blocks.append(block);
                    has_active_block = true;
                }
            } else {
                if (!has_start_command) {
                    if (lines[num_line].line.join("").trimmed().isEmpty()) {
                        continue;
                    }
                    error_messages.append(QString("Строка %1: ДОПОЛНИТЕЛЬНАЯ СТРОКА, КОТОРАЯ НЕ ОТНОСИТСЯ НИ К ОДНОЙ КОМАНДЕ!").arg(num_line + 1));
                    res = false;
                    continue;
                }
                QString type_str = lines[num_line].line[col(dii_struct::ColumnName::TYPE)];
                if (!type_str.isEmpty() && (!isNumber(type_str) || type_str.toInt() < 0 || type_str.toInt() > 9)) {
                    error_messages.append(QString("Строка %1: НЕДОПУСТИМОЕ ЗНАЧЕНИЕ В ПОЛЕ ТИП").arg(num_line + 1));
                    res = false;
                    continue;
                }
                dii_struct::CommandLine command_line;
                command_line.command = lines[num_line].line[col(dii_struct::ColumnName::COMMAND)];
                command_line.operation = lines[num_line].line[col(dii_struct::ColumnName::OPERATION)];
                command_line.type = lines[num_line].line[col(dii_struct::ColumnName::TYPE)];
                command_line.line_type = dii_struct::LineType::CONTINUE_COMMAND;
                command_line.command_type = dii_struct::CommandType::ADDITIONAL;
                command.command_lines.append(command_line);
                continue;
            }
        }

        if (lines[num_line].type == dii_struct::LineType::BAD_TYPE) {
            error_messages.append(QString("Строка %1: НЕВЕРНЫЙ ТИП СТРОКИ").arg(num_line + 1));
            res = false;
            continue;
        }
    }
    if (chapters.count() != 4) {
        error_messages.append(QString("ДОЛЖНО БЫТЬ ОПИСАНО 4 РАЗДЕЛА!"));
        res = false;
    } else {
        if ((chapters[0].type != dii_struct::ChapterType::STATE && chapters[1].type != dii_struct::ChapterType::STATE && chapters[2].type != dii_struct::ChapterType::STATE && chapters[3].type != dii_struct::ChapterType::STATE) ||
            (chapters[0].type != dii_struct::ChapterType::NON_STATE && chapters[1].type != dii_struct::ChapterType::NON_STATE && chapters[2].type != dii_struct::ChapterType::NON_STATE && chapters[3].type != dii_struct::ChapterType::NON_STATE) ||
            (chapters[0].type != dii_struct::ChapterType::ACCIDENT && chapters[1].type != dii_struct::ChapterType::ACCIDENT && chapters[2].type != dii_struct::ChapterType::ACCIDENT && chapters[3].type != dii_struct::ChapterType::ACCIDENT) ||
            (chapters[0].type != dii_struct::ChapterType::PRILOSHENIE && chapters[1].type != dii_struct::ChapterType::PRILOSHENIE && chapters[2].type != dii_struct::ChapterType::PRILOSHENIE && chapters[3].type != dii_struct::ChapterType::PRILOSHENIE)) {
            error_messages.append(QString("ДОЖНО БЫТЬ ОПИСАНО 4 РАЗДЕЛА - ШТАТ, НШС, АВАР, ПРИЛОЖЕНИЕ!"));
            res = false;
        }
    }

    dii_struct::DiiFile dii_file;
    if (!res) {
        return dii_file;
    }

    if (chapters[0].type == dii_struct::ChapterType::STATE) {
        dii_file.chapters.append(chapters[0]);
    } else if (chapters[1].type == dii_struct::ChapterType::STATE) {
        dii_file.chapters.append(chapters[1]);
    } else if (chapters[2].type == dii_struct::ChapterType::STATE) {
        dii_file.chapters.append(chapters[2]);
    } else if (chapters[3].type == dii_struct::ChapterType::STATE) {
        dii_file.chapters.append(chapters[3]);
    }

    if (chapters[0].type == dii_struct::ChapterType::NON_STATE) {
        dii_file.chapters.append(chapters[0]);
    } else if (chapters[1].type == dii_struct::ChapterType::NON_STATE) {
        dii_file.chapters.append(chapters[1]);
    } else if (chapters[2].type == dii_struct::ChapterType::NON_STATE) {
        dii_file.chapters.append(chapters[2]);
    } else if (chapters[3].type == dii_struct::ChapterType::NON_STATE) {
        dii_file.chapters.append(chapters[3]);
    }

    if (chapters[0].type == dii_struct::ChapterType::ACCIDENT) {
        dii_file.chapters.append(chapters[0]);
    } else if (chapters[1].type == dii_struct::ChapterType::ACCIDENT) {
        dii_file.chapters.append(chapters[1]);
    } else if (chapters[2].type == dii_struct::ChapterType::ACCIDENT) {
        dii_file.chapters.append(chapters[2]);
    } else if (chapters[3].type == dii_struct::ChapterType::ACCIDENT) {
        dii_file.chapters.append(chapters[3]);
    }

    if (chapters[0].type == dii_struct::ChapterType::PRILOSHENIE) {
        dii_file.chapters.append(chapters[0]);
    } else if (chapters[1].type == dii_struct::ChapterType::PRILOSHENIE) {
        dii_file.chapters.append(chapters[1]);
    } else if (chapters[2].type == dii_struct::ChapterType::PRILOSHENIE) {
        dii_file.chapters.append(chapters[2]);
    } else if (chapters[3].type == dii_struct::ChapterType::PRILOSHENIE) {
        dii_file.chapters.append(chapters[3]);
    }

    dii_file.passport = passport;

    return dii_file;
}

SectionStruct DiiReader::convertDii(const dii_struct::DiiFile& dii_file, QString& errors)
{
    errors.clear();

    SectionStruct section_struct;
    QString current_errors;
    section_struct.passport = getPassport(dii_file.passport, current_errors);
    if (!current_errors.isEmpty()) {
        errors.append(current_errors);
        current_errors.clear();
    }

    for (const auto& chapter : dii_file.chapters) {
        section_struct.chapters.append(getChapter(chapter, current_errors));
        if (!current_errors.isEmpty()) {
            errors.append(current_errors);
            current_errors.clear();
        }
    }

    if (!errors.isEmpty()) {
        section_struct.clear();
    }

    return section_struct;
}

PassportStruct DiiReader::getPassport(const QList<dii_struct::Command> passport, QString& errors)
{
    errors.clear();
    PassportStruct result;

    for (const auto& command : passport) {
        for (const auto& line : command.command_lines) {
            if (line.line_type != dii_struct::LineType::PASSPORT) {
                errors.append("В паспорте обнаружена строка не являющаяся паспортом\n");
                continue;
            }
            QStringList passport_line = line.type.split("=");
            if (passport_line.count() != 2) {
                errors.append(QString("В паспорте обнаружена строка, которую не удалось разделить по символу '=': %1\n").arg(line.type));
                continue;
            }
            if (passport_line[0] == "КОНТРОЛЬНАЯ СУММА") {
                if (!result.checksum.isEmpty()) {
                    errors.append(QString("В паспорте повторно встречена контрольная сумма!\n"));
                    continue;
                }
                result.checksum = passport_line[1];
            } else if (passport_line[0] == "ТИП ДОКУМЕНТА") {
                if (!result.doc_type.isEmpty()) {
                    errors.append(QString("В паспорте повторно встречен тип документа!\n"));
                    continue;
                }
                result.doc_type = passport_line[1];
            } else if (passport_line[0] == "СОДЕРЖАНИЕ") {
                if (!result.content.isEmpty()) {
                    errors.append(QString("В паспорте повторно встречено содержание!\n"));
                    continue;
                }
                result.content = passport_line[1];
            } else if (passport_line[0] == "РАЗРАБОТЧИК") {
                if (!result.developer.isEmpty()) {
                    errors.append(QString("В паспорте повторно встречен разработчик!\n"));
                    continue;
                }
                result.developer = passport_line[1];
            } else if (passport_line[0] == "ТИП ИЗДЕЛИЯ") {
                if (!result.product_type.isEmpty()) {
                    errors.append(QString("В паспорте повторно встречен тип изделия!\n"));
                    continue;
                }
                result.product_type = passport_line[1];
            } else if (passport_line[0] == "ИНДЕКС ДОКУМЕНТА") {
                if (!result.doc_index.isEmpty()) {
                    errors.append(QString("В паспорте повторно встречен индекс документа!\n"));
                    continue;
                }
                result.doc_index = passport_line[1];
            } else {
                errors.append(QString("Неопознанное поле в паспорте: %1\n").arg(line.type));
                continue;
            }
        }
    }

    if (result.checksum.isEmpty()) {
        errors.append("Не обнаружена контрольная сумма\n");
    }

    if (result.content.isEmpty()) {
        errors.append("Не обнаружено содержание\n");
    }

    if (result.developer.isEmpty()) {
        errors.append("Не обнаружен разработчик\n");
    }

    if (result.doc_index.isEmpty()) {
        errors.append("Не обнаружен индекс документа\n");
    }

    if (result.doc_type.isEmpty()) {
        errors.append("Не обнаружен тип документа\n");
    }

    if (result.product_type.isEmpty()) {
        errors.append("Не обнаружена тип изделия\n");
    }

    if (!errors.isEmpty()) {
        result.clear();
    }

    return result;
}

ChapterStruct DiiReader::getChapter(const dii_struct::Chapter chapter, QString& errors)
{
    errors.clear();

    ChapterStruct result;

    switch (chapter.type) {
    case dii_struct::ChapterType::STATE : result.type = ChapterType::STATE; break;
    case dii_struct::ChapterType::NON_STATE : result.type = ChapterType::NOT_STATE; break;
    case dii_struct::ChapterType::ACCIDENT : result.type = ChapterType::PRI; break;
    default: errors.append("Обнаружен неопознанный раздел!"); return result;
    }

    for (int i = 0; i < chapter.commands.count(); ++i) {
        const dii_struct::Command command = chapter.commands[i];
        DirectiveStruct directive;
        directive.status = DirectStatus::NOT_LOAD;
        directive.number = command.number;
        switch (command.command_lines[0].command_type) {
        case dii_struct::CommandType::COMMAND : directive.type = DirectType::COMMAND;
        case dii_struct::CommandType::INSTRUCTIONS : directive.type = DirectType::INSTRUCTIONS;
        case dii_struct::CommandType::MAIN_OPERATION : directive.type = DirectType::MAIN_OPERATION;
        case dii_struct::CommandType::OPTION : directive.type = DirectType::VARIANT;
        case dii_struct::CommandType::REPORT : directive.type = DirectType::DOKLAD;
        case dii_struct::CommandType::SECTION_START : directive.type = DirectType::START_SECTION;
        default: errors.append(QString("Обнаружена неопознонная директива %1\n").arg(i + 1)); continue;
        }

        for (int num_line = 0; num_line < command.command_lines.count(); ++num_line) {
            dii_struct::CommandLine command_line = command.command_lines[num_line];
            LineStruct line;
            line.type = LineType::DIRECTIVE;
            if (num_line == 0) {
                line.cols_data.append(QString::number(directive.number));
            } else {
                line.cols_data.append("");
            }

            line.cols_data.append(command_line.type);
            line.cols_data.append(command_line.command);
            line.cols_data.append(command_line.operation);

            if (!DirectiveLine::isValid(line.cols_data)) {
                errors.append(QString("Ошибка в количестве столбцов в строке №%1 директивы №%2").arg(num_line).arg(i));
                continue;
            }
            DirectiveLine directive_line(line);
            directive.lines.append(directive_line);
        }

        result.directives.append(directive);
    }

    if (chapter.type == dii_struct::ChapterType::NON_STATE) {
        for (const auto& block : chapter.blocks) {
            NotStateBlock not_state_block;
            not_state_block.name = block.name;
            not_state_block.num_dirs_for_block = block.num_dirs_for_block;
            result.blocks.append(not_state_block);
        }
    }

    return result;
}
