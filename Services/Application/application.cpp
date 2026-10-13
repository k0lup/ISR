#include "application.h"
#include <QCoreApplication>
#include <QStringList>
#include <QTimer>
#include <QMessageBox>
#include "Services/ConfigReader/configreader.h"

Application::Application(QObject* parent) : QObject(parent)
{
    if (!readConfig()) {
        return;
    }

    controller_ = new Controller(app_cfg_);
    gui_window_ = new MainWindow(app_cfg_);
    prog_view_widget_ = gui_window_->getProgViewWgt();

    directive_executor_ = new DirectiveExecutor(app_cfg_);
    catalog_manager_ = new CatalogManager(app_cfg_);
    section_loader_ = new SectionsLoader(app_cfg_);

    setupConnections();

    gui_window_->show();

    //QTimer::singleShot(0, controller_, &Controller::start);
    QTimer::singleShot(0, section_loader_, &SectionsLoader::start);
    is_inicialized_ = true;
}

bool Application::isInicialized() const
{
    return is_inicialized_;
}

bool Application::readConfig()
{
    QStringList args = QCoreApplication::arguments();

    if (args.size() < 3) {
        createDeferredError("В ISR должно быть передано не менее 2 аргументов");
        return false;
    }

    QString cfg_path = args[1];
    QString on_path = args[2];
    QString on284_path = (args.count() >= 4) ? args[3] : "";

    if (cfg_path.isEmpty() || on_path.isEmpty()) {
        createDeferredError("Пути к файлу конфигурации и файлу настройки не могут быть пустыми!");
        return false;
    }

    ConfigReader config_reader;
    if (!config_reader.readConfig(cfg_path, on_path, on284_path)) {
        createDeferredError("Возникли ошибки при разборе входных файлов:\n" + config_reader.getErrors());
        return false;
    }

    app_cfg_ = config_reader.getConfig();
    return true;
}

void Application::createDeferredError(const QString& message) {
    QMessageBox::critical(nullptr, "Ошибка", message);
    //ЗАКРЫВАТЬ ПРИЛОЖЕНИЕ ПО ЗАКРЫТИЮ CRITICAL
}

void Application::setupConnections()
{
    QObject::connect(gui_window_, &MainWindow::loadSectionRequested, section_loader_, &SectionsLoader::onLoadSectionRequested);
    QObject::connect(gui_window_, &MainWindow::sendWindowResponse, directive_executor_, &DirectiveExecutor::onWindowResponse);
    QObject::connect(gui_window_, &MainWindow::requestChangeSection, directive_executor_, &DirectiveExecutor::onSetLoadedSectionRequested);
    QObject::connect(gui_window_, &MainWindow::setNewTraf, this, [this](const QString& traf){
       app_cfg_.traf = traf;
    });

    QObject::connect(prog_view_widget_, &ProgramViewWidget::directiveDoubleClicked, directive_executor_, &DirectiveExecutor::startFrom);

    QObject::connect(section_loader_, &SectionsLoader::message, gui_window_, &MainWindow::onShowInfoRequested);
    QObject::connect(section_loader_, &SectionsLoader::failed, gui_window_, &MainWindow::onShowCriticalRequested);
    //Нужно отдельно отработать finished в Controller для окончания загрузки

    QObject::connect(section_loader_, &SectionsLoader::requestStatusSection, directive_executor_, &DirectiveExecutor::onStatusSectionsRequested);
    QObject::connect(section_loader_, &SectionsLoader::sectionLoaded, directive_executor_, &DirectiveExecutor::setSection);
    QObject::connect(section_loader_, &SectionsLoader::failed, directive_executor_, &DirectiveExecutor::onLoadSectionFailed); //Пока там, потом обратным вызовом сделаем, когда реализуем RequestManager

    QObject::connect(directive_executor_, &DirectiveExecutor::sendStatusSection, section_loader_, &SectionsLoader::onStatusSectionReceived);
    QObject::connect(directive_executor_, &DirectiveExecutor::sectionChanged, gui_window_, &MainWindow::onSectionChanged);
    QObject::connect(directive_executor_, &DirectiveExecutor::requestFailed, this, [this](const RequestId& source_request_id, const QString& message) {
        Q_UNUSED(source_request_id);
        gui_window_->onShowWarningRequested("Не удалось выполнить запрос:\n" + message);
    });

    QObject::connect(directive_executor_, &DirectiveExecutor::warning, gui_window_, &MainWindow::onShowWarningRequested);
    QObject::connect(directive_executor_, &DirectiveExecutor::failed, gui_window_, &MainWindow::onShowCriticalRequested);
    QObject::connect(directive_executor_, &DirectiveExecutor::requestShowWindow, gui_window_, &MainWindow::onRequestShowWindow);
    QObject::connect(directive_executor_, &DirectiveExecutor::requestLoadSection, section_loader_, &SectionsLoader::onExecutorRequestLoadSection);
    //PRIS пока затычка
    QObject::connect(directive_executor_, &DirectiveExecutor::requestSendDataToPris, this, [this](const PRISMessage& message, const RequestId request_id){
        Q_UNUSED(message);

        PRISResultData result;
        result.status = true;
        directive_executor_->onPRISResponse(result, request_id);
    });
    //Пока так, а дальше надо бы request_id гасить по получению ответа
    QObject::connect(directive_executor_, &DirectiveExecutor::sendSectionForView, gui_window_, &MainWindow::onSectionChanged);

    QObject::connect(section_loader_, &SectionsLoader::finished, this, [this](){
        QSet<QString> section_names = section_loader_->getSectionsNames();
        gui_window_->onListSectionLoaded(section_names);
    });


    QObject::connect(gui_window_, &MainWindow::destroyed, this, &Application::deleteLater);
}

Application::~Application()
{
    if (gui_window_) {
        gui_window_->deleteLater();
    }
    if (prog_view_widget_) {
        prog_view_widget_->deleteLater();
    }
    if (controller_) {
        controller_->deleteLater();
    }
    if (directive_executor_) {
        directive_executor_->deleteLater();
    }
    if (section_loader_) {
        section_loader_->deleteLater();
    }
    if (catalog_manager_) {
        catalog_manager_->deleteLater();
    }
}
