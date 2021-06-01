/**
关于连接部分的代码
*/
#include "mywidget.h"


void MyWidget::onConnect(){
    switch (m_client->state()) {
    case QMqttClient::Disconnected:
        qDebug()<<"m_client->state():"<<"Disconnected";
        m_client->setHostname(ConnectHostName.toUtf8());
        m_client->setPort(ConnectPort);
        m_client->setCleanSession(ConnectCleanSession);
        m_client->setKeepAlive(60);//心跳

        if(!ConnectClientID.isEmpty())
        {m_client->setClientId(ConnectClientID.toUtf8());}

        if(!ConnectUsername.isEmpty())
        {m_client->setUsername(ConnectUsername.toUtf8());}

        if(!ConnectPassword.isEmpty())
        {m_client->setPassword(ConnectPassword.toUtf8());}

        m_client->connectToHost();
        break;
    case QMqttClient::Connecting:
        qDebug()<<"m_client->state():"<<"Connecting";

        break;
    case QMqttClient::Connected:
        qDebug()<<"m_client->state():"<<"Connected";

        break;
    default:
        return;
    }
}

void MyWidget::onDisconnect(){
    if(m_client->state()!= QMqttClient::Disconnected){
        m_client->disconnectFromHost();
    }
}
