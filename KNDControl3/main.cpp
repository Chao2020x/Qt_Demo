#include "kndwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KNDWidget w;
    w.show();
    return a.exec();
}
