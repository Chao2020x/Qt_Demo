#include "mqttclient.h"

/**
 *  消息订阅
 *  topic:settings_tag
 *  说明:设备参数设置指令
 *
 *  topic:shake_detect_tag
 *  说明:单次光纤测振指令
 */
//初始化消息订阅
void MQttClient::slot_InitMQttClientSub()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();

    if(m_client == nullptr) {return;}

    QStringList m_list_SubscribeInfo;
    m_list_SubscribeInfo.clear();
    m_list_SubscribeInfo << "/subscribe/num"
                         << "/subscribe/Topic"      //订阅的Topic
                         << "/subscribe/QOS"        //订阅的QOS
                         ;
    QSettings setting(MQttConf,QSettings::IniFormat);

    int num = setting.value(m_list_SubscribeInfo.at(0)).toInt();
    QString mmppppp = QString("The number of Topics Subscribed is :%1").arg(num);
    emit sig_mqttClient_SubOrRel_info(mmppppp);
    if(num <= 0){return;}
    QString topic222;
    int qos222=0;
    for(int i = 0; i < num; i ++){
        topic222.isEmpty();
        topic222 = setting.value(m_list_SubscribeInfo.at(1)+QString::number(i)).toString();
        if(topic222.isEmpty()){continue;}
        qos222 = 0;
        qos222 = setting.value(m_list_SubscribeInfo.at(2)+QString::number(i)).toInt();
        if((qos222 > 2) || (qos222 < 0)) { qos222 = 0;}

        QString subResult;
        subResult.clear();
        if(!m_client->subscribe(topic222,qos222)){
            subResult = QString("Could not subscribe:[Topic:%1 QOS:%2]...Is there a valid connection?").arg(
                          topic222).arg(qos222);
        }else {
            subResult = QString("subscribe:[Topic:%1 QOS:%2] success...").arg(topic222).arg(qos222);
        }
        emit sig_mqttClient_SubOrRel_info(subResult);
    }
    m_list_SubscribeInfo.clear();
    return;
}

//接收MQtt服务器发送过来的消息
void MQttClient::slot_ReceiveMessageFromMQtt(const QByteArray &message, const QMqttTopicName &topic)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();

    QStringList m_list_ReceiveTopic;
    m_list_ReceiveTopic.clear();
    m_list_ReceiveTopic << "settings_tag"       //0 设备参数设置指令。
                       << "shake_detect_tag"    //1 单次光纤测振指令
                        ;
    const QString content = QLatin1String("Received Topic: ")
                        + topic.name()
                        + QLatin1String(" Message: ")
                        + message;
    emit sig_mqttClient_SubOrRel_info(content);

    if(topic.name().contains(m_list_ReceiveTopic.at(0))){
        slot_Sub_settings_tag(message);
        return;
    }

    if(topic.name().contains(m_list_ReceiveTopic.at(1))) {
        slot_Sub_shake_detect_tag(message);
        return;
    }
    return;
}

//设备参数设置指令
void MQttClient::slot_Sub_settings_tag(const QByteArray &message)
{
    SettingsTag *setting22XX = new SettingsTag;

    if(!messageParse222Tag(message,setting22XX)) {
        delete setting22XX; setting22XX = nullptr;
        return;
    }

    if(setting22XX->isSetOSW || setting22XX->isSetGT_MT){
        emit sig_settings_tag(setting22XX);
    }else{
        delete setting22XX; setting22XX = nullptr;
    }
    return;
}

//单次光纤测振指令
void MQttClient::slot_Sub_shake_detect_tag(const QByteArray &message)
{
    int Channel_ID = 0;
    if(!messageParse222Tag(message,Channel_ID)) {return;}
    if((Channel_ID < 0) || (Channel_ID > 5)) {return;}
    emit sig_shake_detect_tag(true,Channel_ID);
    return;
}

bool MQttClient::messageParse222Tag(const QByteArray &message,SettingsTag *setting22Gt)
{
    QJsonParseError jsonError;
    // 转化为 JSON 文档
    QJsonDocument doucment = QJsonDocument::fromJson(message, &jsonError);
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        // 解析未发生错误
        if (!doucment.isObject()) {
            emit sig_mqttClient_SubOrRel_info("解析未发生错误，但不是JSON 文档对象...");
            return false;
        }
        QJsonObject object = doucment.object();  // 转化为对象
        if (object.contains("data")) {
            QJsonValue value = object.value("data");
            if(!value.isObject()){;}
            QJsonObject obj = value.toObject();

            if (obj.contains("")) {
                QJsonValue value = obj.value("");
                if (value.isString()) {
                    setting22Gt->isSetGT_MT = true;
                }
             }
         }
        emit sig_mqttClient_SubOrRel_info("Content Parsing Successful");
        return true;
    }
    emit sig_mqttClient_SubOrRel_info("Content Parsing Failed...");
    return false;
}

bool MQttClient::messageParse222Tag(const QByteArray &message, int &ch)
{
    QJsonParseError jsonError;
    // 转化为 JSON 文档
    QJsonDocument doucment = QJsonDocument::fromJson(message, &jsonError);
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        // 解析未发生错误
        if (!doucment.isObject()) {
            emit sig_mqttClient_SubOrRel_info("解析未发生错误，但不是JSON 文档对象...");
            return false;
        }
        QJsonObject object = doucment.object();  // 转化为对象
        if (object.contains("id")) {
            QJsonValue value = object.value("id");
                QString uuid = value.toString();
                if(uuid.isEmpty()) {return false;}
                 ch = uuid.right(1).toInt();
         }
        emit sig_mqttClient_SubOrRel_info("Content Parsing Successful");
        return true;
    }
    emit sig_mqttClient_SubOrRel_info("Content Parsing Failed...");
    return false;
}
