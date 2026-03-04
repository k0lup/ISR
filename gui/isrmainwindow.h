#ifndef ISRMAINWINDOW_H
#define ISRMAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QCloseEvent>
#include <memory>
#include <QListWidget>
#include "config/app_config.h"
#include "loadingoverlay.h"
#include "setReader/sectionsloader.h"

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
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    std::shared_ptr<const AppConfig> cfg_;
    LoadingOverlay* load_overlay_ = nullptr;
    SectionsLoader* sections_loader_ = nullptr;
    QListWidget* info_test_wgt_ = nullptr;

    bool readSections(const QStringList& sections_path, const QStringList& sections);
private slots:
    void beginStartup();
};

#endif // ISRMAINWINDOW_H
