#include "mainwindow.h"

#include "sectionlistdialog.h"
#include "stencildialog.h"
#include "headerdialog.h"
#include "../core/isrcontroller.h"

#include <QAction>
#include <QListView>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStringListModel>
#include <QStatusBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(IsrController *controller, QWidget *parent)
    : QMainWindow(parent)
    , m_controller(controller)
{
    auto *prepMenu = menuBar()->addMenu(QStringLiteral("Подготовка"));
    auto *sectionsAction = prepMenu->addAction(QStringLiteral("Список разделов..."));
    auto *stencilAction = prepMenu->addAction(QStringLiteral("Трафарет..."));
    auto *headerAction = prepMenu->addAction(QStringLiteral("Заголовок раздела..."));
    prepMenu->addAction(QStringLiteral("Загрузка структуры"));
    auto *exitAction = prepMenu->addAction(QStringLiteral("Выход"));

    connect(sectionsAction, &QAction::triggered, this, &MainWindow::openSectionDialog);
    connect(stencilAction, &QAction::triggered, this, &MainWindow::openStencilDialog);
    connect(headerAction, &QAction::triggered, this, &MainWindow::openHeaderDialog);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    m_tabWidget = new QTabWidget(central);

    const QStringList tabNames = {QStringLiteral("Штат"), QStringLiteral("НШС"), QStringLiteral("ПРИ")};
    for (const QString &tabName : tabNames) {
        auto *view = new QListView(m_tabWidget);
        view->setModel(new QStringListModel({}, view));
        m_tabWidget->addTab(view, tabName);
    }

    layout->addWidget(m_tabWidget);
    setCentralWidget(central);

    statusBar()->showMessage(QStringLiteral("ИСР - [раздел не выбран]"));
    setWindowTitle(QStringLiteral("ИСР - [раздел не выбран]"));

    connect(m_controller, &IsrController::statusChanged, this, [this](const QString &text) {
        setWindowTitle(text);
        statusBar()->showMessage(text);
    });
    connect(m_controller, &IsrController::switchSectionTab, this, &MainWindow::switchToSectionTab);
}

void MainWindow::openSectionDialog()
{
    SectionListDialog dialog(m_controller->sections(), this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString error;
    if (!m_controller->selectSection(dialog.selectedSection(), error)) {
        QMessageBox::warning(this, QStringLiteral("Ошибка"), error);
    }
}

void MainWindow::openStencilDialog()
{
    StencilDialog dialog(m_controller->stencil(), this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString error;
    if (!m_controller->setStencil(dialog.stencil(), error)) {
        QMessageBox::warning(this, QStringLiteral("Ошибка"), error);
    }
}

void MainWindow::openHeaderDialog()
{
    if (m_controller->stencil().isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Заголовок"), QStringLiteral("Сначала задайте трафарет"));
        return;
    }

    HeaderDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString error;
    if (!m_controller->setHeader(dialog.headerText(), error)) {
        QMessageBox::warning(this, QStringLiteral("Ошибка"), error);
    }
}

void MainWindow::switchToSectionTab(const QString &name)
{
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        if (m_tabWidget->tabText(i).compare(name, Qt::CaseInsensitive) == 0) {
            m_tabWidget->setCurrentIndex(i);
            return;
        }
    }
}
