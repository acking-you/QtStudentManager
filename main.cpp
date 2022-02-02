#include "mainwindow.h"
#include "stusql.h"
#include <QApplication>
#include"mydialog.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    return a.exec();
}
