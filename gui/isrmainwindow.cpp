#include "isrmainwindow.h"
#include <QMenu>
#include <QMenuBar>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QApplication>
#include "logger/logging_categories.h"
#include <QLoggingCategory>
#include <QTimer>
#include <QMessageBox>
#include <QStringList>


/*static QString expandUserPath(const QString &path)
{
    if (path.startsWith("~"))
    {
        QString home = QDir::homePath();
        QString newPath = path;
        newPath.replace(0, 1, home);
        return QDir::cleanPath(newPath);
    }

    return QDir::cleanPath(path);
}*/

ISRMainWindow::ISRMainWindow(std::shared_ptr<const AppConfig> cfg, QWidget* parent) :
    QMainWindow(parent),
    cfg_(std::move(cfg))
{
    qCDebug(logCore) << QString("Инициализация окна ISRMainWindow");

    load_overlay_ = new LoadingOverlay(this);
    //sections_loader_ = new SectionsLoader(cfg_, this);
    section_list_wgt_ = new SectionListWgt(this);
    title_section_wgt_ = new TitleSectionWgt(this);

    setWindowTitle("ИСР - [раздел не выбран]");

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

    QObject::connect(sections_list_action, &QAction::triggered, this, &ISRMainWindow::onSectionsListActTriggered);
    QObject::connect(title_of_section, &QAction::triggered, this, &ISRMainWindow::onTitleOfSectionActTriggered);

    qCDebug(logCore) << QString("Завершена инициализация окна ISRMainWindow");

    QTimer::singleShot(0, this, &ISRMainWindow::beginStartup);
}

void ISRMainWindow::closeEvent(QCloseEvent *event) {
    event->accept();


    QCoreApplication::quit();
}



void ISRMainWindow::showLoading(const QString& msg, bool indeterminate)
{
    load_overlay_->setIndeterminate(indeterminate);
    load_overlay_->setMessage(msg);
    load_overlay_->raise();
    load_overlay_->show();
    load_overlay_->setFocus();
}

void ISRMainWindow::beginStartup() {
    // 1) UI overlay
        showLoading("Загрузка списка разделов...", true);

        // 2) поток
        sections_thread_ = new QThread(this);

        // 3) воркер (без parent = this, чтобы не привязать к UI-потоку)
        sections_loader_ = new SectionsLoader(cfg_, nullptr);
        sections_loader_->moveToThread(sections_thread_);

        // 4) запуск работы
        connect(sections_thread_, &QThread::started, sections_loader_, &SectionsLoader::start);

        // 5) сигналы -> UI
        connect(sections_loader_, &SectionsLoader::progress, this, &ISRMainWindow::setLoadingProgress);
        connect(sections_loader_, &SectionsLoader::message,  this, &ISRMainWindow::setLoadingMessage);

        // 6) завершение: скрыть overlay + остановить поток
        connect(sections_loader_, &SectionsLoader::finished, this, &ISRMainWindow::hideLoading);
        connect(sections_loader_, &SectionsLoader::finished, sections_thread_, &QThread::quit);

        // 7) ошибка: скрыть overlay + показать ошибку + остановить поток
        connect(sections_loader_, &SectionsLoader::errorMessage, this, [this](const QString& msg){
            hideLoading();
            showErrorMessage(msg);
        });
        connect(sections_loader_, &SectionsLoader::errorMessage, sections_thread_, &QThread::quit);

        // 8) после finished забрать результат (важно: в UI-потоке)
        connect(sections_loader_, &SectionsLoader::finished, this, [this](){
            QStringList list = sections_loader_->getSectionsNames().values();
            list.sort();
            section_list_wgt_->setSections(list);
        });

        // 9) аккуратная очистка объектов
        connect(sections_thread_, &QThread::finished, sections_loader_, &QObject::deleteLater);
        connect(sections_thread_, &QThread::finished, sections_thread_, &QObject::deleteLater);

        // 10) старт
        sections_thread_->start();
}

void ISRMainWindow::setLoadingMessage(const QString& msg)
{
    if (load_overlay_->isVisible()) load_overlay_->setMessage(msg);
}

void ISRMainWindow::setLoadingProgress(int percent)
{
    if (load_overlay_->isVisible()) load_overlay_->setProgress(percent);
}

void ISRMainWindow::hideLoading()
{
    load_overlay_->hide();
}

void ISRMainWindow::showErrorMessage(const QString &msg)
{
    qCWarning(logCore) << QString("Был запрошен вывод сообщения об ошибке: %1").arg(msg);
    QMessageBox::critical(nullptr, "Ошибка!", msg);
}

void ISRMainWindow::onSectionsListActTriggered() {
    if (section_list_wgt_->exec() == QDialog::Accepted) {
        setSelectedSection(section_list_wgt_->getSelectedSection());
        //setWindowTitle(QString("ИСР - [%1]").arg(active_section_name_));
    } else {
        qCInfo(logCore) << QString("Было открыто окно со списком разделов, но раздел не был выбран");
    }
}

void ISRMainWindow::setSelectedSection(const QString& section_name) {
    if (!section_name.isEmpty()) {
        active_section_name_ = section_name;
        setWindowTitle(QString("ИСР - [%1]").arg(active_section_name_));
        qCInfo(logCore) << QString("Был выбран раздел '%1'").arg(section_name);
    } else {
        QString message("В качестве имени выбранного раздела была получена пустая строка");
        qCWarning(logCore) << message;
        QMessageBox::warning(nullptr, "Предупреждение", message);
    }
}

void ISRMainWindow::onTitleOfSectionActTriggered() {
    if (active_section_name_.isEmpty()) {
        QString message = "Имя выбранного раздела пустое! Открытие окна 'Заголовок раздела' недопустимо";
        QMessageBox::warning(nullptr, "Предупреждение", message);
        qCWarning(logCore) << message;
    }
    if (title_section_wgt_->exec(active_section_name_) == QDialog::Accepted) {
        qCInfo(logCore) << "Получен заголовок раздела";
        setWindowTitle(QString("ИСР - [%1, файл структуры не загружен]").arg(active_section_name_));
    }
}

ISRMainWindow::~ISRMainWindow() {

}
