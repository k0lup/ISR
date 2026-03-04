#ifndef ISRMAINWINDOW_H
#define ISRMAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QCloseEvent>
#include <memory>
#include <QListWidget>
#include <QThread>
#include "config/app_config.h"
#include "loadingoverlay.h"
#include "setReader/sectionsloader.h"
#include "menuWgt/sectionlistwgt.h"

class ISRMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    ISRMainWindow(std::shared_ptr<const AppConfig> cfg, QWidget *parent = nullptr);
    ~ISRMainWindow();
public slots:
    void showLoading(const QString& msg, bool indeterminate = false);
    void setLoadingMessage(const QString& msg);
    void setLoadingProgress(int percent);
    void hideLoading();
    void showErrorMessage(const QString& msg);

    void onSectionsListActTriggered();
    void setSelectedSection(const QString& section_name);
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    std::shared_ptr<const AppConfig> cfg_;
    LoadingOverlay* load_overlay_ = nullptr;
    SectionsLoader* sections_loader_ = nullptr;
    SectionListWgt* section_list_wgt_ = nullptr; //виджет пункт меню "выбрать раздел"

    QString active_section_name_;
    QThread* sections_thread_ = nullptr;

    bool readSections(const QStringList& sections_path, const QStringList& sections);
private slots:
    void beginStartup();
};

#endif // ISRMAINWINDOW_H
