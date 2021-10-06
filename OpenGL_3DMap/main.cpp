#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
#if(QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}