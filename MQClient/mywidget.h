#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <qmqttclient.h>
#include <QtMqtt/qmqttclient.h>
#include <QMessageBox>
#include <QtCore/QDateTime>
#include <QtMqtt/qmqttglobal.h>
#include <QtMqtt/qmqttsubscription.h>

#define TIMEMS          QTime::currentTime().toString("HH:mm:ss.zzz")
#define TIME            QTime::currentTime().toString("HH:mm:ss")
#define QDATE           QDate::currentDate().toString("yyyy-MM-dd")
#define QTIME           QTime::currentTime().toString("HH-mm-ss")
#define DATETIME        QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
#define STRDATETIME     QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss")
#define STRDATETIMEMS   QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss-zzz")

namespace Ui {
class MyWidget;
}
QT_END_NAMESPACE

class MyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyWidget(QWidget *parent = 0);
    ~MyWidget();

private slots:

    void on_btnConnect_clicked();//连接
    void on_btnDisconnect_clicked();//断开连接
    void on_btnSubscribe_clicked();//订阅
    void on_btnRelease_clicked();//发布
    void on_btnClean_clicked();//清除
    void on_btnClose_clicked();//关闭

    void add_error_log(QMqttClient::ClientError error);
    void UpdataMessagetoLOG(const QByteArray &message, const QMqttTopicName &topic=QMqttTopicName());
    void UpdataMessagetoLOG_2();
    void UpdataMessagetoLOG_3(QMqttMessage msg);
    /*------------连接函数-----------*/
    void onConnect();
    void onDisconnect();
    void updateConnectUiStatus();
    /*------------订阅函数-----------*/
    void onSubscribe();
    void updateSubscribeUiStatus();
    /*------------发布函数-----------*/
    void onRelease();

private:
    Ui::MyWidget *ui;
    QMqttClient *m_client;
//    QMqttSubscription *m_subscr;
    /*------------初始化函数----------*/
    void InitConnect();//初始化连接
    void InitSubscribe();//初始化订阅
    void InitRelease();//初始化发布
    void InitLog();//初始化LOG
    /*------------连接变量------------*/
    QString ConnectHostName;
    int ConnectPort;
    QString ConnectClientID;
    QString ConnectUsername;
    QString ConnectPassword;
    bool ConnectCleanSession;
    bool ConnectSSL;
    /*------------订阅变量------------*/
    QStringList SubscribeTopic;//后期变为列表格式
    int SubscribeQOS;
    /*------------发布变量------------*/
    QStringList ReleaseTopic;
    QString ReleaseText;
    int ReleaseQOS;
    bool ReleaseRetained;



};

#endif // MYWIDGET_H
