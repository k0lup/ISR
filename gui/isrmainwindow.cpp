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

    preparation_menu_ = new QMenu("Подготовка", this);
    sections_menu_ = new QMenu("Разделы", this);
    segments_menu_ = new QMenu("Секции", this);
    work_menu_ = new QMenu("Работа", this);
    auto_menu_ = new QMenu("Авто", this);
    docs_menu_ = new QMenu("ЭД", this);
    sections_rep_menu_ = new QMenu("Разделы РЭП", this);
    segments_rep_menu_ = new QMenu("Секции РЭП", this);

    sections_list_action_ = preparation_menu_->addAction("Список разделов...");
    title_of_section_ = preparation_menu_->addAction("Заголовок раздела...");
    pfks_action_ = preparation_menu_->addAction("ПФКС...");
    control_spo_action_ = preparation_menu_->addAction("Контроль СПО...");
    load_structure_action_ = preparation_menu_->addAction("Загрузка структуры");
    preparation_menu_->addSeparator();
    debug_of_section_action_ = preparation_menu_->addAction("Отладка раздела...");
    preparation_menu_->addSeparator();
    add_sections_on_list_sections_action_ = preparation_menu_->addAction("Добавить разделы в список разделов...");
    rebuild_sections_list_action_ = preparation_menu_->addAction("Перестроить список разделов");
    stencil_action_ = preparation_menu_->addAction("Трафарет...");
    QMenu* danger_menu = preparation_menu_->addMenu("Нештат");
    danger_menu->addAction("Выход");

    regular_con_section_action_ = segments_menu_->addAction("Штатное проведение раздела");
    nshc_of_section_action_ = segments_menu_->addAction("НШС раздела");
    pri_of_section_action_ = segments_menu_->addAction("ПРИ раздела");

    run_operation_action_ = work_menu_->addAction("Выполнить операцию");
    cancel_var_action_ = work_menu_->addAction("Отменить вариант");
    sp_action_ = work_menu_->addAction("СП...");
    go_to_active_segmets_action_ = work_menu_->addAction("Перейти к активной сессии");
    imin_answer_pris_to_ko_action_ = work_menu_->addAction("Имитация ответа ПРИС на КО (ненорм.)");
    time_of_head_section_action_ = work_menu_->addAction("Продолжительность головного раздела...");
    QAction* move_window_to_position_action = work_menu_->addAction("Вернуть окно на место");
    work_menu_->addSeparator();
    end_section_action_ = work_menu_->addAction("Конец раздела...");
    exit_of_sections_action_ = work_menu_->addAction("Выход из разделов...");
    work_menu_->addSeparator();
    QAction* settings_action = work_menu_->addAction("Настройка");

    start_or_end_auto_label_action_ = auto_menu_->addAction("Начать/окончить отметку блока 'Авто'");
    start_or_end_auto_label_action_->setShortcut(QKeySequence(Qt::Key_F7));
    unset_auto_label_action_ = auto_menu_->addAction("Снять отметку блока 'Авто'");
    unset_auto_label_action_->setShortcut(QKeySequence(Qt::Key_F7));
    auto_menu_->addSeparator();
    run_auto_block_action_ = auto_menu_->addAction("Выполнить блок 'Авто'");
    stop_auto_block_action_ = auto_menu_->addAction("Остановить выполнение блока 'Авто'");

    QAction* nshc_of_product_action = docs_menu_->addAction("НШС изделия");
    QAction* avar_of_product_action = docs_menu_->addAction("АВАР изделия");
    QAction* pri_of_product_action = docs_menu_->addAction("ПРИ изделия");
    QAction* nshc_rep_action = docs_menu_->addAction("НШС РЭП");
    QAction* avar_pep_action = docs_menu_->addAction("АВАР РЭП");
    QAction* pri_rep_action = docs_menu_->addAction("ПРИ РЭП");

    QAction* list_of_sections_rep_action = sections_rep_menu_->addAction("Список разделов РЭП");

    QAction* section_rep_action = segments_rep_menu_->addAction("Раздел РЭП");
    QAction* sectond_section_action = segments_rep_menu_->addAction("Второй раздел");
    QAction* third_section_action = segments_rep_menu_->addAction("Третий раздел");
    QAction* end_of_section_action = segments_rep_menu_->addAction("Конец раздела");
    QAction* exit_of_the_sections_action = segments_rep_menu_->addAction("Выход из разделов");


    menuBar()->addMenu(preparation_menu_);
    menuBar()->addMenu(sections_menu_);
    menuBar()->addMenu(segments_menu_);
    menuBar()->addMenu(work_menu_);
    menuBar()->addMenu(auto_menu_);
    menuBar()->addMenu(docs_menu_);
    menuBar()->addMenu(sections_rep_menu_);
    menuBar()->addMenu(segments_rep_menu_);

    buildStateMachine();

    QObject::connect(sections_list_action_, &QAction::triggered, this, &ISRMainWindow::onSectionsListActTriggered);
    QObject::connect(title_of_section_, &QAction::triggered, this, &ISRMainWindow::onTitleOfSectionActTriggered);

    qCDebug(logCore) << QString("Завершена инициализация окна ISRMainWindow");
    machine_.start();
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

void ISRMainWindow::enterStartupLoading() {
    qCInfo(logCore) << "Машана состояний переводится в st_startup_loading_";

    /*sections_menu_->setEnabled(false);
    preparation_menu_->setEnabled(false);
    segments_menu_->setEnabled(false);
    work_menu_->setEnabled(false);
    auto_menu_->setEnabled(false);
    docs_menu_->setEnabled(false);
    sections_rep_menu_->setEnabled(false);
    segments_rep_menu_->setEnabled(false);*/
    this->setEnabled(false);

    beginStartup();

    qCInfo(logCore) << "Машана состояний переведена в st_startup_loading_";
}

void ISRMainWindow::enterIdleNoSection() {
    qCInfo(logCore) << "Машана состояний переводится в st_idle_no_section_";
    this->setEnabled(true);
    /*
    sections_menu_->                    setEnabled(true);
    preparation_menu_->                 setEnabled(true);
    segments_menu_->                    setEnabled(true);
    work_menu_->                        setEnabled(true);
    auto_menu_->                        setEnabled(true);
    docs_menu_->                        setEnabled(true);
    sections_rep_menu_->                setEnabled(true);
    segments_rep_menu_->                setEnabled(true);
    */

    sections_list_action_->                 setEnabled(true);
    title_of_section_->                     setEnabled(false);
    pfks_action_->                          setEnabled(false);
    control_spo_action_->                   setEnabled(false);
    load_structure_action_->                setEnabled(false);
    debug_of_section_action_->              setEnabled(true);
    add_sections_on_list_sections_action_-> setEnabled(true);
    rebuild_sections_list_action_->         setEnabled(true);
    stencil_action_->                       setEnabled(true);

    regular_con_section_action_->           setEnabled(false);
    nshc_of_section_action_->               setEnabled(false);
    pri_of_section_action_->                setEnabled(false);

    run_operation_action_->                 setEnabled(false);
    cancel_var_action_->                    setEnabled(false);
    sp_action_->                            setEnabled(false);
    go_to_active_segmets_action_->          setEnabled(false);
    imin_answer_pris_to_ko_action_->        setEnabled(false);
    time_of_head_section_action_->          setEnabled(false);
    end_section_action_->                   setEnabled(false);
    exit_of_sections_action_->              setEnabled(false);

    start_or_end_auto_label_action_->       setEnabled(false);
    unset_auto_label_action_->              setEnabled(false);
    run_auto_block_action_->                setEnabled(false);
    stop_auto_block_action_->               setEnabled(false);

    sections_menu_->clear();
    sections_menu_->addAction("< Разделов нет >");

    qCInfo(logCore) << "Машана состояний переведена в st_idle_no_section_";
}

void ISRMainWindow::enterSectionSelected() {
    qCInfo(logCore) << "Машана состояний переводится в st_section_selected_";

    sections_list_action_->                 setEnabled(true);
    title_of_section_->                     setEnabled(true);
    pfks_action_->                          setEnabled(false);
    control_spo_action_->                   setEnabled(false);
    load_structure_action_->                setEnabled(false);
    debug_of_section_action_->              setEnabled(true);
    add_sections_on_list_sections_action_-> setEnabled(true);
    rebuild_sections_list_action_->         setEnabled(true);
    stencil_action_->                       setEnabled(true);

    regular_con_section_action_->           setEnabled(false);
    nshc_of_section_action_->               setEnabled(false);
    pri_of_section_action_->                setEnabled(false);

    run_operation_action_->                 setEnabled(false);
    cancel_var_action_->                    setEnabled(false);
    sp_action_->                            setEnabled(false);
    go_to_active_segmets_action_->          setEnabled(false);
    imin_answer_pris_to_ko_action_->        setEnabled(false);
    time_of_head_section_action_->          setEnabled(false);
    end_section_action_->                   setEnabled(false);
    exit_of_sections_action_->              setEnabled(false);

    start_or_end_auto_label_action_->       setEnabled(false);
    unset_auto_label_action_->              setEnabled(false);
    run_auto_block_action_->                setEnabled(false);
    stop_auto_block_action_->               setEnabled(false);

    setWindowTitle(QString("ИСР - [%1]").arg(active_section_name_));

    qCInfo(logCore) << "Машана состояний переведена в st_section_selected_";
}

void ISRMainWindow::enterSectionTitleReady() {
    qCInfo(logCore) << "Машана состояний переводится в st_title_ready_";

    setWindowTitle(QString("ИСР - [%1, файл структуры не загружен]").arg(active_section_name_));

    sections_list_action_->                 setEnabled(true);
    title_of_section_->                     setEnabled(false);
    pfks_action_->                          setEnabled(true);
    control_spo_action_->                   setEnabled(true);
    load_structure_action_->                setEnabled(true);
    debug_of_section_action_->              setEnabled(false);
    add_sections_on_list_sections_action_-> setEnabled(false);
    rebuild_sections_list_action_->         setEnabled(false);
    stencil_action_->                       setEnabled(false);

    regular_con_section_action_->           setEnabled(false);
    nshc_of_section_action_->               setEnabled(false);
    pri_of_section_action_->                setEnabled(false);

    run_operation_action_->                 setEnabled(false);
    cancel_var_action_->                    setEnabled(false);
    sp_action_->                            setEnabled(true);
    go_to_active_segmets_action_->          setEnabled(false);
    imin_answer_pris_to_ko_action_->        setEnabled(false);
    time_of_head_section_action_->          setEnabled(true);
    end_section_action_->                   setEnabled(true);
    exit_of_sections_action_->              setEnabled(true);

    start_or_end_auto_label_action_->       setEnabled(false);
    unset_auto_label_action_->              setEnabled(false);
    run_auto_block_action_->                setEnabled(false);
    stop_auto_block_action_->               setEnabled(false);

    time_after_start_head_section_.start();


    qCInfo(logCore) << "Машана состояний переведена в st_title_ready_";
}

void ISRMainWindow::enterStructureReady() {
    qCInfo(logCore) << "Машана состояний переводится в st_structure_ready_";



    qCInfo(logCore) << "Машана состояний переведена в st_structure_ready_";
}

void ISRMainWindow::enterErrorDetected() {
    qCInfo(logCore) << "Машана состояний переводится в st_error_detected_";

    this->setEnabled(false);
    QString message = "При работе ИСР произошла серьезная ошибка! Программа должна быть завершена!";
    QMessageBox::critical(nullptr, "Ошибка", message);
    qCWarning(logCore) << message;

    qCInfo(logCore) << "Машана состояний переведена в st_error_detected_";
}

void ISRMainWindow::beginStartup() {
    if (sections_thread_ != nullptr) {
        QString message = "Попытка двойной инициализации приложения!";
        qCCritical(logCore) << message;
        QMessageBox::critical(nullptr, "Ошибка", message);
        emit errorDetected();
        return;
    }

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
    connect(sections_loader_, &SectionsLoader::failed, this, [this](const QString& msg){
        hideLoading();
        showErrorMessage(msg);
        emit errorDetected();
    });
    connect(sections_loader_, &SectionsLoader::failed, sections_thread_, &QThread::quit);

    // 8) после finished забрать результат (важно: в UI-потоке)
    connect(sections_loader_, &SectionsLoader::finished, this, [this](){
        QStringList list = sections_loader_->getSectionsNames().values();
        list.sort();
        section_list_wgt_->setSections(list);

        emit startupCompleted();
    });

    // 9) аккуратная очистка объектов
    QObject::connect(sections_thread_, &QThread::finished, sections_loader_, &QObject::deleteLater);

    QObject::connect(sections_thread_, &QThread::finished, sections_thread_, &QObject::deleteLater);

    QObject::connect(sections_thread_, &QThread::finished, this, [this]() {
        sections_loader_ = nullptr;
        sections_thread_ = nullptr;
    });

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
    if (section_list_wgt_->exec(mode_sections_list_wgt_) == QDialog::Accepted) {
        SectionSelected section = section_list_wgt_->getSection();
        switch (section.mode) {
        case MODE_SECTION_LIST_WGT::FOR_SELECT:
            setSelectedSection(section.section_name);
            break;
        case MODE_SECTION_LIST_WGT::FOR_LOAD:
            loadSelectedSection(section.section_name);
            break;
        default:
            QString message = "Получен раздел для выбора/загрузки в неизвестном режиме!";
            qCCritical(logCore) << message;
            QMessageBox::critical(nullptr, "Ошибка", message);
            emit errorDetected();
            break;
        }

    } else {
        qCInfo(logCore) << QString("Было открыто окно со списком разделов, но раздел не был выбран");
    }
}

void ISRMainWindow::setSelectedSection(const QString& section_name) {
    if (!section_name.isEmpty()) {
        active_section_name_ = section_name;
        qCInfo(logCore) << QString("Был выбран раздел '%1'").arg(section_name);
        emit sectionChosen();
    } else {
        QString message("В качестве имени выбранного раздела была получена пустая строка");
        qCWarning(logCore) << message;
        QMessageBox::warning(nullptr, "Предупреждение", message);
    }
}

void ISRMainWindow::loadSelectedSection(const QString& section_name) {
    if (!section_name.isEmpty()) {
        qCInfo(logCore) << QString("Запрошена загрузка раздела '%1'").arg(section_name);

        //загрузки структуры

        emit structureLoaded();
    }
}

void ISRMainWindow::onTitleOfSectionActTriggered() {
    if (active_section_name_.isEmpty()) {
        QString message = "Имя выбранного раздела пустое! Открытие окна 'Заголовок раздела' недопустимо";
        QMessageBox::warning(nullptr, "Предупреждение", message);
        qCWarning(logCore) << message;
        return;
    }
    if (title_section_wgt_->exec(active_section_name_) == QDialog::Accepted) {
        qCInfo(logCore) << "Получен заголовок раздела";
        emit titleConfirmed();
    }
}

void ISRMainWindow::buildStateMachine() {
    st_startup_loading_ = new QState(&machine_);
    st_idle_no_section_ = new QState(&machine_);
    st_section_selected_ = new QState(&machine_);
    st_title_ready_ = new QState(&machine_);
    st_structure_ready_ = new QState(&machine_);
    st_error_detected_ = new QState(&machine_);

    machine_.setInitialState(st_startup_loading_);

    // Переходы
    st_startup_loading_->addTransition(this, &ISRMainWindow::startupCompleted, st_idle_no_section_);

    st_idle_no_section_->addTransition(this, &ISRMainWindow::sectionChosen, st_section_selected_);

    st_section_selected_->addTransition(this, &ISRMainWindow::titleConfirmed, st_title_ready_);
    st_section_selected_->addTransition(this, &ISRMainWindow::sectionChosen, st_section_selected_);

    st_title_ready_->addTransition(this, &ISRMainWindow::structureLoaded, st_structure_ready_);
    st_title_ready_->addTransition(this, &ISRMainWindow::sectionsClosed, st_idle_no_section_);

    st_structure_ready_->addTransition(this, &ISRMainWindow::sectionsClosed, st_idle_no_section_);

    st_startup_loading_->addTransition(this, &ISRMainWindow::errorDetected, st_error_detected_);
    st_idle_no_section_->addTransition(this, &ISRMainWindow::errorDetected, st_error_detected_);
    st_section_selected_->addTransition(this, &ISRMainWindow::errorDetected, st_error_detected_);
    st_title_ready_->addTransition(this, &ISRMainWindow::errorDetected, st_error_detected_);
    st_structure_ready_->addTransition(this, &ISRMainWindow::errorDetected, st_error_detected_);
    st_error_detected_->addTransition(this, &ISRMainWindow::errorDetected, st_error_detected_);

    st_error_detected_->addTransition(this, &ISRMainWindow::errorResetRequested, st_startup_loading_);


    // Реакция на вход
    QObject::connect(st_startup_loading_, &QState::entered, this, &ISRMainWindow::enterStartupLoading);
    QObject::connect(st_idle_no_section_, &QState::entered, this, &ISRMainWindow::enterIdleNoSection);
    QObject::connect(st_section_selected_, &QState::entered, this, &ISRMainWindow::enterSectionSelected);
    QObject::connect(st_title_ready_, &QState::entered, this, &ISRMainWindow::enterSectionTitleReady);
    QObject::connect(st_structure_ready_, &QState::entered, this, &ISRMainWindow::enterStructureReady);
    QObject::connect(st_error_detected_, &QState::entered, this, &ISRMainWindow::enterErrorDetected);
}

ISRMainWindow::~ISRMainWindow() {
    if (sections_loader_ != nullptr) {
        sections_loader_->cancel();
    }
    if (sections_thread_ != nullptr) {
        sections_thread_->quit();
        sections_thread_->wait();
    }
}
