#include "mainwindow.h"
#include <QMap>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QInputDialog>
#include "Services/GUI/ProgViewWgt/programviewwidget.h"
#include "Services/GUI/MenuDialogWindow/sectionlistdialog.h"

#include <Logger/logger.h>
#include "logging_categories.h"

MainWindow::MainWindow(const AppConfig& cfg, QWidget *parent) : QMainWindow(parent), cfg_(cfg)
{
    qCInfo(logGUI) << "Вызван конструктор MainWindow";
    createActions();
    createMenu();
    setupConnections();

    exec_wgt_ = new ProgramViewWidget(this);
    setCentralWidget(exec_wgt_);

    qCInfo(logGUI) << "Завершен конструктор MainWindow";
}

void MainWindow::createActions()
{
    qCInfo(logGUI) << "Вызван метод createActions()";

    list_of_sections_act_ = new QAction("Список разделов...", this);
    title_of_secion_act_ = new QAction("Заголовок раздела...", this);
    pfks_act_ = new QAction("ПФКС...", this);
    control_spo_act_ = new QAction("Контроль СПО...", this);
    load_struct_act_ = new QAction("Загрузка структуры", this);
    debug_section_act_ = new QAction("Отладка раздела...", this);
    add_sections_on_list_act_ = new QAction("Добавить разделы в список разделов...", this);
    rebuild_list_sections_act_ = new QAction("Перестроить список разделов", this);
    traf_act_ = new QAction("Трафарет...", this);

    exit_act_ = new QAction("Выход", this);
    QList<QAction*> sections_act_list_;

    state_chapter_act_ = new QAction("Штатное проведение раздела", this);
    not_state_chapter_act_ = new QAction("НШС раздела", this);
    pri_chapter_act_ = new QAction("ПРИ раздела", this);

    run_operations_act_ = new QAction("Выполнить операцию", this);
    cancel_variant_act_ = new QAction("Отменить вариант", this);
    save_prot_act_ = new QAction("СП...", this);
    go_to_active_chapter_act_ = new QAction("Перейти к активной секции", this);
    imit_answer_pris_act_ = new QAction("Имитация ответа ПРИС на КО (ненорм.)", this);
    time_of_head_section_act_ = new QAction("Продолжительность головного раздела...", this);
    set_start_pos_window_act_ = new QAction("Вернуть окно на место", this);
    end_of_section_act_ = new QAction("Конец раздела...", this);
    exit_of_sections_act_ = new QAction("Выход из разделов...", this);
    settings_act_ = new QAction("Настройка...", this);

    start_or_end_auto_block_act_ = new QAction("Начать/окончить отметку блока 'Авто'", this);
    remove_auto_mark_act_ = new QAction("Снять отметку блока 'Авто'", this);
    run_auto_block_act_ = new QAction("Выполнить блок 'Авто'", this);
    stop_auto_block_act_ = new QAction("Остановить выполнение блока 'Авто'", this);

    nshs_isd_act_ = new QAction("НШС изделия", this);
    avar_isd_act_ = new QAction("АВАР изделия", this);
    pri_isd_act_ = new QAction("ПРИ изделия", this);
    nshs_rep_act_ = new QAction("НШС РЭП", this);
    avar_rep_act_ = new QAction("АВАР РЭП", this);
    pri_rep_act_ = new QAction("ПРИ РЭП", this);

    list_of_sections_rep_act_ = new QAction("Список разделов РЭП", this);

    sections_rep_act_ = new QAction("Раздел РЭП", this);
    second_section_rep_act_ = new QAction("Второй раздел", this);
    third_section_rep_act_ = new QAction("Третий раздел", this);
    end_of_rep_section_act_ = new QAction("Конец раздела", this);
    exit_of_rep_section_act_ = new QAction("Выход из разделов", this);

    qCInfo(logGUI) << "Завершен метод createActions()";
}

void MainWindow::createMenu()
{
    qCInfo(logGUI) << "Вызван метод createMenu()";

    preparaion_menu_ = new QMenu("Подготовка", this);
    {
        preparaion_menu_->addAction(list_of_sections_act_);
        preparaion_menu_->addAction(title_of_secion_act_);
        preparaion_menu_->addAction(pfks_act_);
        preparaion_menu_->addAction(control_spo_act_);
        preparaion_menu_->addAction(load_struct_act_);
        preparaion_menu_->addSeparator();
        preparaion_menu_->addAction(debug_section_act_);
        preparaion_menu_->addSeparator();
        preparaion_menu_->addAction(add_sections_on_list_act_);
        preparaion_menu_->addAction(rebuild_list_sections_act_);
        preparaion_menu_->addAction(traf_act_);

        not_state_menu_ = new QMenu("Нештат", this);
        {
            not_state_menu_->addAction(exit_act_);
        }

        preparaion_menu_->addMenu(not_state_menu_);
    }
    sections_menu_= new QMenu("Разделы", this);
    {
        rebuildSectionsMenu();
    }
    chapters_menu_ = new QMenu("Секции", this);
    {
        chapters_menu_->addAction(state_chapter_act_);
        chapters_menu_->addAction(not_state_chapter_act_);
        chapters_menu_->addAction(pri_chapter_act_);

        state_chapter_act_->setCheckable(true);
        state_chapter_act_->setChecked(false);
        not_state_chapter_act_->setCheckable(true);
        not_state_chapter_act_->setChecked(false);
        pri_chapter_act_->setCheckable(true);
        pri_chapter_act_->setChecked(false);
    }
    work_menu_ = new QMenu("Работа", this);
    {
        work_menu_->addAction(run_operations_act_);
        work_menu_->addAction(cancel_variant_act_);
        work_menu_->addAction(save_prot_act_);
        work_menu_->addAction(go_to_active_chapter_act_);
        work_menu_->addAction(imit_answer_pris_act_);
        work_menu_->addAction(time_of_head_section_act_);
        work_menu_->addAction(set_start_pos_window_act_);
        work_menu_->addSeparator();
        work_menu_->addAction(end_of_section_act_);
        work_menu_->addAction(exit_of_sections_act_);
        work_menu_->addSeparator();
        work_menu_->addAction(settings_act_);
    }
    auto_menu_ = new QMenu("Авто", this);
    {
        auto_menu_->addAction(start_or_end_auto_block_act_);
        auto_menu_->addAction(remove_auto_mark_act_);
        auto_menu_->addSeparator();
        auto_menu_->addAction(run_auto_block_act_);
        auto_menu_->addAction(stop_auto_block_act_);
    }
    ed_menu_ = new QMenu("ЭД", this);
    {
        ed_menu_->addAction(nshs_isd_act_);
        ed_menu_->addAction(avar_isd_act_);
        ed_menu_->addAction(pri_isd_act_);
        ed_menu_->addAction(nshs_rep_act_);
        ed_menu_->addAction(avar_rep_act_);
        ed_menu_->addAction(pri_rep_act_);
    }
    sections_rep_menu_ = new QMenu("Разделы РЭП", this);
    {
        sections_rep_menu_->addAction(list_of_sections_rep_act_);
    }
    chapters_rep_menu_ = new QMenu("Секции РЭП", this);
    {
        chapters_rep_menu_->addAction(sections_rep_act_);
        chapters_rep_menu_->addAction(second_section_rep_act_);
        chapters_rep_menu_->addAction(third_section_rep_act_);
        chapters_rep_menu_->addAction(end_of_rep_section_act_);
        chapters_rep_menu_->addAction(exit_of_rep_section_act_);
    }

    menuBar()->addMenu(preparaion_menu_);
    menuBar()->addMenu(sections_menu_);
    menuBar()->addMenu(chapters_menu_);
    menuBar()->addMenu(work_menu_);
    menuBar()->addMenu(auto_menu_);
    menuBar()->addMenu(ed_menu_);
    menuBar()->addMenu(sections_rep_menu_);
    menuBar()->addMenu(chapters_rep_menu_);

    qCInfo(logGUI) << "Завершен метод createMenu()";
}

void MainWindow::setActiveSectionInMenu(const QString& section_name)
{
    for (int i = 0; i < sections_act_list_.count(); ++i) {
        sections_act_list_[i]->setChecked(false);
    }

    if (!loaded_sections_.contains(section_name)) {
        loaded_sections_.append(section_name);
        rebuildSectionsMenu();
    }

    if (loaded_sections_.contains(section_name)) {
        for (int i = 0; i < sections_act_list_.count(); ++i) {
            if (sections_act_list_[i]->text() == section_name) {
                sections_act_list_[i]->setChecked(true);
                break;
            }
        }
    }
}

void MainWindow::rebuildSectionsMenu()
{
    qCInfo(logGUI) << "Вызван метод rebuildSectionsMenu()";
    if (loaded_sections_.isEmpty()) {
        clearSectionsMenu();
        QAction* no_section_act = new QAction("< Разделов нет >", this);
        sections_menu_->addAction(no_section_act);
        no_section_act->setCheckable(false);
        qCInfo(logGUI) << "Завершен метод createActions() - активных разделов нет";
        return;
    }

    QStringList sections_in_menu;
    for (int i = sections_act_list_.count() - 1; i >= 0; --i) {
        QAction* act = sections_act_list_[i];

        if (!loaded_sections_.contains(act->text())) {
            qCDebug(logGUI) << QString("Раздела '%1' нет в списке загруженных - удаляем его из меню").arg(act->text());
            sections_menu_->removeAction(act);
            sections_act_list_.removeAt(i);
            act->deleteLater();
        } else {
            sections_in_menu.append(act->text());
        }
    }

    for (const auto& section_name : loaded_sections_) {
        if (!sections_in_menu.contains(section_name)) {
            qCDebug(logGUI) << QString("Раздела '%1' нет в меню - добавляем").arg(section_name);
            QAction* act = new QAction(section_name, this);
            act->setCheckable(true);
            act->setChecked(false);
            QObject::connect(act, &QAction::triggered, this, [this, act](){
                onSectionsActListItemTriggered(act);
            });
            sections_act_list_.append(act);
            sections_menu_->addAction(act);
        }
    }

    qCInfo(logGUI) << "Завершен метод rebuildSectionsMenu() - разделы актуализированы";
}

void MainWindow::clearSectionsMenu()
{
    qCInfo(logGUI) << "Вызван метод createActions()";

    sections_menu_->clear();
    for (QAction* action : sections_act_list_) {
        action->deleteLater();
    }

    sections_act_list_.clear();

    qCInfo(logGUI) << "Завершен метод createActions()";
}

//connect'ы для sections_menu_ выполняются в методе MainWindow::rebuildSectionsMenu();
void MainWindow::setupConnections()
{
    qCInfo(logGUI) << "Вызван метод setupConnections()";

    QObject::connect(list_of_sections_act_,         &QAction::triggered, this, &MainWindow::onListOfSectionsActTriggered);
    QObject::connect(title_of_secion_act_,          &QAction::triggered, this, &MainWindow::onTitleOfSecionActTriggered);
    QObject::connect(pfks_act_,                     &QAction::triggered, this, &MainWindow::onPfksActTriggered);
    QObject::connect(control_spo_act_,              &QAction::triggered, this, &MainWindow::onControlSpoActTriggered);
    QObject::connect(load_struct_act_,              &QAction::triggered, this, &MainWindow::onLoadStructActTriggered);
    QObject::connect(debug_section_act_,            &QAction::triggered, this, &MainWindow::onDebugSectionActTriggered);
    QObject::connect(add_sections_on_list_act_,     &QAction::triggered, this, &MainWindow::onAddSectionsOnListActTriggered);
    QObject::connect(rebuild_list_sections_act_,    &QAction::triggered, this, &MainWindow::onRebuildListSectionsActTriggered);
    QObject::connect(traf_act_,                     &QAction::triggered, this, &MainWindow::onTrafActTriggered);
    QObject::connect(exit_act_,                     &QAction::triggered, this, &MainWindow::onExitActTriggered);
    QObject::connect(state_chapter_act_,            &QAction::triggered, this, &MainWindow::onStateChapterActTriggered);
    QObject::connect(not_state_chapter_act_,        &QAction::triggered, this, &MainWindow::onNotStateChapterActTriggered);
    QObject::connect(pri_chapter_act_,              &QAction::triggered, this, &MainWindow::onPriChapterActTriggered);
    QObject::connect(run_operations_act_,           &QAction::triggered, this, &MainWindow::onRunOperationsActTriggered);
    QObject::connect(cancel_variant_act_,           &QAction::triggered, this, &MainWindow::onCancelVariantActTriggered);
    QObject::connect(save_prot_act_,                &QAction::triggered, this, &MainWindow::onSaveProtActTriggered);
    QObject::connect(go_to_active_chapter_act_,     &QAction::triggered, this, &MainWindow::onGoToActiveChapterActTriggered);
    QObject::connect(imit_answer_pris_act_,         &QAction::triggered, this, &MainWindow::onImitAnswerPrisActTriggered);
    QObject::connect(time_of_head_section_act_,     &QAction::triggered, this, &MainWindow::onTimeOfHeadSectionActTriggered);
    QObject::connect(set_start_pos_window_act_,     &QAction::triggered, this, &MainWindow::onSetStartPosWindowActTriggered);
    QObject::connect(end_of_section_act_,           &QAction::triggered, this, &MainWindow::onEndOfSectionActTriggered);
    QObject::connect(exit_of_sections_act_,         &QAction::triggered, this, &MainWindow::onExitOfSectionsActTriggered);
    QObject::connect(settings_act_,                 &QAction::triggered, this, &MainWindow::onSettingsActTriggered);
    QObject::connect(start_or_end_auto_block_act_,  &QAction::triggered, this, &MainWindow::onStartOrEndAutoBlockActTriggered);
    QObject::connect(remove_auto_mark_act_,         &QAction::triggered, this, &MainWindow::onRemoveAutoMarkActTriggered);
    QObject::connect(run_auto_block_act_,           &QAction::triggered, this, &MainWindow::onRunAutoBlockActTriggered);
    QObject::connect(stop_auto_block_act_,          &QAction::triggered, this, &MainWindow::onStopAutoBlockActTriggered);
    QObject::connect(nshs_isd_act_,                 &QAction::triggered, this, &MainWindow::onNshsIsdActTriggered);
    QObject::connect(avar_isd_act_,                 &QAction::triggered, this, &MainWindow::onAvarIsdActTriggered);
    QObject::connect(pri_isd_act_,                  &QAction::triggered, this, &MainWindow::onPriIsdActTriggered);
    QObject::connect(nshs_rep_act_,                 &QAction::triggered, this, &MainWindow::onNshsRepActTriggered);
    QObject::connect(avar_rep_act_,                 &QAction::triggered, this, &MainWindow::onAvarRepActTriggered);
    QObject::connect(pri_rep_act_,                  &QAction::triggered, this, &MainWindow::onPriRepActTriggered);
    QObject::connect(list_of_sections_rep_act_,     &QAction::triggered, this, &MainWindow::onListOfSectionsRepActTriggered);
    QObject::connect(sections_rep_act_,             &QAction::triggered, this, &MainWindow::onSectionsRepActTriggered);
    QObject::connect(second_section_rep_act_,       &QAction::triggered, this, &MainWindow::onSecondSectionRepActTriggered);
    QObject::connect(third_section_rep_act_,        &QAction::triggered, this, &MainWindow::onThirdSectionRepActTriggered);
    QObject::connect(end_of_rep_section_act_,       &QAction::triggered, this, &MainWindow::onEndOfRepSectionActTriggered);
    QObject::connect(exit_of_rep_section_act_,      &QAction::triggered, this, &MainWindow::onExitOfRepSectionActTriggered);

    qCInfo(logGUI) << "Завершен метод setupConnections()";
}

void MainWindow::onListOfSectionsActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Подготовка'->'Список разделов...'";

    SectionListDialog section_list_dialog;
    section_list_dialog.setSections(all_sections_);
    if (section_list_dialog.exec(SectionListDialog::Mode::LOAD) == QDialog::Accepted) {
        QString section_name = section_list_dialog.getSelectedSection();
        emit loadSectionRequested(section_name, RequestIdGenerator::NO_REQUEST_ID);
    }


    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Подготовка'->'Список разделов...'";
}

void MainWindow::onTitleOfSecionActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Подготовка'->'Заголовок раздела...'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Подготовка'->'Заголовок раздела...'";
}

void MainWindow::onPfksActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Подготовка'->'ПФКС...'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Подготовка'->'ПФКС...'";
}

void MainWindow::onControlSpoActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Подготовка'->'Контроль СПО...'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Подготовка'->'Контроль СПО...'";
}

void MainWindow::onLoadStructActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Подготовка'->'Загрузка структуры'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Подготовка'->'Загрузка структуры'";
}

void MainWindow::onDebugSectionActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Подготовка'->'Отладка раздела...'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Подготовка'->'Отладка раздела...'";
}

void MainWindow::onAddSectionsOnListActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Подготовка'->'Добавить разделы в список разделов...'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Подготовка'->'Добавить разделы в список разделов...'";
}

void MainWindow::onRebuildListSectionsActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Подготовка'->'Перестроить список разделов'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Подготовка'->'Перестроить список разделов'";
}

void MainWindow::onTrafActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Подготовка'->'Трафарет...'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Подготовка'->'Трафарет...'";
}

void MainWindow::onExitActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Подготовка'->'Нештат'->'Выход'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Подготовка'->'Нештат'->'Выход'";
}

void MainWindow::onSectionsActListItemTriggered(QAction* act_triggered)
{
    QString section_name = act_triggered->text();
    qCInfo(logGUI) << QString("Вызван обработчик пункта меню 'Разделы'->'%1'").arg(section_name);

    emit requestChangeSection(section_name, RequestIdGenerator::NO_REQUEST_ID);

    qCInfo(logGUI) << QString("Завершен обработчик пункта меню 'Разделы'->'%1'").arg(section_name);
}

void MainWindow::onStateChapterActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Секции'->'Штатное проведение раздела'";

    exec_wgt_->onChapterChanged(ChapterType::STATE);

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Секции'->'Штатное проведение раздела'";
}

void MainWindow::onNotStateChapterActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Секции'->'НШС раздела'";

    exec_wgt_->onChapterChanged(ChapterType::NOT_STATE);

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Секции'->'НШС раздела'";
}

void MainWindow::onPriChapterActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Секции'->'ПРИ раздела'";

    exec_wgt_->onChapterChanged(ChapterType::PRI);

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Секции'->'ПРИ раздела'";
}

void MainWindow::onRunOperationsActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Работа'->'Выполнить операцию'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Работа'->'Выполнить операцию'";
}

void MainWindow::onCancelVariantActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Работа'->'Отменить вариант'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Работа'->'Отменить вариант'";
}

void MainWindow::onSaveProtActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Работа'->'СП...'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Работа'->'СП...'";
}

void MainWindow::onGoToActiveChapterActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Работа'->'Перейти к активной секции'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Работа'->'Перейти к активной секции'";
}

void MainWindow::onImitAnswerPrisActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Работа'->'Имитация ответа ПРИС на КО (ненорм.)'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Работа'->'Имитация ответа ПРИС на КО (ненорм.)'";
}

void MainWindow::onTimeOfHeadSectionActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Работа'->'Продолжительность головного раздела...'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Работа'->'Продолжительность головного раздела...'";
}

void MainWindow::onSetStartPosWindowActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Работа'->'Вернуть окно на место'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Работа'->'Вернуть окно на место'";
}

void MainWindow::onEndOfSectionActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Работа'->'Конец раздела...'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Работа'->'Конец раздела...'";
}

void MainWindow::onExitOfSectionsActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Работа'->'Выход из разделов...'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Работа'->'Выход из разделов...'";
}

void MainWindow::onSettingsActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Работа'->'Настройка...'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Работа'->'Настройка...'";
}

void MainWindow::onStartOrEndAutoBlockActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Авто'->'Начать/окончить отметку блока 'Авто''";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Авто'->'Начать/окончить отметку блока 'Авто''";
}

void MainWindow::onRemoveAutoMarkActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Авто'->'Снять отметку блока 'Авто''";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Авто'->'Снять отметку блока 'Авто''";
}

void MainWindow::onRunAutoBlockActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Авто'->'Выполнить блок 'Авто''";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Авто'->'Выполнить блок 'Авто''";
}

void MainWindow::onStopAutoBlockActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Авто'->'Остановить выполнение блока 'Авто''";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Авто'->'Остановить выполнение блока 'Авто''";
}

void MainWindow::onNshsIsdActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'ЭД'->'НШС изделия'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'ЭД'->'НШС изделия'";
}

void MainWindow::onAvarIsdActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'ЭД'->'АВАР изделия'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'ЭД'->'АВАР изделия'";
}

void MainWindow::onPriIsdActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'ЭД'->'ПРИ изделия'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'ЭД'->'ПРИ изделия'";
}

void MainWindow::onNshsRepActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'ЭД'->'НШС РЭП'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'ЭД'->'НШС РЭП'";
}

void MainWindow::onAvarRepActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'ЭД'->'АВАР РЭП'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'ЭД'->'АВАР РЭП'";
}

void MainWindow::onPriRepActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'ЭД'->'ПРИ РЭП'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'ЭД'->'ПРИ РЭП'";
}

void MainWindow::onListOfSectionsRepActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Разделы РЭП'->'Список разделов РЭП'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Разделы РЭП'->'Список разделов РЭП'";
}

void MainWindow::onSectionsRepActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Секции РЭП'->'Раздел РЭП'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Секции РЭП'->'Раздел РЭП'";
}

void MainWindow::onSecondSectionRepActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Секции РЭП'->'Второй раздел'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Секции РЭП'->'Второй раздел'";
}

void MainWindow::onThirdSectionRepActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Секции РЭП'->'Третий раздел'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Секции РЭП'->'Третий раздел'";
}

void MainWindow::onEndOfRepSectionActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Секции РЭП'->'Конец раздела'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Секции РЭП'->'Конец раздела'";
}

void MainWindow::onExitOfRepSectionActTriggered()
{
    qCInfo(logGUI) << "Вызван обработчик пункта меню 'Секции РЭП'->'Выход из разделов'";

    qCInfo(logGUI) << "Завершен обработчик пункта меню 'Секции РЭП'->'Выход из разделов'";
}

void MainWindow::onShowWarningRequested(const QString& warning_message)
{
    QMessageBox::warning(this, "Предупреждение", warning_message);
}

void MainWindow::onShowCriticalRequested(const QString& critical_message)
{
    QMessageBox::critical(this, "Ошибка", critical_message);
}

void MainWindow::onShowInfoRequested(const QString& info_message)
{
    QMessageBox::information(this, "Информация", info_message);
}

void MainWindow::onSectionChanged(const ExecutionSnapshot& section)
{
    setActiveSectionInMenu(section.section_name);
    setActiveChapterInMenu(section.active_chpater);
    exec_wgt_->setSection(section);
}

void MainWindow::setActiveChapterInMenu(const ChapterType& chapter)
{
    state_chapter_act_->setChecked(false);
    not_state_chapter_act_->setChecked(false);
    pri_chapter_act_->setChecked(false);
    switch (chapter) {
    case ChapterType::STATE : state_chapter_act_->setChecked(true); return;
    case ChapterType::NOT_STATE : not_state_chapter_act_->setChecked(true); return;
    case ChapterType::PRI : pri_chapter_act_->setChecked(true); return;
    }
}

ProgramViewWidget* MainWindow::getProgViewWgt() {
    return exec_wgt_;
}

void MainWindow::onRequestShowWindow(const WidgetTypes& wgt_type, const WidgetInfo& wgt_info, RequestId source_request_id)
{
    WindowResultData result_data;
    switch (wgt_type) {
    case WidgetTypes::WARNING : onShowWarningRequested(wgt_info.information.join("\n")); result_data.status = true; break;
    case WidgetTypes::INFO : result_data = showInfoWgt(wgt_info.title, wgt_info.information.join("\n")); break;
    case WidgetTypes::VARIANT : result_data = showVarWgt(wgt_info.title, wgt_info.information); break;
    }

    emit sendWindowResponse(result_data, source_request_id);
}

WindowResultData MainWindow::showInfoWgt(const QString& window_title, const QString& info)
{
    WindowResultData result_data;

    QMessageBox::StandardButton result = QMessageBox::question(
                this, window_title, info, QMessageBox::Ok | QMessageBox::Cancel,
                QMessageBox::Cancel
                );

    if (result == QMessageBox::Ok) {
        result_data.status = true;
    } else {
        result_data.status = false;
    }

    return result_data;
}

WindowResultData MainWindow::showVarWgt(const QString& window_title, const QStringList& variants)
{
    bool ok = false;

    QString selected = QInputDialog::getItem(
                this,
                window_title,
                "Выберите вариант:",
                variants,
                0,
                false,
                &ok
                );
    WindowResultData result_data;
    result_data.status = ok;
    if (!ok) {
        return result_data;
    }

    result_data.data = selected;
    return result_data;
}

void MainWindow::onListSectionLoaded(const QSet<QString>& section_list)
{
    all_sections_.clear();
    all_sections_ = section_list.toList();
}
