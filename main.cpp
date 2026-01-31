#include "datamodel.h"
#include "datasource.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DataSource *dataSource = new DataSource(&app);
    DataModel *dataModel = new DataModel(dataSource, &app);

    MainWindow mainWindow (dataModel);
    mainWindow.show();
    return app.exec();
}
