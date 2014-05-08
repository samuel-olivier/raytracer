#include <QApplication>
#include <QStyleFactory>
#include <QDebug>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    a.setStyle("Fusion");
//    qDebug() << QStyleFactory::keys();

    return a.exec();
}
