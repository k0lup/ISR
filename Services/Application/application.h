#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>


#include "Services/GUI/mainwindow.h"
#include "Services/GUI/ProgViewWgt/programviewwidget.h"
#include "Services/Controller/controller.h"
#include "Services/DirectiveExecutor/directiveexecutor.h"
#include "Services/FilesReaders/sectionsloader.h"
#include "Services/CatalogManager/catalogmanager.h"
#include "Services/ConfigReader/configtypes.h"

class Application : public QObject
{
    Q_OBJECT
public:
    explicit Application(QObject* parent = nullptr);
    bool isInicialized() const;
    ~Application();
signals:
    void startRequested();
private:
    void setupConnections();

    void createDeferredError(const QString& message);
    bool readConfig();
private:
    MainWindow* gui_window_ = nullptr;
    ProgramViewWidget* prog_view_widget_ = nullptr;          //Получаем из MainWindow();
    Controller* controller_ = nullptr;
    DirectiveExecutor* directive_executor_ = nullptr;
    SectionsLoader* section_loader_ = nullptr;
    CatalogManager* catalog_manager_ = nullptr;

    AppConfig app_cfg_;

    bool is_inicialized_ = false;
};

#endif // APPLICATION_H
