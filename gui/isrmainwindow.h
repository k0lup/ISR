#pragma once

#include <QMainWindow>
#include <QCloseEvent>
#include <memory>

#include "config/app_config.h"
#include "domain/isr_permissions.h"
#include "domain/isr_ui_state.h"

class LoadingOverlay;
class SectionListWgt;
class TitleSectionWgt;
class QMenu;
class QAction;

namespace isr {
class ISRController;
}

class ISRMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ISRMainWindow(std::shared_ptr<const AppConfig> cfg, QWidget* parent = nullptr);
    ~ISRMainWindow() override;

public slots:
    void applyUiState(const isr::ISRUiState& state);
    void applyPermissions(const isr::ISRPermissions& permissions);
    void setSectionsList(const QStringList& sections);

    void showLoading(const QString& msg, bool indeterminate = false);
    void setLoadingMessage(const QString& msg);
    void setLoadingProgress(int percent);
    void hideLoading();
    void showErrorMessage(const QString& msg);
    void showWarningMessage(const QString& msg);

private slots:
    void onSectionsListActTriggered();
    void onTitleOfSectionActTriggered();
    void onLoadStructureTriggered();
    void onExitSectionsTriggered();
protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void buildMenus();
    void connectUi();
    void rebuildSectionsMenu(const QStringList& sections);

private:
    std::shared_ptr<const AppConfig> cfg_;
    isr::ISRController* controller_ = nullptr;

    LoadingOverlay* loadOverlay_ = nullptr;
    SectionListWgt* sectionListWgt_ = nullptr;
    TitleSectionWgt* titleSectionWgt_ = nullptr;

    isr::ISRUiState currentUiState_;

    QMenu* sectionsMenu_ = nullptr;
    QMenu* preparationMenu_ = nullptr;
    QMenu* segmentsMenu_ = nullptr;
    QMenu* workMenu_ = nullptr;
    QMenu* autoMenu_ = nullptr;
    QMenu* docsMenu_ = nullptr;
    QMenu* sectionsRepMenu_ = nullptr;
    QMenu* segmentsRepMenu_ = nullptr;

    QAction* sectionsListAction_ = nullptr;
    QAction* titleOfSectionAction_ = nullptr;
    QAction* pfksAction_ = nullptr;
    QAction* controlSpoAction_ = nullptr;
    QAction* loadStructureAction_ = nullptr;
    QAction* debugOfSectionAction_ = nullptr;
    QAction* addSectionsAction_ = nullptr;
    QAction* rebuildSectionsAction_ = nullptr;
    QAction* stencilAction_ = nullptr;

    QAction* regularSectionAction_ = nullptr;
    QAction* nshsSectionAction_ = nullptr;
    QAction* priSectionAction_ = nullptr;

    QAction* runOperationAction_ = nullptr;
    QAction* cancelVariantAction_ = nullptr;
    QAction* spAction_ = nullptr;
    QAction* goToActiveSegmentsAction_ = nullptr;
    QAction* imitatePrisAnswerAction_ = nullptr;
    QAction* headSectionTimeAction_ = nullptr;
    QAction* endSectionAction_ = nullptr;
    QAction* exitSectionsAction_ = nullptr;

    QAction* markAutoAction_ = nullptr;
    QAction* unsetAutoAction_ = nullptr;
    QAction* runAutoBlockAction_ = nullptr;
    QAction* stopAutoBlockAction_ = nullptr;
};
