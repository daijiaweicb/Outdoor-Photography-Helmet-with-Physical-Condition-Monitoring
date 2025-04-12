#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<SystemMode>("SystemMode");
    MainWindow w;
    w.show();
    return a.exec();
}
