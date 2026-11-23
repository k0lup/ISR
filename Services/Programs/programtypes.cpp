#include "Services/Programs/programtypes.h"

QString directTypeToString(const DirectType direct_type)
{
    switch (direct_type) {
    case DirectType::COMMAND        : return "COMMAND";
    case DirectType::DOKLAD         : return "DOKLAD";
    case DirectType::INSTRUCTIONS   : return "INSTRUCTIONS";
    case DirectType::INVALID        : return "INVALID";
    case DirectType::MAIN_OPERATION : return "MAIN_OPERATION";
    case DirectType::START_SECTION  : return "START_SECTION";
    case DirectType::SUB_DIRECTIVE  : return "SUB_DIRECTIVE";
    case DirectType::VARIANT        : return "VARIANT";
    }
}

QString directStatusToString(const DirectStatus direct_status)
{
    switch (direct_status) {
    case DirectStatus::CURRENT      : return "CURRENT";
    case DirectStatus::FAILED       : return "FAILED";
    case DirectStatus::FINISHED     : return "FINISHED";
    case DirectStatus::LOAD         : return "LOAD";
    case DirectStatus::NOT_LOAD     : return "NOT_LOAD";
    case DirectStatus::RUNNING      : return "RUNNING";
    }
}

QString ChapterTypeToString(const ChapterType chapter_type)
{
    switch (chapter_type) {
    case ChapterType::NOT_STATE     : return "NOT_STATE";
    case ChapterType::PRI           : return "PRI";
    case ChapterType::STATE         : return "STATE";
    }
}
