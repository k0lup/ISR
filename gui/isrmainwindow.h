#ifndef ISRMAINWINDOW_H
#define ISRMAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QCloseEvent>
#include <memory>
#include <QListWidget>
#include <QThread>
#include <QStateMachine>
#include <QState>
#include <QElapsedTimer>
#include "config/app_config.h"
#include "loadingoverlay.h"
#include "setReader/sectionsloader.h"
#include "menuWgt/sectionlistwgt.h"
#include "menuWgt/titlesectionwgt.h"
#include "servises/catalogmanager.h"

class ISRMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    ISRMainWindow(std::shared_ptr<const AppConfig> cfg, QWidget *parent = nullptr);
    ~ISRMainWindow();
signals:
    void startupCompleted();
    void sectionChosen();
    void titleConfirmed();
    void sectionsClosed();
    void structureLoaded();
    void errorDetected();
    void errorResetRequested();

    void loadSetSectionRequest(const QString& section_name);
public slots:
    void showLoading(const QString& msg, bool indeterminate = false);
    void setLoadingMessage(const QString& msg);
    void setLoadingProgress(int percent);
    void hideLoading();
    void showErrorMessage(const QString& msg);

    void setSelectedSection(const QString& section_name);

    void loadSelectedSection(const QString& section_name);
    void onSectionSetLoaded(const QString& section_name, Section section);

    void onReadySectionPaths(const quint64 request_id, const QString& section, const QStringList& paths);
private slots:
    void beginStartup();
    void onSectionsListActTriggered();
    void onTitleOfSectionActTriggered();

    void enterStartupLoading();
    void enterIdleNoSection();
    void enterSectionSelected();
    void enterSectionTitleReady();
    void enterStructureReady();
    void enterErrorDetected();
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    std::shared_ptr<const AppConfig> cfg_;
    LoadingOverlay* load_overlay_ = nullptr;
    SectionsLoader* sections_loader_ = nullptr;
    SectionListWgt* section_list_wgt_ = nullptr; //виджет пункт меню "выбрать раздел"
    MODE_SECTION_LIST_WGT mode_sections_list_wgt_ = MODE_SECTION_LIST_WGT::FOR_SELECT; //режим работы окна пунккта меню "выбрать раздел"
    TitleSectionWgt* title_section_wgt_ = nullptr; //виджет пункта меню "заголовок раздела"

    QElapsedTimer time_after_start_head_section_;

    QString active_section_name_;
    QThread* sections_thread_ = nullptr;

    CatalogManager* catalog_manager_ = nullptr;
    QThread* catalog_manager_thread_ = nullptr;
    quint64 last_request_id_ = -1;

    quint64 getNextRequestId();

    bool readSections(const QStringList& sections_path, const QStringList& sections);

    QStateMachine machine_;
    QState* st_startup_loading_ = nullptr;
    QState* st_idle_no_section_ = nullptr;
    QState* st_section_selected_ = nullptr;
    QState* st_title_ready_ = nullptr;
    QState* st_structure_ready_ = nullptr;
    QState* st_error_detected_ = nullptr;

    void buildStateMachine();

    QMenu* sections_menu_ = nullptr;
    QMenu* preparation_menu_ = nullptr;
    QMenu* segments_menu_ = nullptr;
    QMenu* work_menu_ = nullptr;
    QMenu* auto_menu_ = nullptr;
    QMenu* docs_menu_ = nullptr;
    QMenu* sections_rep_menu_ = nullptr;
    QMenu* segments_rep_menu_ = nullptr;

    QAction* sections_list_action_ = nullptr;
    QAction* title_of_section_ = nullptr;
    QAction* pfks_action_ = nullptr;
    QAction* control_spo_action_ = nullptr;
    QAction* load_structure_action_ = nullptr;
    QAction* debug_of_section_action_ = nullptr;
    QAction* add_sections_on_list_sections_action_ = nullptr;
    QAction* rebuild_sections_list_action_ = nullptr;
    QAction* stencil_action_ = nullptr;

    QAction* regular_con_section_action_ = nullptr;
    QAction* nshc_of_section_action_ = nullptr;
    QAction* pri_of_section_action_ = nullptr;

    QAction* run_operation_action_ = nullptr;
    QAction* cancel_var_action_ = nullptr;
    QAction* sp_action_ = nullptr;
    QAction* go_to_active_segmets_action_ = nullptr;
    QAction* imin_answer_pris_to_ko_action_ = nullptr;
    QAction* time_of_head_section_action_ = nullptr;
    QAction* end_section_action_ = nullptr;
    QAction* exit_of_sections_action_ = nullptr;

    QAction* start_or_end_auto_label_action_ = nullptr;
    QAction* unset_auto_label_action_ = nullptr;
    QAction* run_auto_block_action_ = nullptr;
    QAction* stop_auto_block_action_ = nullptr;
};

#endif // ISRMAINWINDOW_H
