#include "isrmainwindow.h"
#include <QMenu>
#include <QMenuBar>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QApplication>
#include "logger/logging_categories.h"
#include <QLoggingCategory>

ISRMainWindow::ISRMainWindow(std::shared_ptr<const AppConfig> cfg, QWidget* parent) :
    QMainWindow(parent),
    cfg_(std::move(cfg))
{
    qCDebug(logCore) << QString("Инициализация окна ISRMainWindow");

    QMenu* preparation_menu = new QMenu("Подготовка", this);
    QMenu* sections_menu = new QMenu("Разделы", this);
    QMenu* segments_menu = new QMenu("Секции", this);
    QMenu* work_menu = new QMenu("Работа", this);
    QMenu* auto_menu = new QMenu("Авто", this);
    QMenu* docs_menu = new QMenu("ЭД", this);
    QMenu* sections_rep_menu = new QMenu("Разделы РЭП", this);
    QMenu* segments_rep_menu = new QMenu("Секции РЭП", this);

    QAction* sections_list_action = preparation_menu->addAction("Список разделов...");
    QAction* title_of_section = preparation_menu->addAction("Заголовок раздела...");
    QAction* pfks_action = preparation_menu->addAction("ПФКС...");
    QAction* control_spo_action = preparation_menu->addAction("Контроль СПО...");
    QAction* load_structure_action = preparation_menu->addAction("Загрузка структуры");
    preparation_menu->addSeparator();
    QAction* debug_of_section_action = preparation_menu->addAction("Отладка раздела...");
    preparation_menu->addSeparator();
    QAction* add_sections_on_list_sections_action = preparation_menu->addAction("Добавить разделы в список разделов...");
    QAction* rebuild_sections_list_action = preparation_menu->addAction("Перестроить список разделов");
    QAction* stencil_action = preparation_menu->addAction("Трафарет...");
    QMenu* danger_menu = preparation_menu->addMenu("Нештат");
    danger_menu->addAction("Выход");

    sections_menu->addAction("< Разделов нет >");

    QAction* regular_con_section_action = segments_menu->addAction("Штатное проведение раздела");
    QAction* nshc_of_section_action = segments_menu->addAction("НШС раздела");
    QAction* pri_of_section_action = segments_menu->addAction("ПРИ раздела");

    QAction* run_operation_action = work_menu->addAction("Выполнить операцию");
    QAction* cancel_var_action = work_menu->addAction("Отменить вариант");
    QAction* sp_action = work_menu->addAction("СП...");
    QAction* go_to_active_segmets_action = work_menu->addAction("Перейти к активной сессии");
    QAction* imin_answer_pris_to_ko_action = work_menu->addAction("Имитация ответа ПРИС на КО (ненорм.)");
    QAction* time_of_head_section_action = work_menu->addAction("Продолжительность головного раздела...");
    QAction* move_window_to_position_action = work_menu->addAction("Вернуть окно на место");
    work_menu->addSeparator();
    QAction* end_section_action = work_menu->addAction("Конец раздела...");
    QAction* exit_of_sections_action = work_menu->addAction("Выход из разделов...");
    work_menu->addSeparator();
    QAction* settings_action = work_menu->addAction("Настройка");

    QAction* start_or_end_auto_label_action = auto_menu->addAction("Начать/окончить отметку блока 'Авто'");
    start_or_end_auto_label_action->setShortcut(QKeySequence(Qt::Key_F7));
    QAction* unset_auto_label_action = auto_menu->addAction("Снять отметку блока 'Авто'");
    unset_auto_label_action->setShortcut(QKeySequence(Qt::Key_F7));
    auto_menu->addSeparator();
    QAction* run_auto_block_action = auto_menu->addAction("Выполнить блок 'Авто'");
    QAction* stop_auto_block_action = auto_menu->addAction("Остановить выполнение блока 'Авто'");

    QAction* nshc_of_product_action = docs_menu->addAction("НШС изделия");
    QAction* avar_of_product_action = docs_menu->addAction("АВАР изделия");
    QAction* pri_of_product_action = docs_menu->addAction("ПРИ изделия");
    QAction* nshc_rep_action = docs_menu->addAction("НШС РЭП");
    QAction* avar_pep_action = docs_menu->addAction("АВАР РЭП");
    QAction* pri_rep_action = docs_menu->addAction("ПРИ РЭП");

    QAction* list_of_sections_rep_action = sections_rep_menu->addAction("Список разделов РЭП");

    QAction* section_rep_action = segments_rep_menu->addAction("Раздел РЭП");
    QAction* sectond_section_action = segments_rep_menu->addAction("Второй раздел");
    QAction* third_section_action = segments_rep_menu->addAction("Третий раздел");
    QAction* end_of_section_action = segments_rep_menu->addAction("Конец раздела");
    QAction* exit_of_the_sections_action = segments_rep_menu->addAction("Выход из разделов");


    menuBar()->addMenu(preparation_menu);
    menuBar()->addMenu(sections_menu);
    menuBar()->addMenu(segments_menu);
    menuBar()->addMenu(work_menu);
    menuBar()->addMenu(auto_menu);
    menuBar()->addMenu(docs_menu);
    menuBar()->addMenu(sections_rep_menu);
    menuBar()->addMenu(segments_rep_menu);

    qCDebug(logCore) << QString("Завершена инициализация окна ISRMainWindow");
}

void ISRMainWindow::closeEvent(QCloseEvent *event) {
    event->accept();


    QCoreApplication::quit();
}

ISRMainWindow::~ISRMainWindow() {

}
