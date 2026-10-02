#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QMenu;
class QAction;

class ProgramViewWidget; //Убрать

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
private:
    QMenu* preparaion_menu_ = nullptr;                      //Подготовка
        QAction* list_of_sections_act_ = nullptr;               //->Список разделов...
        QAction* title_of_secion_act_ = nullptr;                //->Заголовок раздела...
        QAction* pfks_act_ = nullptr;                           //->ПФКС...
        QAction* control_spo_act_ = nullptr;                    //->Контроль СПО...
        QAction* load_struct_act_ = nullptr;                    //->Загрузка структуры
        QAction* debug_section_act_ = nullptr;                  //->Отладка раздела...
        QAction* add_sections_on_list_act_ = nullptr;           //->Добавить разделы в список разделов...
        QAction* rebuild_list_sections_act_ = nullptr;          //->Перестроить список разделов
        QAction* traf_act_ = nullptr;                           //->Трафарет...
        QMenu* not_state_menu_ = nullptr;                       //->Нештат
            QAction* exit_act_ = nullptr;                               //->Выход
    QMenu* sections_menu_ = nullptr;                         //Разделы
        QList<QAction*> sections_act_list_;                     //->Список доступных разделов
    QMenu* chapters_menu_ = nullptr;                        //Секции
        QAction* state_chapter_act_ = nullptr;                  //->Штатное проведение раздела
        QAction* not_state_chapter_act_ = nullptr;              //->НШС раздела
        QAction* pri_chapter_act_ = nullptr;                    //->ПРИ раздела
    QMenu* work_menu_ = nullptr;                            //Работа
        QAction* run_operations_act_ = nullptr;                 //->Выполнить операцию
        QAction* cancel_variant_act_ = nullptr;                 //->Отменить вариант
        QAction* save_prot_act_ = nullptr;                      //->СП...
        QAction* go_to_active_chapter_act_ = nullptr;           //->Перейти к активной секции
        QAction* imit_answer_pris_act_ = nullptr;               //->Имитация ответа ПРИС на КО (ненорм.)
        QAction* time_of_head_section_act_ = nullptr;           //->Продолжительность головного раздела...
        QAction* set_start_pos_window_act_ = nullptr;           //->Вернуть окно на место
        QAction* end_of_section_act_ = nullptr;                 //->Конец раздела...
        QAction* exit_of_sections_act_ = nullptr;               //->Выход из разделов...
        QAction* settings_act_ = nullptr;                       //->Настройка...
    QMenu* auto_menu_ = nullptr;                            //Авто
        QAction* start_or_end_auto_block_act_ = nullptr;        //->Начать/окончить отметку блока "Авто"
        QAction* remove_auto_mark_act_ = nullptr;               //->Снять отметку блока "Авто"
        QAction* run_auto_block_act_ = nullptr;                 //->Выполнить блок "Авто"
        QAction* stop_auto_block_act_ = nullptr;                //->Остановить выполнение блока "Авто"
    QMenu* ed_menu_ = nullptr;                              //ЭД
        QAction* nshs_isd_act_ = nullptr;                       //->НШС изделия
        QAction* avar_isd_act_ = nullptr;                       //->АВАР изделия
        QAction* pri_isd_act_ = nullptr;                        //->ПРИ изделия
        QAction* nshs_rep_act_ = nullptr;                       //->НШС РЭП
        QAction* avar_rep_act_ = nullptr;                       //->АВАР РЭП
        QAction* pri_rep_act_ = nullptr;                        //->ПРИ РЭП
    QMenu* sections_rep_menu_ = nullptr;                     //Разделы РЭП
        QAction* list_of_sections_rep_act_ = nullptr;           //->Список разделов РЭП
    QMenu* chapters_rep_menu_ = nullptr;                    //Секции РЭП
        QAction* sections_rep_act_ = nullptr;                   //->Раздел РЭП
        QAction* second_section_rep_act_ = nullptr;             //->Второй раздел
        QAction* third_section_rep_act_ = nullptr;              //->Третий раздел
        QAction* end_of_rep_section_act_ = nullptr;             //->Конец раздела
        QAction* exit_of_rep_section_act_ = nullptr;            //->Выход из разделов
private slots:
    void onListOfSectionsActTriggered();
    void onTitleOfSecionActTriggered();
    void onPfksActTriggered();
    void onControlSpoActTriggered();
    void onLoadStructActTriggered();
    void onDebugSectionActTriggered();
    void onAddSectionsOnListActTriggered();
    void onRebuildListSectionsActTriggered();
    void onTrafActTriggered();
    void onExitActTriggered();
    void onSectionsActListItemTriggered(QAction* act_triggered);
    void onStateChapterActTriggered();
    void onNotStateChapterActTriggered();
    void onPriChapterActTriggered();
    void onRunOperationsActTriggered();
    void onCancelVariantActTriggered();
    void onSaveProtActTriggered();
    void onGoToActiveChapterActTriggered();
    void onImitAnswerPrisActTriggered();
    void onTimeOfHeadSectionActTriggered();
    void onSetStartPosWindowActTriggered();
    void onEndOfSectionActTriggered();
    void onExitOfSectionsActTriggered();
    void onSettingsActTriggered();
    void onStartOrEndAutoBlockActTriggered();
    void onRemoveAutoMarkActTriggered();
    void onRunAutoBlockActTriggered();
    void onStopAutoBlockActTriggered();
    void onNshsIsdActTriggered();
    void onAvarIsdActTriggered();
    void onPriIsdActTriggered();
    void onNshsRepActTriggered();
    void onAvarRepActTriggered();
    void onPriRepActTriggered();
    void onListOfSectionsRepActTriggered();
    void onSectionsRepActTriggered();
    void onSecondSectionRepActTriggered();
    void onThirdSectionRepActTriggered();
    void onEndOfRepSectionActTriggered();
    void onExitOfRepSectionActTriggered();
private:
    ProgramViewWidget* exec_wgt_ = nullptr;
    QStringList loaded_sections_;
private:
    void createActions();
    void createMenu();
    void setupConnections();

    void rebuildSectionsMenu();
    void clearSectionsMenu();
signals:

};

#endif // MAINWINDOW_H
