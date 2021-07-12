#ifndef BPNETWORKQT_H
#define BPNETWORKQT_H

#include "bpnetwork.h"
#include <QObject>
class MainWindow;
class BPNetworkQt:public QObject,public BPNetwork
{
    Q_OBJECT
signals:
    void sigException(QString s);
public:
    BPNetworkQt(MainWindow* window,int in,int out);
    ~BPNetworkQt();

    // BPNetwork interface
private:
    void exception(std::string ex="Default Exception");
    MainWindow* mWindow;

};

#endif // BPNETWORKQT_H
