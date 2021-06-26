#include "mqttwidget.h"
#include "ui_mqttwidget.h"

MQttWidget::MQttWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MQttWidget)
{
    textBrowsercount = 0;
    ui->setupUi(this);
    m_timer222connectMqtt = new QTimer(this);
    m_timer222connectMqtt->setInterval(30*1000);//10秒
    connect(m_timer222connectMqtt,SIGNAL(timeout()),this,SLOT(slot_timerworking()));
    m_timer222connectMqtt->stop();
    //新建MQtt逻辑线程
    m_mqttClientThread = new QThread(this);
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    //新建MQtt客户端对象
    m_mqttClient = new MQttClient();
    //将对象移动到逻辑线程
    m_mqttClient->moveToThread(m_mqttClientThread);
//----------------关联界面UI、MQttClient类、QThread线程间的信号与槽--------------
    //MQtt逻辑与执行线程之间
    connect(m_mqttClientThread,SIGNAL(finished()),m_mqttClient,SLOT(deleteLater()));

//---------------------MQtt逻辑与UI之间
    //连接服务器
    connect(this,SIGNAL(sig_onConnect(ClientInfo*)),
            m_mqttClient,SLOT(slot_InitMQttClient(ClientInfo*)));
    connect(m_mqttClient,SIGNAL(sig_mqttClient_state(int)),
            this,SLOT(slot_mqttClient_state(int)));
    //显示信息
    connect(m_mqttClient,&MQttClient::sig_mqttClient_SubOrRel_info,
            this,&MQttWidget::show_Message_into_textBrowser);
    //断开连接
    connect(this,&MQttWidget::sig_onDisconnect,m_mqttClient,
            &MQttClient::slot_DisconnectMQttSever);

//--------------要通过MQtt发送信息
    //用于监测设备向监测平台上传单路由光纤振动监测数据
    connect(this,SIGNAL(sig_SendMessage222MQttXXXX(class_calculation_result*)),
            m_mqttClient,SLOT(slot_SendMessage222MQtt(class_calculation_result*)));
    //用于监测设备向监测平台上传设备性能状态监测数据。
    //是否巡检返回的数据，false为正常周期任务生成的数据，true为巡检命令生成的数据。
    connect(this,SIGNAL(sig_SendMessage222MQttXXXX(bool)),
            m_mqttClient,SLOT(slot_SendMessage222MQtt(bool)));
    //用于监测设备向监测平台上传故障告警数据
    connect(this,SIGNAL(sig_SendMessage222MQttXXXX(DeviceFaultAlarm)),
            m_mqttClient,SLOT(slot_SendMessage222MQtt(DeviceFaultAlarm)));

//--------------要通过MQtt接收到的信息
    //设备参数设置指令
    connect(m_mqttClient,SIGNAL(sig_settings_tag(SettingsTag*)),
            this,SLOT(slot_GetMessageFromMQttXXXX(SettingsTag*)));
    //单次光纤测振指令
    connect(m_mqttClient,SIGNAL(sig_shake_detect_tag(bool,int)),
            this,SLOT(slot_GetMessageFromMQttXXXX(bool,int)));
    //启动线程
    m_mqttClientThread->start();

    QTimer::singleShot(1000,this,[=](){
        on_btnReadIni_clicked();
    });
}

MQttWidget::~MQttWidget()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    m_mqttClientThread->quit();
    m_mqttClientThread->wait();
    m_timer222connectMqtt->stop();
    delete ui;
}

//用于监测设备向监测平台上传单路由光纤振动监测数据
void MQttWidget::slot_SendMessage222MQttXXXX(class_calculation_result *clss)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    //复制一份
    class_calculation_result *clss22 = new class_calculation_result();
    clss22->class_copy(*clss);
    //发送信号
    emit sig_SendMessage222MQttXXXX(clss22);
}

//用于监测设备向监测平台上传设备性能状态监测数据。
void MQttWidget::slot_SendMessage222MQttXXXX(bool isInspect)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    //正常周期任务生成的数据不需要你去发送
    if(isInspect){
        emit sig_SendMessage222MQttXXXX(isInspect);
    }
}

//用于监测设备向监测平台上传故障告警数据
void MQttWidget::slot_SendMessage222MQttXXXX(DeviceFaultAlarm faultCode)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    emit sig_SendMessage222MQttXXXX(faultCode);
}

//连接
void MQttWidget::on_btnConnect_clicked()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    //获取连接信息
    ClientInfo *info = new ClientInfo;
    info->HostName = ui->lineEditServer->text();
    info->Port = ui->spinBoxPort->value();
    info->KeepAlive = ui->spinBox_KeepAlive->value();
    info->ClientID = ui->lineEdit_ClientID->text();
    info->UserName = ui->lineEdit_Username->text();
    info->PassWord = ui->lineEdit_Password->text();
    info->IsCleanSession = ui->cbx_CleanSession->isChecked();
    info->IsSSL = ui->cbx_SSL->isChecked();

    //校验登录信息并显示
    if(info->HostName.isEmpty() || info->ClientID.isEmpty()) {
        show_Message_into_textBrowser("Server IP or ClientID is empty...\nPlease input again...");
        return;
    }

    QString cc22 = QString("ClientID:%1 Connecting to the MQtt Server[%2:%3]").arg(
                info->ClientID).arg(info->HostName).arg(info->Port);
    show_Message_into_textBrowser(cc22);

    //发送连接信号
    emit sig_onConnect(info);
    return;
}

//断开连接
void MQttWidget::on_btnDisconnect_clicked()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    //发送断开连接信号
    emit sig_onDisconnect();
    //更新UI样式
    updateConnectUiStatus(false);
}

void MQttWidget::show_Message_into_textBrowser(const QString &Message)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    if(textBrowsercount >= 20){
        //清空
        ui->textBrowser->clear();
        textBrowsercount = 0;
    }

    QString cc = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss]");
    cc += " ";
    cc += Message;
    ui->textBrowser->append(cc);
    ++ textBrowsercount;
    return;
}

void MQttWidget::slot_mqttClient_state(int state)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    QString cc222;
    bool state222 = false;//未连接
    switch (state) {
    case QMqttClient::Disconnected:
        state222 = false;//未连接
        cc222 = "Disconnected";
        break;
    case QMqttClient::Connecting:
        state222 = false;//连接中
        cc222 = "Connecting";
        break;
    case QMqttClient::Connected:
        state222 = true;//已连接
        cc222 = "Connected";
        break;
    default:
        break;
    }
    updateConnectUiStatus(state222);
    QString GGG = QString("MQttClient %1 to the MQtt Server").arg(
                cc222);
    show_Message_into_textBrowser(GGG);
    return;
}


void MQttWidget::updateConnectUiStatus(bool isConnect)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    bool state = !isConnect;
    ui->lineEditServer->setEnabled(state);
    ui->spinBoxPort->setEnabled(state);
    ui->spinBox_KeepAlive->setEnabled(state);
    ui->btnConnect->setEnabled(state);
    ui->lineEdit_ClientID->setEnabled(state);
    ui->lineEdit_Username->setEnabled(state);
    ui->lineEdit_Password->setEnabled(state);
    ui->btnConnect->setEnabled(state);

    ui->cbx_CleanSession->setEnabled(state);
    ui->cbx_SSL->setEnabled(state);
    if(isConnect){
        ui->label_isConnect->setText("已连接"); //绿色
        ui->label_isConnect->setStyleSheet("QLabel{background-color:rgb(0,255,0);}");
    }else{
        ui->label_isConnect->setText("未连接"); //白色
        ui->label_isConnect->setStyleSheet("QLabel{background-color:rgb(255,255,255);}");
    }
    return;
}

void MQttWidget::on_btnReadIni_clicked()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    QStringList m_ConfStrList;
    m_ConfStrList.clear();
    m_ConfStrList << "/clientInfo/IP"
                  << "/clientInfo/port"
                  << "/clientInfo/isCleanSession"
                  << "/clientInfo/keepAlive"
                  << "/clientInfo/ID"
                  << "/clientInfo/username"
                  << "/clientInfo/password"
                  << "/clientInfo/isAutoReconnect"
                  << "/clientInfo/isSSL"
                   ;
    QSettings setting(MQttConf,QSettings::IniFormat);

    //IP
    ui->lineEditServer->setText(setting.value(m_ConfStrList.at(0)).toString());
    //Port
    ui->spinBoxPort->setValue(setting.value(m_ConfStrList.at(1)).toInt());
    //isCleanSession
    ui->cbx_CleanSession->setChecked(setting.value(m_ConfStrList.at(2)).toBool());
    //keepAlive
    ui->spinBox_KeepAlive->setValue(setting.value(m_ConfStrList.at(3)).toInt());
    //ID
    ui->lineEdit_ClientID->setText(setting.value(m_ConfStrList.at(4)).toString());
    //username
    ui->lineEdit_Username->setText(setting.value(m_ConfStrList.at(5)).toString());
    //password
    ui->lineEdit_Password->setText(setting.value(m_ConfStrList.at(6)).toString());

    bool cc = setting.value(m_ConfStrList.at(7)).toBool();
    QTimer::singleShot(1000,this,[=](){
        ui->cbx_Auto_Reconnect->setChecked(cc);
    });

    ui->cbx_SSL->setChecked(setting.value(m_ConfStrList.at(8)).toBool());
    return;
}

void MQttWidget::on_btnSaveIni_clicked()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    QStringList m_ConfStrList;
    m_ConfStrList.clear();
    m_ConfStrList << "/clientInfo/IP"
                  << "/clientInfo/port"
                  << "/clientInfo/isCleanSession"
                  << "/clientInfo/keepAlive"
                  << "/clientInfo/ID"
                  << "/clientInfo/username"
                  << "/clientInfo/password"
                  << "/clientInfo/isAutoReconnect"
                  << "/clientInfo/isSSL"
                   ;
    QSettings setting(MQttConf,QSettings::IniFormat);
    setting.setValue(m_ConfStrList.at(0),ui->lineEditServer->text());
    setting.setValue(m_ConfStrList.at(1),ui->spinBoxPort->value());
    setting.setValue(m_ConfStrList.at(2),ui->cbx_CleanSession->isChecked());
    setting.setValue(m_ConfStrList.at(3),ui->spinBox_KeepAlive->value());
    setting.setValue(m_ConfStrList.at(4),ui->lineEdit_ClientID->text());
    setting.setValue(m_ConfStrList.at(5),ui->lineEdit_Username->text());
    setting.setValue(m_ConfStrList.at(6),ui->lineEdit_Password->text());
    setting.setValue(m_ConfStrList.at(7),ui->cbx_Auto_Reconnect->isChecked());
    setting.setValue(m_ConfStrList.at(8),ui->cbx_SSL->isChecked());

    return;
}

void MQttWidget::on_btnClearLog_clicked()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    ui->textBrowser->clear();
    textBrowsercount = 0;
}

void MQttWidget::on_cbx_Auto_Reconnect_stateChanged(int arg1)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId()
           <<"stateChanged:"<<arg1;
    if(ui->cbx_Auto_Reconnect->isChecked()){
        m_timer222connectMqtt->start();
    }
    else {
        m_timer222connectMqtt->stop();
    }
}

void MQttWidget::slot_timerworking()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    if(ui->btnConnect->isEnabled()){
        on_btnConnect_clicked();
    }
    return;
}

//单次光纤测振指令
void MQttWidget::slot_GetMessageFromMQttXXXX(bool isInspect, int Channel_ID)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    QString info = QString("单次光纤测振指令,测振光纤%1通道").arg(Channel_ID);
    show_Message_into_textBrowser(info);
    emit sig_shake_detect_tag(isInspect,Channel_ID);
}

//设备参数设置指令
void MQttWidget::slot_GetMessageFromMQttXXXX(SettingsTag *set22)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    if(set22->isSetOSW){
        show_Message_into_textBrowser("正在设置光开关...");
    }
    if(set22->isSetGT_MT){
        show_Message_into_textBrowser("正在设置GT_MT参数...");
    }
    emit sig_settings_tag(set22);
}
