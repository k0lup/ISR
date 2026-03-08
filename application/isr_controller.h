#pragma once

#include <QObject>
#include <memory>

#include "config/app_config.h"
#include "domain/isr_permissions.h"
#include "domain/isr_session_state.h"
#include "domain/isr_ui_state.h"

namespace isr {

class SectionsCatalogService;
class TitleSectionData;

class ISRController : public QObject
{
    Q_OBJECT
public:
    explicit ISRController(std::shared_ptr<const AppConfig> cfg, QObject* parent = nullptr);
    ~ISRController() override;

    void start();

signals:
    void uiStateChanged(const isr::ISRUiState& state);
    void permissionsChanged(const isr::ISRPermissions& permissions);
    void sectionsListChanged(const QStringList& sections);

    void showErrorRequested(const QString& msg);
    void showWarningRequested(const QString& msg);

public slots:
    void onSectionChosen(const QString& sectionName);
    void onLoadSectionRequested(const QString& sectionName);
    void onTitleConfirmed(const isr::TitleSectionData& title);
    void onLoadStructureRequested();
    void onCloseSectionsRequested();

private slots:
    void onStartupLoaded(const QStringList& sections);
    void onStartupFailed(const QString& error);
    void onStartupMessage(const QString& msg);
    void onStartupProgress(int progress);

private:
    void publishState();
    isr::ISRUiState buildUiState() const;
    isr::ISRPermissions buildPermissions() const;
    void ensureService();

private:
    std::shared_ptr<const AppConfig> cfg_;
    isr::ISRSessionState session_;
    SectionsCatalogService* sectionsService_ = nullptr;
};

} // namespace isr
