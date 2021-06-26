#ifndef MQTTWIDGET_H
#define MQTTWIDGET_H

#include <QWidget>
#include <QThread>
#include "mqttclient.h"
#include "class_calculation_result.h"
namespace Ui {
class MQttWidget;
}

class MQttWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MQttWidget(QWidget *parent = 0);
    ~MQttWidget();

signals:
    void sig_onConnect(ClientInfo*);
    void sig_onDisconnect();

    void sig_SendMessage222MQttXXXX(class_calculation_result *clss);
    void sig_SendMessage222MQttXXXX(bool);
    void sig_SendMessage222MQttXXXX(DeviceFaultAlarm faultCode);


    void sig_shake_detect_tag(bool,int);//单次光纤测振指令
    void sig_settings_tag(SettingsTag*);//设备参数设置指令

public slots:

    //用于监测设备向监测平台上传单路由光纤振动监测数据
    void slot_SendMessage222MQttXXXX(class_calculation_result *clss);
    //用于监测设备向监测平台上传设备性能状态监测数据。
    //是否巡检返回的数据，false为正常周期任务生成的数据，true为巡检命令生成的数据。
    void slot_SendMessage222MQttXXXX(bool isInspect=true);
    //用于监测设备向监测平台上传故障告警数据
    void slot_SendMessage222MQttXXXX(DeviceFaultAlarm faultCode);

private slots:
    void on_btnConnect_clicked();
    void on_btnDisconnect_clicked();
    void show_Message_into_textBrowser(const QString &Message);
    void slot_mqttClient_state(int);
    void updateConnectUiStatus(bool isConnect);//更新UI
    void on_btnReadIni_clicked();//读取配置文件
    void on_btnSaveIni_clicked();//保存配置文件
    void on_btnClearLog_clicked();//清空LOG
    void on_cbx_Auto_Reconnect_stateChanged(int arg1);
    void slot_timerworking();
    void slot_GetMessageFromMQttXXXX(bool isInspect ,int Channel_ID); //单次光纤测振指令
    void slot_GetMessageFromMQttXXXX(SettingsTag* set22);

private:
    Ui::MQttWidget *ui;
    MQttClient *m_mqttClient;
    QThread *m_mqttClientThread;
    QTimer *m_timer222connectMqtt;
    int textBrowsercount;//计数,用于清空Log
};

#endif // MQTTWIDGET_H
