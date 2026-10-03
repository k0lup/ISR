#include "application.h"
#include <QCoreApplication>
#include <QStringList>
#include <QTimer>

Application::Application(QObject* parent) : QObject(parent)
{
    controller_ = new Controller();
    gui_window_ = new MainWindow();
    prog_view_widget_ = gui_window_->getProgViewWgt();

    directive_executor_ = new DirectiveExecutor();
    catalog_manager_ = new CatalogManager();
    section_loader_ = new SectionsLoader();

    QTimer::singleShot(0, controller_, &Controller::start);
}
