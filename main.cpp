#include "mainwindow.h"

#include <QApplication>

#include <chrono>
#include <thread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    if (w.initFailed)
        return 0;
    w.show();
    return a.exec();
}
