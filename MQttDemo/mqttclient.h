#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include <QList>
#include <QMutex>
#include <QTimer>
#include <QString>
#include <QThread>
#include <QSettings>
#include <QEventLoop>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtMqtt/qmqttclient.h>

#include "info.h"
#include "class_calculation_result.h"

//MQtt配置文件
#define MQttConf "conf.ini"


enum SettingGT_MT{
    SetOpenCOM = 101,
    SetCloseCOM = 102,
    SetVOA = 210,
    SetAlarm = 220,
    SetModStep = 230,
    SetFiberLen = 240,
    SetParamAB = 250,
    SetOther = 500
};

//设置指令
struct SettingsTag
{
    bool isSetOSW;        //是否设置光开关
    int OSWChannelID;     //光纤切换通道
    bool isSetGT_MT;      //是否设置GT_MT参数
    SettingGT_MT setTpye; //设置类型
    int value_1;
    int value_2;
    int value_3;
    int value_4;

    SettingsTag() {
        isSetOSW = false;
        OSWChannelID = 0;
        isSetGT_MT = false;
        setTpye = SetOther;
        value_1 = 0;
        value_2 = 0;
        value_3 = 0;
        value_4 = 0;
    }
};

enum DeviceFaultAlarm{
    ClockFault         = 1,     //时钟故障
    FanFault           = 2,     //风扇故障
    PowerFault         = 3,     //电源故障
    NetworkFault       = 4,     //网络故障
    LaserFault         = 5,     //激光器故障
    OpticalSignalFault = 6,     //光信号故障
    FiberFault         = 7      //断纤故障
};

struct ClientInfo
{
    QString HostName;        //IP
    int Port;                //端口号
    QString ClientID;        //连接名
    QString UserName;        //用户名
    QString PassWord;        //密码
    int KeepAlive;           //心跳时间
    bool IsCleanSession;
    bool IsSSL;
};

//MQtt发布信息结构体
struct ReleaseMessage
{
    QString Topic;
    QByteArray Text;
    int Qos;
    bool Retained;
    ReleaseMessage(QString topic,QByteArray text){
        Topic = topic;
        Text = text;
        Qos = 0;
        Retained = false;
    }
    ReleaseMessage(){}
};
class MQttClient : public QObject
{
    Q_OBJECT
public:
    QMqttClient *m_client;
    explicit MQttClient(QObject *parent = nullptr);
    ~MQttClient();

signals:
    void sig_start_timer();                            //初始化启动定时器
    void sig_mqttClient_state(int);                    //连接状态的信息
    void sig_mqttClient_SubOrRel_info(const QString&); //发布或订阅的信息
    void sig_shake_detect_tag(bool,int);               //单次光纤测振指令
    void sig_settings_tag(SettingsTag *);              //设备参数设置指令

public slots:
    void slot_InitMQttClient(ClientInfo*);      //初始化MQtt客户端,连接信号与槽
    void slot_ConnectMQttSever();               //连接服务器
    void slot_InitMQttClientSub();              //初始化MQtt客户端订阅
    void slot_DisconnectMQttSever();            //断开与服务器的连接

    void slot_mqttClient_state();
    void slot_SendMessage222MQtt_done(ReleaseMessage *message);          //将信息发送到MQtt服务器
    void slot_SendMessage222MQtt(ReleaseMessage *message);               //将信息添加到m_list_message中
    void slot_SendMessage222MQtt(class_calculation_result *calss_result);//将报警信息结构体添加到m_list_message中
    void slot_SendMessage222MQtt(bool device = false);
    void slot_SendMessage222MQtt(DeviceFaultAlarm faultCode);

    void slot_ReceiveMessageFromMQtt(const QByteArray &message,
                                      const QMqttTopicName &topic);//从服务器接收信息

    void slot_Sub_settings_tag(const QByteArray &message);//设备参数设置指令
    void slot_Sub_shake_detect_tag(const QByteArray &message);//单次光纤测振指令

private slots:
    void slot_InitQTimer();                     //定时发送向MQtt服务器发送信息的定时器
    void slot_timeworking();                    //定时发送向MQtt服务器发送信息

private:
    QMutex m_mutex;                          //好像没什么用
    Info m_systemInfo;                       //获取系统CPU、内存、磁盘等信息
    QTimer *m_timer_SendMessage222MQtt;      //发送信息到MQtt服务器的定时器
    QList<ReleaseMessage *> m_list_message;  //用于存储要发送的信息
    //将报警数据解析成JSON格式
    bool contentParse222JSON(ReleaseMessage *message,
                             class_calculation_result *calss_result);
    //从数据文件夹内读取波形数据
    bool read_data_from_file(QJsonArray &jsonArray,const QString &file_name);
    //将字符串解析成JSON格式
    bool contentParse222JSON(ReleaseMessage *message,
                             QString &data);
    bool contentParse222JSON(ReleaseMessage *message,bool mm_isInspect);
    bool contentParse222JSON(ReleaseMessage *message,DeviceFaultAlarm faultCode);

    bool messageParse222Tag(const QByteArray &message,SettingsTag *setting22Gt);
    bool messageParse222Tag(const QByteArray &message,int &ch);


};
#endif // MQTTCLIENT_H
