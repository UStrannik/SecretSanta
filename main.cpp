#include "datasource.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DataSource *dataSource = new DataSource(&app);

    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
