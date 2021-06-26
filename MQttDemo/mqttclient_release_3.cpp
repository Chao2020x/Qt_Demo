#include "mqttclient.h"

/**
 *  消息发布
 *  topic:alarm_tag
 *  说明:故障告警数据
 *  搞个定时器定时查询，发送
 *  且预留接口出来让外界发送
 */

void MQttClient::slot_SendMessage222MQtt(DeviceFaultAlarm faultCode)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    //new一个ReleaseMessage
    ReleaseMessage *rel_mg = new ReleaseMessage;

    //数据解析成JSON
    contentParse222JSON(rel_mg,faultCode);

    //加入列表
    slot_SendMessage222MQtt(rel_mg);
}

bool MQttClient::contentParse222JSON(ReleaseMessage *message, DeviceFaultAlarm faultCode)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    message->Topic = "alarm_tag";
    message->Qos = 0;
    message->Retained = false;
    message->Text.clear();

    QString faultType;
    switch (faultCode) {
    case ClockFault:         faultType="时钟故障";   break;
    case FanFault:           faultType="风扇故障";   break;
    case PowerFault:         faultType="电源故障";   break;
    case NetworkFault:       faultType="网络故障";   break;
    case LaserFault:         faultType="激光器故障"; break;
    case OpticalSignalFault: faultType="光信号故障"; break;
    case FiberFault:         faultType="断纤故障";   break;
    default:                 faultType="故障码错误"; break;
    }

    QStringList m_list_ReleaseInfo;
    m_list_ReleaseInfo.clear();
    m_list_ReleaseInfo << "version"              //0 表示协议版本号，当前版本号是1.0。
                       << "id"                   //1 表示消息采集设备唯一标识，。字段组成结构为: 公司标识+电房标识+光纤测振主机设备id。
                       << "time"                 //2 表示数据上传的时间。格式为YYYY-MM-DD HH24:MM:SS
                       << "data"                 //3 监测设备采集的数值，data是一个json串
                       << "prob"                 //4 故障码
                       << "desc"                 //5 故障描述（数据类型：String）
                       << "statusdatetime"       //6 表示数据采集的时间点，格式为YYYY-MM-DD HH24:MM:SS
                       ;

    QJsonObject aloneObject;

    QSettings setting(MQttConf,QSettings::IniFormat);

    //QString 表示协议版本号，当前版本号是1.0。
    QString valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(0);
    QString _version = setting.value(valueConf).toString();
    if(_version.isEmpty()) {_version = "1.0";}
    aloneObject.insert(m_list_ReleaseInfo.at(0),_version);

    //QString 表示光纤唯一标识。组成结构为: 公司标识+电房标识+光纤测振主机设备id+光纤通道id。
    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(0);
    QString _id = setting.value(valueConf).toString();
    if(_id.isEmpty()) {_id = "GZDLGSYOSCER";}
    aloneObject.insert(m_list_ReleaseInfo.at(1),_id);

    //"time"                 //2 表示数据上传的时间。
    aloneObject.insert(m_list_ReleaseInfo.at(2),QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    QJsonObject dataObject;
    //"prob"                 //4 故障码
    dataObject.insert(m_list_ReleaseInfo.at(4),QString::number((int)faultCode));

    //"desc"                 //5 故障描述（数据类型：String）
    dataObject.insert(m_list_ReleaseInfo.at(5),faultType);

    //"statusdatetime"       //6 表示数据采集的时间点
    dataObject.insert(m_list_ReleaseInfo.at(6),QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    aloneObject.insert(m_list_ReleaseInfo.at(3),dataObject);

    // 构建 Json 文档
    QJsonDocument document;
    document.setObject(aloneObject);
    message->Text = document.toJson(QJsonDocument::Compact);

    m_list_ReleaseInfo.clear();
    return true;
}
