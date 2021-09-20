#include "qt_gui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qt_gui w;
    w.show();
    return a.exec();
}
