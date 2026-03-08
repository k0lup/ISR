#pragma once

namespace isr {

struct ISRPermissions {
    bool canOpenSectionsList = false;
    bool canEditTitle = false;
    bool canOpenPfks = false;
    bool canControlSpo = false;
    bool canLoadStructure = false;
    bool canDebugSection = false;
    bool canAddSections = false;
    bool canRebuildSections = false;
    bool canOpenStencil = false;

    bool canSelectRegularSection = false;
    bool canSelectNshsSection = false;
    bool canSelectPriSection = false;

    bool canRunOperation = false;
    bool canCancelVariant = false;
    bool canOpenSp = false;
    bool canGoToActiveSegments = false;
    bool canImitatePrisAnswer = false;
    bool canShowHeadDuration = false;
    bool canEndSection = false;
    bool canExitSections = false;

    bool canMarkAuto = false;
    bool canUnsetAuto = false;
    bool canRunAutoBlock = false;
    bool canStopAutoBlock = false;
};

} // namespace isr
