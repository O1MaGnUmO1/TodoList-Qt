#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet("QMainWindow{background-color: lightgrey;border: 1px solid black;}");
    MainWindow w;
    w.setWindowTitle("Todo List");
    w.show();
    return a.exec();
}
