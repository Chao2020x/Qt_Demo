#include "mqttclient.h"

/**
 *  消息发布
 *  topic:device_status_tag
 *  说明:设备性能状态数据
 *  搞个定时器，定时发送
 *  且预留接口出来让外界发送
 */

void MQttClient::slot_SendMessage222MQtt(bool device)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    //new一个ReleaseMessage
    ReleaseMessage *rel_mg = new ReleaseMessage;

    //数据解析成JSON
    contentParse222JSON(rel_mg,device);

    //加入列表
    slot_SendMessage222MQtt(rel_mg);

}

bool MQttClient::contentParse222JSON(ReleaseMessage *message,bool mm_isInspect)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    message->Topic = "device_status_tag";
    message->Qos = 0;
    message->Retained = false;
    message->Text.clear();

    QStringList m_list_ReleaseInfo;
    m_list_ReleaseInfo.clear();
    m_list_ReleaseInfo << "version"              //0 表示协议版本号，当前版本号是1.0。
                       << "id"                   //1 表示消息采集设备唯一标识，。字段组成结构为: 公司标识+电房标识+光纤测振主机设备id。
                       << "ip"                   //2 表示测振设备的ip
                       << "telnet_port"          //3 设备telnet端口
                       << "snmp_port"            //4 设备snmp端口。
                       << "shake_detect_period"  //5 表示设置光纤测振周期。单位秒。不设置可为空。
                       << "per_detect_period"    //6 表示设置性能检测周期。单位秒。不设置可为空。（默认一小时的0分为周期的启动）
                       << "is_inspect"           //7 是否巡检返回的数据，0为正常周期任务生成的数据，1为巡检命令生成的数据。
                       << "time"                 //8 表示数据上传的时间。格式为YYYY-MM-DD HH24:MM:SS
                       << "data"                 //9 监测设备采集的数值，data是一个json串，里面字段内容第一个双引号为数据名称，第二个双引号为数据值。
                       << "host_cpu"             //10 主机、工控机CPU使用率（百分比）0.61表示61%（数据类型：float）
                       << "host_mem"             //11 主机、工控机内存使用率（百分比），0.61表示61%（数据类型：float）
                       << "host_clock"           //12 主机、工控机时钟状态(0表示正常，1表示故障)
                       << "host_fan"             //13 主机、工控机风扇状态(0表示正常，1表示故障)
                       << "host_power"           //14 主机、工控机电源状态(0表示正常，1表示故障)
                       << "host_temp"            //15 主机、工控机温度，单位为摄氏度（数据类型：String）
                       << "laser"                //16 激光器相关状态数据。支持多个激光器设备上传
                       << "laserNum"             //17 激光器数量
                       << "laser_id"             //18 激光器唯一标识 （数据类型：int）
                       << "laser_shake"          //19 激光器振动，单位为摄氏度。（数据类型：float）
                       << "laser_elec"           //20 激光器电流，单位为A。（数据类型：float）
                       << "laser_shakeRef"       //21 光纤基准值振动，单位为摄氏度。（数据类型：float）
                       << "statusdatetime"       //22 表示数据采集的时间点 “2018-11-20 15:58:30”（数据类型： String）
                       << "other"
                       ;

    QJsonObject aloneObject;
    QSettings setting(MQttConf,QSettings::IniFormat);

    //QString 表示协议版本号，当前版本号是1.0。
    QString valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(0);
    QString _version = setting.value(valueConf).toString();
    if(_version.isEmpty()) {_version = "1.0";}
    aloneObject.insert(m_list_ReleaseInfo.at(0),_version);

    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(1);
    QString _id = setting.value(valueConf).toString();
    if(_id.isEmpty()) {_id = "GZDLGSYOSCER";}
    //QString 表示光纤唯一标识。组成结构为: 公司标识+电房标识+光纤测振主机设备id+光纤通道id。
    aloneObject.insert(m_list_ReleaseInfo.at(1),_id);

    //QString 表示测振设备的ip
    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(2);
    QString _ip = setting.value(valueConf).toString();
    if(_ip.isEmpty()) {_ip = "196.128.0.12";}
    aloneObject.insert(m_list_ReleaseInfo.at(2),_ip);

    //"telnet_port"          //3 设备telnet端口
    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(3);
    QString _telnet_port = setting.value(valueConf).toString();
    if(_telnet_port.isEmpty()) {_telnet_port = "23";}
    aloneObject.insert(m_list_ReleaseInfo.at(3),_telnet_port);

    //"snmp_port"            //4 设备snmp端口。
    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(4);
    QString _snmp_port = setting.value(valueConf).toString();
    if(_snmp_port.isEmpty()) {_snmp_port = "161";}
    aloneObject.insert(m_list_ReleaseInfo.at(4),_snmp_port);


    //"shake_detect_period"  //5 表示设置光纤测振周期。单位秒。不设置可为空。
    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(5);
    QString _shake_detect_period = setting.value(valueConf).toString();
    if(_shake_detect_period.isEmpty()) {_shake_detect_period = "";}
    aloneObject.insert(m_list_ReleaseInfo.at(5),_shake_detect_period);

    //"per_detect_period"    //6 表示设置性能检测周期。单位秒。不设置可为空。
    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(6);
    QString _per_detect_period = setting.value(valueConf).toString();
    if(_per_detect_period.isEmpty()) {_per_detect_period = "";}
    aloneObject.insert(m_list_ReleaseInfo.at(6),_per_detect_period);

    //"is_inspect"           //7 是否巡检返回的数据，0为正常周期任务生成的数据
//    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(7);
//    QString _is_inspect = setting.value(valueConf).toString();
//    if(_is_inspect.isEmpty()) {_is_inspect = "0";}
    QString _is_inspect = (mm_isInspect ? "1" : "0");
    aloneObject.insert(m_list_ReleaseInfo.at(7),_is_inspect);

    //"time"                 //8 表示数据上传的时间。格式为YYYY-MM-DD HH24:MM:SS
    aloneObject.insert(m_list_ReleaseInfo.at(8), QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    //"data"                 //9 监测设备采集的数值，data是一个json串
    QJsonObject dataObject;
    //"host_cpu"             //10 主机、工控机CPU使用率（百分比）
    dataObject.insert(m_list_ReleaseInfo.at(10),QString::number(m_systemInfo.GetCPUPercent(),'g',2));

    //"host_mem"             //11 主机、工控机内存使用率（百分比）
    dataObject.insert(m_list_ReleaseInfo.at(11),QString::number(m_systemInfo.GetMemoryUsedPercent(),'g',2));

    //"host_clock"           //12 主机、工控机时钟状态
    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(12);
    QString _host_clock= setting.value(valueConf).toString();
    if(_host_clock.isEmpty()) {_host_clock = "0";}
    dataObject.insert(m_list_ReleaseInfo.at(12),_host_clock);
    //"host_fan"             //13 主机、工控机风扇状态
    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(13);
    QString _host_fan= setting.value(valueConf).toString();
    if(_host_fan.isEmpty()) {_host_fan = "0";}
    dataObject.insert(m_list_ReleaseInfo.at(13),_host_fan);
    //"host_power"           //14 主机、工控机电源状态
    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(14);
    QString _host_power= setting.value(valueConf).toString();
    if(_host_power.isEmpty()) {_host_power = "0";}
    dataObject.insert(m_list_ReleaseInfo.at(14),_host_power);
    //"host_temp"            //15 主机、工控机温度，单位为摄氏度
    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(15);
    QString _host_temp= setting.value(valueConf).toString();
    if(_host_temp.isEmpty()) {_host_temp = "36";}
    dataObject.insert(m_list_ReleaseInfo.at(15),_host_temp);
    //"laser"                //16 激光器相关状态数据
    QJsonArray laserArray;
    // "laserNum"             //17 激光器数量
    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(17);
    int _laserNum = setting.value(valueConf).toInt();

    for(int nn = 0; nn < _laserNum; nn ++){
        QJsonObject laserXXXObject;
        QString cc_head = m_list_ReleaseInfo.at(16) + QString::number(nn);

        //"laser_id"             //18 激光器唯一标识 （数据类型：int）
        valueConf = cc_head +"/"+ m_list_ReleaseInfo.at(18);
        int _id = setting.value(valueConf).toInt();
        laserXXXObject.insert(m_list_ReleaseInfo.at(18),_id);

        //"laser_shake"          //19 激光器振动，单位为摄氏度。（数据类型：float）
        valueConf = cc_head +"/"+ m_list_ReleaseInfo.at(19);
        float _shake = setting.value(valueConf).toFloat();
        laserXXXObject.insert(m_list_ReleaseInfo.at(19),_shake);

        //"laser_elec"           //20 激光器电流，单位为A。（数据类型：float）
        valueConf = cc_head +"/"+ m_list_ReleaseInfo.at(20);
        float _elec = setting.value(valueConf).toFloat();
        laserXXXObject.insert(m_list_ReleaseInfo.at(20),_elec);

        //"laser_shakeRef"       //21 光纤基准值振动，单位为摄氏度。（数据类型：float）
        valueConf = cc_head +"/"+ m_list_ReleaseInfo.at(21);
        float _shakeRef = setting.value(valueConf).toFloat();
        laserXXXObject.insert(m_list_ReleaseInfo.at(21),_shakeRef);

        laserArray.append(laserXXXObject);
    }
    dataObject.insert(m_list_ReleaseInfo.at(16),laserArray);

    // "statusdatetime"       //22 表示数据采集的时间点 “2018-11-20 15:58:30”（数据类型： String）
    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(22);
    QString _statusdatetime= setting.value(valueConf).toString();
    if(_statusdatetime.isEmpty()) {_statusdatetime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");}
    else{
        QDateTime ccmm = QDateTime::fromString(_statusdatetime,"yyyy-MM-dd-HH-mm-ss");
        _statusdatetime = ccmm.toString("yyyy-MM-dd HH:mm:ss");
    }
    dataObject.insert(m_list_ReleaseInfo.at(22),_statusdatetime);

    aloneObject.insert(m_list_ReleaseInfo.at(9),dataObject);
    // 构建 Json 文档
    QJsonDocument document;
    document.setObject(aloneObject);
    message->Text = document.toJson(QJsonDocument::Compact);
    m_list_ReleaseInfo.clear();
    return true;
}
