#include <QApplication>
#include <QMessageBox>

#include "src/core/isrcontroller.h"
#include "src/ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    IsrController controller;
    QString error;
    if (!controller.initialize(app.arguments(), error)) {
        QMessageBox::critical(nullptr, QStringLiteral("Ошибка запуска ИСР"), error);
        return 1;
    }

    MainWindow window(&controller);
    window.resize(900, 600);
    window.show();

    return app.exec();
}
