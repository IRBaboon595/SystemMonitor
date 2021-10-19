#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QStyle * style = QStyleFactory::create("Cleanlooks");
    a.setStyle(style);
    w.show();
    return a.exec();
}
