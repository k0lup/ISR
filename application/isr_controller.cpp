#include "application/isr_controller.h"

#include "domain/title_section_data.h"
#include "services/sections_catalog_service.h"

namespace isr {

ISRController::ISRController(std::shared_ptr<const AppConfig> cfg, QObject* parent)
    : QObject(parent)
    , cfg_(std::move(cfg))
{
}

ISRController::~ISRController() = default;

void ISRController::start()
{
    session_.fatalError = false;
    session_.startupCompleted = false;
    session_.startupLoading = true;
    session_.startupMessage = QStringLiteral("Загрузка списка разделов...");
    session_.startupProgress = 0;
    session_.startupProgressKnown = false;
    publishState();

    ensureService();
    sectionsService_->start();
}

void ISRController::ensureService()
{
    if (sectionsService_ != nullptr) {
        return;
    }

    sectionsService_ = new SectionsCatalogService(cfg_, this);
    connect(sectionsService_, &SectionsCatalogService::finished,
            this, &ISRController::onStartupLoaded);
    connect(sectionsService_, &SectionsCatalogService::failed,
            this, &ISRController::onStartupFailed);
    connect(sectionsService_, &SectionsCatalogService::message,
            this, &ISRController::onStartupMessage);
    connect(sectionsService_, &SectionsCatalogService::progress,
            this, &ISRController::onStartupProgress);
}

void ISRController::onStartupLoaded(const QStringList& sections)
{
    session_.availableSections = sections;
    session_.startupCompleted = true;
    session_.startupLoading = false;
    session_.startupMessage.clear();
    session_.startupProgressKnown = false;
    publishState();
}

void ISRController::onStartupFailed(const QString& error)
{
    session_.fatalError = true;
    session_.startupLoading = false;
    session_.startupCompleted = false;
    publishState();
    emit showErrorRequested(error);
}

void ISRController::onStartupMessage(const QString& msg)
{
    session_.startupMessage = msg;
    publishState();
}

void ISRController::onStartupProgress(int progress)
{
    session_.startupProgress = progress;
    session_.startupProgressKnown = true;
    publishState();
}

void ISRController::onSectionChosen(const QString& sectionName)
{
    const QString trimmed = sectionName.trimmed();
    if (trimmed.isEmpty()) {
        emit showWarningRequested(QStringLiteral("Раздел не выбран"));
        return;
    }

    session_.hasCurrentSection = true;
    session_.currentSection = CurrentSectionState{};
    session_.currentSection.sectionName = trimmed;
    publishState();
}

void ISRController::onLoadSectionRequested(const QString& sectionName)
{
    onSectionChosen(sectionName);
}

void ISRController::onTitleConfirmed(const TitleSectionData& title)
{
    if (!session_.hasCurrentSection) {
        emit showWarningRequested(QStringLiteral("Сначала выберите раздел"));
        return;
    }

    if (!title.isValid()) {
        emit showWarningRequested(QStringLiteral("Заголовок раздела заполнен некорректно"));
        return;
    }

    session_.currentSection.titleAssigned = true;
    session_.currentSection.title = title;
    publishState();
}

void ISRController::onLoadStructureRequested()
{
    if (!session_.hasCurrentSection) {
        emit showWarningRequested(QStringLiteral("Сначала выберите раздел"));
        return;
    }

    if (!session_.currentSection.titleAssigned) {
        emit showWarningRequested(QStringLiteral("Сначала задайте заголовок раздела"));
        return;
    }

    // В этой точке потом подключишь реальный загрузчик .dii файла.
    session_.currentSection.structureLoaded = true;
    publishState();
}

void ISRController::onCloseSectionsRequested()
{
    session_.clearCurrentSection();
    publishState();
}

void ISRController::publishState()
{
    emit uiStateChanged(buildUiState());
    emit permissionsChanged(buildPermissions());
    emit sectionsListChanged(session_.availableSections);
}

ISRUiState ISRController::buildUiState() const
{
    ISRUiState state;

    if (session_.fatalError) {
        state.stage = UiStage::FatalError;
        state.windowTitle = QStringLiteral("ИСР - [ошибка]");
        return state;
    }

    if (session_.startupLoading || !session_.startupCompleted) {
        state.stage = UiStage::StartupLoading;
        state.windowTitle = QStringLiteral("ИСР - [загрузка]");
        state.loadingVisible = true;
        state.loadingMessage = session_.startupMessage.isEmpty()
                ? QStringLiteral("Загрузка списка разделов...")
                : session_.startupMessage;
        state.loadingIndeterminate = !session_.startupProgressKnown;
        state.loadingProgress = session_.startupProgress;
        state.loadingProgressKnown = session_.startupProgressKnown;
        return state;
    }

    if (!session_.hasCurrentSection) {
        state.stage = UiStage::NoSection;
        state.windowTitle = QStringLiteral("ИСР - [раздел не выбран]");
        return state;
    }

    state.activeSectionName = session_.currentSection.sectionName;
    state.openedSections << session_.currentSection.sectionName;

    if (!session_.currentSection.titleAssigned) {
        state.stage = UiStage::SectionSelected;
        state.windowTitle = QStringLiteral("ИСР - [%1]").arg(state.activeSectionName);
        return state;
    }

    if (!session_.currentSection.structureLoaded) {
        state.stage = UiStage::TitleReady;
        state.windowTitle = QStringLiteral("ИСР - [%1, файл структуры не загружен]")
                .arg(state.activeSectionName);
        return state;
    }

    state.stage = UiStage::StructureReady;
    state.windowTitle = QStringLiteral("ИСР - [%1]").arg(state.activeSectionName);
    return state;
}

ISRPermissions ISRController::buildPermissions() const
{
    ISRPermissions p;

    const bool hasSection = session_.hasCurrentSection;
    const bool titleReady = hasSection && session_.currentSection.titleAssigned;
    const bool structureReady = hasSection && session_.currentSection.structureLoaded;
    const bool startupReady = session_.startupCompleted && !session_.fatalError;

    p.canOpenSectionsList = startupReady;
    p.canEditTitle = hasSection && !titleReady;
    p.canOpenPfks = titleReady;
    p.canControlSpo = titleReady;
    p.canLoadStructure = titleReady && !structureReady;

    p.canDebugSection = !hasSection;
    p.canAddSections = !hasSection;
    p.canRebuildSections = !hasSection;
    p.canOpenStencil = !hasSection;

    p.canSelectRegularSection = structureReady;
    p.canSelectNshsSection = structureReady;
    p.canSelectPriSection = structureReady;

    p.canRunOperation = structureReady;
    p.canCancelVariant = structureReady;
    p.canOpenSp = titleReady;
    p.canGoToActiveSegments = structureReady;
    p.canImitatePrisAnswer = structureReady;
    p.canShowHeadDuration = titleReady;
    p.canEndSection = titleReady;
    p.canExitSections = titleReady;

    p.canMarkAuto = structureReady;
    p.canUnsetAuto = structureReady;
    p.canRunAutoBlock = structureReady;
    p.canStopAutoBlock = structureReady;

    return p;
}

} // namespace isr
