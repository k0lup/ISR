#pragma once
#include "Services/Programs/programtypes.h"
#include <QString>

enum class ExecuteMode
{
    AUTO,
    STEP
};

struct ExecuteParam
{
    QString section_name;
    ChapterType active_chapter;
    QString label;
    ExecuteMode mode;

    void clear()
    {
        section_name.clear();
        active_chapter = ChapterType::STATE;
        label.clear();
        mode = ExecuteMode::STEP;
    }
};

struct AutoBlockIndexes
{
    int start_index = -1;
    int end_index = -1;

    void setIndexes(int first_index, int second_index) {
        start_index = std::min(first_index, second_index);
        end_index = std::max(first_index, second_index);
    }

    void clear() {
        start_index = -1;
        end_index = -1;
    }

    bool contains(int index) {
        if (start_index == -1 || end_index == -1) {
            return false;
        }
        if (start_index <= index && index <= end_index) {
            return true;
        } else {
            return false;
        }
    }
};

struct ExecutionSnapshot
{
    QString section_name;
    SectionStruct section;

    ChapterType active_chpater;
    QMap<ChapterType, int> active_dir_index;
    QMap<ChapterType, AutoBlockIndexes> auto_blocks;

    int exec_frame_id;
};
