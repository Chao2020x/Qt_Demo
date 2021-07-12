#include "inc/bpnetworkqt.h"
#include "inc/mainwindow.h"
#include <QMessageBox>


BPNetworkQt::BPNetworkQt(MainWindow *window, int in, int out):BPNetwork (in,out),mWindow(window)
{

}

BPNetworkQt::~BPNetworkQt()
{

}

void BPNetworkQt::exception(std::string ex)
{
    if (ex == "Error sample!")
    {
        emit sigException(QString("样本数量不足，请修改样本数量"));
    }
}
