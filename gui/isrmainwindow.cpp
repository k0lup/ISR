#include "gui/isrmainwindow.h"

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QCoreApplication>

#include "application/isr_controller.h"
#include "domain/title_section_data.h"
#include "gui/loadingoverlay.h"
#include "menuWgt/sectionlistwgt.h"
#include "menuWgt/titlesectionwgt.h"

ISRMainWindow::ISRMainWindow(std::shared_ptr<const AppConfig> cfg, QWidget* parent)
    : QMainWindow(parent)
    , cfg_(std::move(cfg))
{
    loadOverlay_ = new LoadingOverlay(this);
    sectionListWgt_ = new SectionListWgt(this);
    titleSectionWgt_ = new TitleSectionWgt(this);

    buildMenus();
    connectUi();

    controller_ = new isr::ISRController(cfg_, this);
    connect(controller_, &isr::ISRController::uiStateChanged,
            this, &ISRMainWindow::applyUiState);
    connect(controller_, &isr::ISRController::permissionsChanged,
            this, &ISRMainWindow::applyPermissions);
    connect(controller_, &isr::ISRController::sectionsListChanged,
            this, &ISRMainWindow::setSectionsList);
    connect(controller_, &isr::ISRController::showErrorRequested,
            this, &ISRMainWindow::showErrorMessage);
    connect(controller_, &isr::ISRController::showWarningRequested,
            this, &ISRMainWindow::showWarningMessage);

    controller_->start();
}

ISRMainWindow::~ISRMainWindow() = default;

void ISRMainWindow::buildMenus()
{
    setWindowTitle(QStringLiteral("ИСР - [раздел не выбран]"));

    preparationMenu_ = menuBar()->addMenu(QStringLiteral("Подготовка"));
    sectionsMenu_ = menuBar()->addMenu(QStringLiteral("Разделы"));
    segmentsMenu_ = menuBar()->addMenu(QStringLiteral("Секции"));
    workMenu_ = menuBar()->addMenu(QStringLiteral("Работа"));
    autoMenu_ = menuBar()->addMenu(QStringLiteral("Авто"));
    docsMenu_ = menuBar()->addMenu(QStringLiteral("ЭД"));
    sectionsRepMenu_ = menuBar()->addMenu(QStringLiteral("Разделы РЭП"));
    segmentsRepMenu_ = menuBar()->addMenu(QStringLiteral("Секции РЭП"));

    sectionsListAction_ = preparationMenu_->addAction(QStringLiteral("Список разделов..."));
    titleOfSectionAction_ = preparationMenu_->addAction(QStringLiteral("Заголовок раздела..."));
    pfksAction_ = preparationMenu_->addAction(QStringLiteral("ПФКС..."));
    controlSpoAction_ = preparationMenu_->addAction(QStringLiteral("Контроль СПО..."));
    loadStructureAction_ = preparationMenu_->addAction(QStringLiteral("Загрузка структуры"));
    preparationMenu_->addSeparator();
    debugOfSectionAction_ = preparationMenu_->addAction(QStringLiteral("Отладка раздела..."));
    preparationMenu_->addSeparator();
    addSectionsAction_ = preparationMenu_->addAction(QStringLiteral("Добавить разделы в список разделов..."));
    rebuildSectionsAction_ = preparationMenu_->addAction(QStringLiteral("Перестроить список разделов"));
    stencilAction_ = preparationMenu_->addAction(QStringLiteral("Трафарет..."));
    QMenu* dangerMenu = preparationMenu_->addMenu(QStringLiteral("Нештат"));
    dangerMenu->addAction(QStringLiteral("Выход"));

    regularSectionAction_ = segmentsMenu_->addAction(QStringLiteral("Штатное проведение раздела"));
    nshsSectionAction_ = segmentsMenu_->addAction(QStringLiteral("НШС раздела"));
    priSectionAction_ = segmentsMenu_->addAction(QStringLiteral("ПРИ раздела"));

    runOperationAction_ = workMenu_->addAction(QStringLiteral("Выполнить операцию"));
    cancelVariantAction_ = workMenu_->addAction(QStringLiteral("Отменить вариант"));
    spAction_ = workMenu_->addAction(QStringLiteral("СП..."));
    goToActiveSegmentsAction_ = workMenu_->addAction(QStringLiteral("Перейти к активной сессии"));
    imitatePrisAnswerAction_ = workMenu_->addAction(QStringLiteral("Имитация ответа ПРИС на КО (ненорм.)"));
    headSectionTimeAction_ = workMenu_->addAction(QStringLiteral("Продолжительность головного раздела..."));
    workMenu_->addAction(QStringLiteral("Вернуть окно на место"));
    workMenu_->addSeparator();
    endSectionAction_ = workMenu_->addAction(QStringLiteral("Конец раздела..."));
    exitSectionsAction_ = workMenu_->addAction(QStringLiteral("Выход из разделов..."));
    workMenu_->addSeparator();
    workMenu_->addAction(QStringLiteral("Настройка"));

    markAutoAction_ = autoMenu_->addAction(QStringLiteral("Начать/окончить отметку блока 'Авто'"));
    unsetAutoAction_ = autoMenu_->addAction(QStringLiteral("Снять отметку блока 'Авто'"));
    autoMenu_->addSeparator();
    runAutoBlockAction_ = autoMenu_->addAction(QStringLiteral("Выполнить блок 'Авто'"));
    stopAutoBlockAction_ = autoMenu_->addAction(QStringLiteral("Остановить выполнение блока 'Авто'"));

    docsMenu_->addAction(QStringLiteral("НШС изделия"));
    docsMenu_->addAction(QStringLiteral("АВАР изделия"));
    docsMenu_->addAction(QStringLiteral("ПРИ изделия"));
    docsMenu_->addAction(QStringLiteral("НШС РЭП"));
    docsMenu_->addAction(QStringLiteral("АВАР РЭП"));
    docsMenu_->addAction(QStringLiteral("ПРИ РЭП"));

    sectionsRepMenu_->addAction(QStringLiteral("Список разделов РЭП"));

    segmentsRepMenu_->addAction(QStringLiteral("Раздел РЭП"));
    segmentsRepMenu_->addAction(QStringLiteral("Второй раздел"));
    segmentsRepMenu_->addAction(QStringLiteral("Третий раздел"));
    segmentsRepMenu_->addAction(QStringLiteral("Конец раздела"));
    segmentsRepMenu_->addAction(QStringLiteral("Выход из разделов"));

    rebuildSectionsMenu(QStringList());
}

void ISRMainWindow::connectUi()
{
    connect(sectionsListAction_, &QAction::triggered,
            this, &ISRMainWindow::onSectionsListActTriggered);
    connect(titleOfSectionAction_, &QAction::triggered,
            this, &ISRMainWindow::onTitleOfSectionActTriggered);
    connect(loadStructureAction_, &QAction::triggered,
            this, &ISRMainWindow::onLoadStructureTriggered);
    connect(exitSectionsAction_, &QAction::triggered,
            this, &ISRMainWindow::onExitSectionsTriggered);
}

void ISRMainWindow::applyUiState(const isr::ISRUiState& state)
{
    currentUiState_ = state;
    setWindowTitle(state.windowTitle);

    if (state.loadingVisible) {
        showLoading(state.loadingMessage, state.loadingIndeterminate);
        if (state.loadingProgressKnown) {
            setLoadingProgress(state.loadingProgress);
        }
    } else {
        hideLoading();
    }

    rebuildSectionsMenu(state.openedSections);
}

void ISRMainWindow::applyPermissions(const isr::ISRPermissions& p)
{
    sectionsListAction_->setEnabled(p.canOpenSectionsList);
    titleOfSectionAction_->setEnabled(p.canEditTitle);
    pfksAction_->setEnabled(p.canOpenPfks);
    controlSpoAction_->setEnabled(p.canControlSpo);
    loadStructureAction_->setEnabled(p.canLoadStructure);
    debugOfSectionAction_->setEnabled(p.canDebugSection);
    addSectionsAction_->setEnabled(p.canAddSections);
    rebuildSectionsAction_->setEnabled(p.canRebuildSections);
    stencilAction_->setEnabled(p.canOpenStencil);

    regularSectionAction_->setEnabled(p.canSelectRegularSection);
    nshsSectionAction_->setEnabled(p.canSelectNshsSection);
    priSectionAction_->setEnabled(p.canSelectPriSection);

    runOperationAction_->setEnabled(p.canRunOperation);
    cancelVariantAction_->setEnabled(p.canCancelVariant);
    spAction_->setEnabled(p.canOpenSp);
    goToActiveSegmentsAction_->setEnabled(p.canGoToActiveSegments);
    imitatePrisAnswerAction_->setEnabled(p.canImitatePrisAnswer);
    headSectionTimeAction_->setEnabled(p.canShowHeadDuration);
    endSectionAction_->setEnabled(p.canEndSection);
    exitSectionsAction_->setEnabled(p.canExitSections);

    markAutoAction_->setEnabled(p.canMarkAuto);
    unsetAutoAction_->setEnabled(p.canUnsetAuto);
    runAutoBlockAction_->setEnabled(p.canRunAutoBlock);
    stopAutoBlockAction_->setEnabled(p.canStopAutoBlock);
}

void ISRMainWindow::setSectionsList(const QStringList& sections)
{
    sectionListWgt_->setSections(sections);
}

void ISRMainWindow::showLoading(const QString& msg, bool indeterminate)
{
    loadOverlay_->setIndeterminate(indeterminate);
    loadOverlay_->setMessage(msg);
    loadOverlay_->raise();
    loadOverlay_->show();
    loadOverlay_->setFocus();
}

void ISRMainWindow::setLoadingMessage(const QString& msg)
{
    loadOverlay_->setMessage(msg);
}

void ISRMainWindow::setLoadingProgress(int percent)
{
    loadOverlay_->setProgress(percent);
}

void ISRMainWindow::hideLoading()
{
    loadOverlay_->hide();
}

void ISRMainWindow::showErrorMessage(const QString& msg)
{
    QMessageBox::critical(this, QStringLiteral("Ошибка"), msg);
}

void ISRMainWindow::showWarningMessage(const QString& msg)
{
    QMessageBox::warning(this, QStringLiteral("Предупреждение"), msg);
}

void ISRMainWindow::onSectionsListActTriggered()
{
    if (sectionListWgt_->exec(MODE_SECTION_LIST_WGT::FOR_SELECT) != QDialog::Accepted) {
        return;
    }

    const SectionSelected selected = sectionListWgt_->getSection();
    if (selected.mode == MODE_SECTION_LIST_WGT::FOR_LOAD) {
        controller_->onLoadSectionRequested(selected.section_name);
        return;
    }

    controller_->onSectionChosen(selected.section_name);
}

void ISRMainWindow::onTitleOfSectionActTriggered()
{
    if (currentUiState_.activeSectionName.trimmed().isEmpty()) {
        showWarningMessage(QStringLiteral("Сначала выберите раздел"));
        return;
    }

    if (titleSectionWgt_->exec(currentUiState_.activeSectionName) != QDialog::Accepted) {
        return;
    }

    const TitleSection title = titleSectionWgt_->getTitleOfSection();

    isr::TitleSectionData data;
    data.nameSection = title.name_section;
    data.protocolName = title.name_of_prot;
    data.traf = title.traf;
    data.rep = title.rep;
    data.operatorPuAis = title.operator_pu_ais;
    data.operatorAis = title.operator_ais;
    data.operatorTkckSsBvs = title.operator_tkck_ss_bvs;
    data.operatorNtkSi = title.operator_ntk_si;
    data.operatorBs = title.operator_bs;

    controller_->onTitleConfirmed(data);
}

void ISRMainWindow::onLoadStructureTriggered()
{
    controller_->onLoadStructureRequested();
}

void ISRMainWindow::onExitSectionsTriggered()
{
    controller_->onCloseSectionsRequested();
}

void ISRMainWindow::rebuildSectionsMenu(const QStringList& sections)
{
    sectionsMenu_->clear();

    if (sections.isEmpty()) {
        sectionsMenu_->addAction(QStringLiteral("< Разделов нет >"));
        return;
    }

    for (const QString& section : sections) {
        sectionsMenu_->addAction(section);
    }
}

void ISRMainWindow::closeEvent(QCloseEvent *event) {
    event->accept();

    QCoreApplication::quit();
}
