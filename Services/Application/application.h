#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>


#include "Services/GUI/mainwindow.h"
#include "Services/GUI/ProgViewWgt/programviewwidget.h"
#include "Services/Controller/controller.h"
#include "Services/DirectiveExecutor/directiveexecutor.h"
#include "Services/ConfigReader/configreader.h"
#include "Services/FilesReaders/sectionsloader.h"
#include "Services/CatalogManager/catalogmanager.h"

class Application : public QObject
{
    Q_OBJECT
public:
    explicit Application(QObject* parent = nullptr);
signals:
    void startRequested();
private:
    void createDeferredError(const QString& message);
private:
    MainWindow* gui_window_ = nullptr;
    ProgramViewWidget* prog_view_widget_ = nullptr;          //Получаем из MainWindow();
    Controller* controller_ = nullptr;
    DirectiveExecutor* directive_executor_ = nullptr;
    ConfigReader* config_reader_ = nullptr;
    SectionsLoader* section_loader_ = nullptr;
    CatalogManager* catalog_manager_ = nullptr;
};

#endif // APPLICATION_H
