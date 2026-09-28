#pragma once
#include <QMap>
#include "Services/Programs/programlines.h"

enum class DirectType {
    MAIN_OPERATION,
    VARIANT,
    START_SECTION,
    COMMAND,
    DOKLAD,
    INSTRUCTIONS,
    INVALID
};

enum class DirectStatus {
    NOT_LOAD,
    LOAD,
    CURRENT,
    RUNNING,
    FINISHED,
    FAILED
};

enum class ChapterType
{
    STATE,
    NOT_STATE,
    PRI
};

struct DirectiveStruct
{
    int number;
    DirectType type;
    DirectStatus status = DirectStatus::NOT_LOAD;

    QList<DirectiveLine> lines;

    bool operator==(const DirectiveStruct& directive_struct_for_equal) const
    {
        if ((number == directive_struct_for_equal.number) &&
                (type == directive_struct_for_equal.type) &&
                (lines == directive_struct_for_equal.lines)) {
            return true;
        }

        return false;
    }

    bool operator!=(const DirectiveStruct& directive_struct_for_equal) const
    {
        return !(*this == directive_struct_for_equal);
    }
};

struct ChapterStruct
{
    ChapterType type;
    QString name;
    QMap<QString, int> labels;

    QList<DirectiveStruct> directives;

    bool operator==(const ChapterStruct& chapter_struct_for_equal) const
    {
      if ((type == chapter_struct_for_equal.type) &&
              (name == chapter_struct_for_equal.name) &&
              (labels == chapter_struct_for_equal.labels) &&
              (directives == chapter_struct_for_equal.directives)) {
          return true;
      }
      return false;
    }

    bool operator!=(const ChapterStruct& chapter_struct_for_equal) const
    {
        return !(*this == chapter_struct_for_equal);
    }
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

    bool operator==(const PassportStruct& passport_struct_for_equal) const
    {
        if ((checksum == passport_struct_for_equal.checksum) &&
                (doc_type == passport_struct_for_equal.doc_type) &&
                (content == passport_struct_for_equal.content) &&
                (developer == passport_struct_for_equal.developer) &&
                (product_type == passport_struct_for_equal.product_type) &&
                (doc_index == passport_struct_for_equal.doc_index)) {
            return true;
        }
        return false;
    }

    bool operator!=(const PassportStruct& passport_struct_for_equal) const
    {
        return !(*this == passport_struct_for_equal);
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

    ChapterStruct& get(ChapterType type) {
        for (auto& chapter : chapters) {
            if (chapter.type == type) {
                return chapter;
            }
        }

        //ПРОДУМАТЬ ВОЗВРАТ ОШИБКИ
    }

    bool operator==(const SectionStruct& section_for_equal) const {
        if ((passport == section_for_equal.passport) &&
                (chapters == section_for_equal.chapters)) {
            return true;
        }
        return false;
    }

    bool operator!=(const SectionStruct& section_for_equal) const {
        return !(*this == section_for_equal);
    }
};
