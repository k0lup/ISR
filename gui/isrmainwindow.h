#ifndef ISRMAINWINDOW_H
#define ISRMAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QCloseEvent>
#include <memory>
#include "config/app_config.h"

class ISRMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    ISRMainWindow(std::shared_ptr<const AppConfig> cfg, QWidget *parent = nullptr);
    ~ISRMainWindow();
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    std::shared_ptr<const AppConfig> cfg_;
};

#endif // ISRMAINWINDOW_H
