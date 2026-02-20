#include "isrmainwindow.h"
#include <QMenu>
#include <QMenuBar>

ISRMainWindow::ISRMainWindow(std::shared_ptr<const AppConfig> cfg, QWidget* parent) :
    QMainWindow(parent),
    cfg_(std::move(cfg))
{
    QMenu *file_menu = new QMenu("Файл", this);
    QMenu *tools_menu = new QMenu("Инструменты", this);
    QMenu *sections_menu = new QMenu("Разделы", this);
    QMenu *pris_menu = new QMenu("PRIS", this);

    QAction *open_struct_act = file_menu->addAction("Открыть файл структуры...");
    open_struct_act->setShortcut(QKeySequence("CTRL+O"));
    QAction *settings_act = file_menu->addAction("Настройки...");
    QAction *open_folder_reports_act = file_menu->addAction("Открыть папку отчетов");
    QAction *exit_act = file_menu->addAction("Выход");

    QAction *catalog_command_act = tools_menu->addAction("Каталог команд...");

    QAction *load_section_SPO_act = sections_menu->addAction("Загрузить раздел из СПО...");
    QAction *rebuild_section_list_act = sections_menu->addAction("Перестроить список разделов (СПО)");
    sections_menu->addSeparator();
    QAction *end_cur_section_act = sections_menu->addAction("Завершить текущий раздел");
    sections_menu->addSeparator();
    QAction *stop_avt_act = sections_menu->addAction("Остановить АВТ (текущий раздел)");
    QAction *abort_avt_after_cur_command_act = sections_menu->addAction("Прервать АВТ после текущей операции");
    QAction *continue_avt_act = sections_menu->addAction("Продолжить АВТ");
    QAction *close_all_sections_act = sections_menu->addAction("Закрыть все разделы (локально)");

    QAction *pris_settings_act = pris_menu->addAction("Настройки...");
    QAction *connect_act = pris_menu->addAction("Подключить");
    QAction *disconnect_act = pris_menu->addAction("Отключить");
    QAction *send_test_package = pris_menu->addAction("Отправить тестовый пакет");

    menuBar()->addMenu(file_menu);
    menuBar()->addMenu(tools_menu);
    menuBar()->addMenu(sections_menu);
    menuBar()->addMenu(pris_menu);
}
