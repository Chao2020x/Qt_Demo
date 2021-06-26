#include "mqttclient.h"

/**
 *  消息发布
 *  topic:fiber_shake_status_tag
 *  说明:光纤振动检测数据
 */

void MQttClient::slot_SendMessage222MQtt(class_calculation_result *calss_result)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    //new一个ReleaseMessage
    ReleaseMessage *rel_mg = new ReleaseMessage();

    //数据解析成JSON
    contentParse222JSON(rel_mg,calss_result);

    //加入列表
    slot_SendMessage222MQtt(rel_mg);
}

bool MQttClient::contentParse222JSON(ReleaseMessage *message, class_calculation_result *calss_result)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();

    message->Topic = "fiber_shake_status_tag";
    message->Qos = 0;
    message->Retained = false;
    message->Text.clear();

    QStringList m_list_ReleaseInfo;
    m_list_ReleaseInfo.clear();
    m_list_ReleaseInfo << "version"       //0 表示协议版本号，当前版本号是1.0。
                       << "id"            //1 表示光纤唯一标识。组成结构为: 公司标识+电房标识+光纤测振主机设备id+光纤通道id。
                       << "ip"            //2 表示测振设备的ip
                       << "is_inspect"    //3 是否巡检返回的数据，0为正常周期任务生成的数据，1为巡检命令生成的数据。
                       << "time"          //4 表示数据上传的时间。格式为YYYY-MM-DD HH24:MM:SS
                       << "accuracy"      //5 表示测振精度值，单位：米。

                       << "fibParams"     //6 监测设备采集所用到的参数，fibParams是一个字符串，各厂商根据自身情况，将相关的重要光纤测振参数拼接在该字符串中（如特发设备的距离、折射率、脉宽、电流等）。
                       << "data"          //7 监测设备采集的数值，data是一个json串，里面字段内容第一个双引号为数据名称，第二个双引号为数据值。
                       << "statusdatetime"//8 表示数据采集的时间点，如“2018-11-20 15:58:30”（数据类型： string）
                       << "statusshake"   //9 光纤上从长度0m开始，当前各测振点依次的振动值。（数据类型：array[float]）
                          ;

    QJsonObject aloneObject;

    QSettings setting(MQttConf,QSettings::IniFormat);

    //QString 表示协议版本号，当前版本号是1.0。
    QString valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(0);
    QString _version = setting.value(valueConf).toString();
    if(_version.isEmpty()) {_version = "1.0";}
    aloneObject.insert(m_list_ReleaseInfo.at(0),_version);

    //QString 表示光纤唯一标识。组成结构为: 公司标识+电房标识+光纤测振主机设备id+光纤通道id。
    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(1);
    QString _id = setting.value(valueConf).toString();
    if(_id.isEmpty()) {_id = "GZDLGSYOSCER";}
    _id += QString::number(calss_result->m_OSW_model_when_warning);
    aloneObject.insert(m_list_ReleaseInfo.at(1),_id);

    //QString 表示测振设备的ip
    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(2);
    QString _ip = setting.value(valueConf).toString();
    if(_ip.isEmpty()) {_ip = "196.128.0.12";}
    aloneObject.insert(m_list_ReleaseInfo.at(2),_ip);

    //QString 是否巡检返回的数据，0为正常周期任务生成的数据，1为巡检命令生成的数据。
    valueConf = message->Topic +"/"+ m_list_ReleaseInfo.at(3);
    QString _is_inspect = setting.value(valueConf).toString();
    if(_is_inspect.isEmpty()) {_is_inspect = "0";}
    aloneObject.insert(m_list_ReleaseInfo.at(3),_is_inspect);

    //QString 表示数据上传的时间。格式为YYYY-MM-DD HH24:MM:SS
    aloneObject.insert(m_list_ReleaseInfo.at(4), QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    //double 表示测振精度值，单位：米。
    aloneObject.insert(m_list_ReleaseInfo.at(5),calss_result->m_lenght_all);

    //QString 监测设备采集所用到的参数，fibParams是一个字符串，
    //光纤总长、置信度、报警名称
    QString fibParams_str = QString("length:%1,confidence:%2,warning_name:%3").arg(
                calss_result->m_lenght_all).arg(calss_result->m_confidence_last).arg(
                calss_result->m_string_warning_name);
    aloneObject.insert(m_list_ReleaseInfo.at(6),fibParams_str);


    QJsonObject dataObject;
    QString datatime222 = m_systemInfo.analysis_dir_name_from_file(calss_result->m_file_name);
    dataObject.insert(m_list_ReleaseInfo.at(8),datatime222);

    //一个波形数组
    QJsonArray pos_mapArray;
    read_data_from_file(pos_mapArray,calss_result->m_file_name);
    dataObject.insert(m_list_ReleaseInfo.at(9),QJsonValue(pos_mapArray));

    aloneObject.insert(m_list_ReleaseInfo.at(7),dataObject);
    // 构建 Json 文档
    QJsonDocument document;
    document.setObject(aloneObject);
    message->Text = document.toJson(QJsonDocument::Compact);

    m_list_ReleaseInfo.clear();
}



#include <QFile>
#define DATAFROMFILELENMAX  (1024*1024/4)   //开辟一个固定大小的singned chr 的大小不能超出1024 * 1024个那么大的空间，否则程序崩溃
bool MQttClient::read_data_from_file(QJsonArray &jsonArray,const QString &file_name)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    //准确性判断
    if(file_name.isEmpty())
    {
        return false;
    }
    long m_len_4M_Data_From_File = 0;
    QString t_file_path_read = "D:\\dataBackup";   //数据保存文件夹位置
    t_file_path_read += "\\";
    t_file_path_read += file_name;

    QFile datafile(t_file_path_read);
    //判断文件是否存在
    if(!datafile.exists())
    {
        return false;
    }

    //如果文件名称不符合标准
    if(!m_systemInfo.is_file_name_up_to_standard(file_name))
    {
        qDebug() << "data file name error " ;
        return false;
    }

    //文件基本信息判断：长度需要落在设置的准确性范围内
    long t_len_file = datafile.size();
    if(t_len_file <= 1)
    {
        return false;
    }
    qDebug() << "t_len_file = " << t_len_file;


    //否则开始读取数据
    //以读写方式打开
    if(!datafile.open(QIODevice::ReadWrite))
    {
        datafile.close();
        return false;
    }

    //不管数据文件的内容是什么，照读~~~交给后续的函数去处理是否准确或者有用
    m_len_4M_Data_From_File = 0;
    //提取具体的值到链表中
    QByteArray t_byteArray = datafile.readAll();
    //之前的想打里面是直接砍掉文件中的数据的前面128个字节的数据（即前面32个数据），现在取消/可以调节这128的值（长度是32）
    for(long t_dataPos = 0; t_dataPos < ((long int)((t_len_file - 4 * 2) / 4)) * 4 - 3; )
    {
        if(((((unsigned char)t_byteArray.at(t_dataPos + 1)) / 16) == 9) && ((((unsigned char)t_byteArray.at(t_dataPos + 3)) / 16) == 10))
        {

//            g_data_4M_once[0][g_len_4M_Data_From_File]= (((unsigned char)t_byteArray.at(t_dataPos + 1)) % 16) * 256
//                    + ((unsigned char)t_byteArray.at(t_dataPos + 0));
//            g_data_4M_once[1][g_len_4M_Data_From_File] = (((unsigned char)t_byteArray.at(t_dataPos + 3)) % 16) * 256
//                    + ((unsigned char)t_byteArray.at(t_dataPos + 2));

            jsonArray.append((((unsigned char)t_byteArray.at(t_dataPos + 1)) % 16) * 256
                                                 + ((unsigned char)t_byteArray.at(t_dataPos + 0)));
            m_len_4M_Data_From_File ++;
            if(m_len_4M_Data_From_File >= DATAFROMFILELENMAX){break;}
            t_dataPos += 4;
        }
        else if(((((unsigned char)t_byteArray.at(t_dataPos + 1)) / 16) == 10) && ((((unsigned char)t_byteArray.at(t_dataPos + 3)) / 16) == 9))
        {
            jsonArray.append((((unsigned char)t_byteArray.at(t_dataPos + 3)) % 16) * 256
                                                 + ((unsigned char)t_byteArray.at(t_dataPos + 2)));


//            g_data_4M_once[1][g_len_4M_Data_From_File]= (((unsigned char)t_byteArray.at(t_dataPos + 1)) % 16) * 256
//                    + ((unsigned char)t_byteArray.at(t_dataPos + 0));
//            g_data_4M_once[0][g_len_4M_Data_From_File] = (((unsigned char)t_byteArray.at(t_dataPos + 3)) % 16) * 256
//                    + ((unsigned char)t_byteArray.at(t_dataPos + 2));

            m_len_4M_Data_From_File ++;
            if(m_len_4M_Data_From_File >= DATAFROMFILELENMAX){break;}
            t_dataPos += 4;
        }
        else
        {
            t_dataPos ++;
        }
    }
    datafile.close();
    return true;
}
