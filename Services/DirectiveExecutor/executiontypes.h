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

struct ExecutionSnapshot
{
    QString section_name;
    SectionStruct section;

    ChapterType active_chpater;
    int active_dir_index;

    int exec_frame_id;
};
