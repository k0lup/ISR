#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTabWidget;
class QListView;
class IsrController;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(IsrController *controller, QWidget *parent = nullptr);

private slots:
    void openSectionDialog();
    void openStencilDialog();
    void openHeaderDialog();
    void switchToSectionTab(const QString &name);

private:
    IsrController *m_controller = nullptr;
    QTabWidget *m_tabWidget = nullptr;
};

#endif // MAINWINDOW_H
