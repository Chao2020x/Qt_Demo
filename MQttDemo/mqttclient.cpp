#include "mqttclient.h"

MQttClient::MQttClient(QObject *parent) : QObject(parent)
{
    m_client = nullptr;
    m_timer_SendMessage222MQtt = nullptr;
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    connect(this,SIGNAL(sig_start_timer()),this,SLOT(slot_InitQTimer()));
    QTimer::singleShot(500,[=](){
        emit sig_start_timer();
    });
}

MQttClient::~MQttClient()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    slot_DisconnectMQttSever();
    if(m_timer_SendMessage222MQtt != nullptr)
        m_timer_SendMessage222MQtt->stop();
}
//定时读取信息并发送
void MQttClient::slot_InitQTimer()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    if(m_timer_SendMessage222MQtt != nullptr)
    {
        m_timer_SendMessage222MQtt->stop();
        delete m_timer_SendMessage222MQtt;
        m_timer_SendMessage222MQtt = nullptr;
    }
    m_timer_SendMessage222MQtt = new QTimer(this);
    m_timer_SendMessage222MQtt->setInterval(500);
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    connect(m_timer_SendMessage222MQtt,SIGNAL(timeout()),this,SLOT(slot_timeworking()));
    m_timer_SendMessage222MQtt->stop();
}

void MQttClient::slot_timeworking()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    m_mutex.lock();
    if(m_list_message.isEmpty()) { m_mutex.unlock(); return;}
    slot_SendMessage222MQtt_done(m_list_message.last());
    delete m_list_message.last();
    m_list_message.pop_back();
    m_mutex.unlock();
    return;
}

void MQttClient::slot_InitMQttClient(ClientInfo *info)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();

    if(m_client != nullptr) { delete m_client; m_client=nullptr;}

    m_client = new QMqttClient(this);
    m_client->setHostname(info->HostName);
    m_client->setPort(info->Port);
    m_client->setKeepAlive(info->KeepAlive);
    m_client->setClientId(info->ClientID);
    m_client->setCleanSession(info->IsCleanSession);

    if(!info->UserName.isEmpty())
    {m_client->setUsername(info->UserName);}

    if(!info->PassWord.isEmpty())
    {m_client->setPassword(info->PassWord);}

    //建立信号与槽
    connect(m_client, &QMqttClient::stateChanged,this,&MQttClient::slot_mqttClient_state);
    connect(m_client, &QMqttClient::messageReceived, this, &MQttClient::slot_ReceiveMessageFromMQtt);

    //连接服务器
    slot_ConnectMQttSever();
    delete info;
    info = nullptr;
}

void MQttClient::slot_mqttClient_state()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    int num = (int)m_client->state();
    emit sig_mqttClient_state(num);
    if(m_client->state() == QMqttClient::Connected){
        m_timer_SendMessage222MQtt->start();
    }else{
        m_timer_SendMessage222MQtt->stop();
    }
}

void MQttClient::slot_ConnectMQttSever()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    if(m_client->state() != QMqttClient::Connected){
        m_client->connectToHost();
        QEventLoop myloop11;
        QTimer::singleShot(1500,&myloop11, &QEventLoop::quit );
        myloop11.exec();

        QTimer::singleShot(300,[=](){
            slot_InitMQttClientSub();
        });
    }
}

void MQttClient::slot_DisconnectMQttSever()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    if((m_client != nullptr) && (m_client->state() != QMqttClient::Disconnected)){
        m_client->disconnectFromHost();
        QEventLoop myloop11;
        QTimer::singleShot(1000,&myloop11, &QEventLoop::quit );
        myloop11.exec();
    }
}

void MQttClient::slot_SendMessage222MQtt_done(ReleaseMessage *message)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();

    if(m_client == nullptr) { return;}
    if(m_client->state() != QMqttClient::Connected){slot_ConnectMQttSever();}

    m_client->publish(message->Topic,message->Text,
                        message->Qos,message->Retained);

    qDebug()<<"send:"<<message->Topic<<message->Text;
    delete message;
    message = nullptr;
    return;
}

void MQttClient::slot_SendMessage222MQtt(ReleaseMessage *message)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();

    m_mutex.lock();
    if(message != nullptr) {m_list_message.push_back(message);}
    if(m_list_message.size() > 100) {
        if(m_list_message.first() != nullptr)
        delete m_list_message.first();
        m_list_message.removeFirst();
    }
    m_mutex.unlock();
    return;
}
