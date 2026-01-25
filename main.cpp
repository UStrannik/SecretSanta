#include "secretsanta.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SecretSanta win;
    win.show();
    return app.exec();
}
