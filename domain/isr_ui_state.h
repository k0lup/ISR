#pragma once

#include <QString>
#include <QStringList>

namespace isr {

enum class UiStage {
    StartupLoading,
    NoSection,
    SectionSelected,
    TitleReady,
    StructureReady,
    FatalError
};

struct ISRUiState {
    UiStage stage = UiStage::StartupLoading;
    QString windowTitle;
    QString activeSectionName;
    QStringList openedSections;

    bool loadingVisible = false;
    QString loadingMessage;
    bool loadingIndeterminate = false;
    int loadingProgress = 0;
    bool loadingProgressKnown = false;
};

} // namespace isr
