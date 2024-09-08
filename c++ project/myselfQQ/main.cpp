#include "widget.h"
#include <QApplication>
#include "drawer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //drawer类
    Drawer drawer;
    drawer.resize(250,700);
    drawer.show();

    return a.exec();
}
