#pragma once

#include <QString>
#include <QStringList>
#include "domain/title_section_data.h"

namespace isr {

struct CurrentSectionState {
    QString sectionName;
    bool titleAssigned = false;
    bool structureLoaded = false;
    TitleSectionData title;
};

struct ISRSessionState {
    QStringList availableSections;

    bool startupCompleted = false;
    bool fatalError = false;

    bool hasCurrentSection = false;
    CurrentSectionState currentSection;

    bool startupLoading = false;
    QString startupMessage;
    int startupProgress = 0;
    bool startupProgressKnown = false;

    void clearCurrentSection()
    {
        hasCurrentSection = false;
        currentSection = CurrentSectionState{};
    }
};

} // namespace isr
