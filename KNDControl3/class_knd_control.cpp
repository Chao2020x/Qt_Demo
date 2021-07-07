#include "class_knd_control.h"

Class_KND_Control::Class_KND_Control(QObject *parent) : QObject(parent)
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    connect(this, SIGNAL(sig_parameter_init_net_BBJ()),
            this, SLOT(parameter_init_net_BBJ()));
    QTimer::singleShot(2000, this, [=]()
                       { emit sig_parameter_init_net_BBJ(); });
}

Class_KND_Control::~Class_KND_Control()
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    //连接涉及的参数
    m_is_parameter_init = false;     //基本参数是否进行了初始化
    m_int_port = 0;                  //侦听的端口
    m_secs_warning_keep = 30;        //持续报警的时间（毫秒）
    m_count_net_bbj_grounp = 0;      //配置的继电器组数量
    m_stringList_net_BBJ_IP.clear(); //继电器组的IP链表
    m_list_stru_net_bbj_use_once_port.clear();
    m_list_stru_net_bbj_wait_to_open.clear();
    m_list_stru_net_bbj_wait_to_close.clear();
    m_stringList_net_BBJ.clear(); //网络声光报警器相关参数表头的字符串链表

    delete m__stru_net_bbj_wait_to_openOrClose;

    m_timer_wait_to_open->stop();
    delete m_timer_wait_to_open;

    m_timer_wait_to_close->stop();
    delete m_timer_wait_to_close;

    m_timer_all_to_close->stop();
    delete m_timer_all_to_close;

    m_commandSend22KND.clear(); //发送命令
    m_commandRece22KND.clear(); //结束命令
    m_tcpSocketConnectIsOk = false;

    m_tcpSocket->abort();
    delete m_tcpSocket;
}

void Class_KND_Control::stringList_init_net_BBJ()
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    m_stringList_net_BBJ.clear();
    m_stringList_net_BBJ
        << "net_BBJ"               //0 表头
        << "port"                  //1 声光报警器的开放端口---全部的端口号一致，不要设置不同的端口号
        << "second_warning_remain" //2 剩余的报警持续时间（秒）
        << "count_grounp"          //3 继电器组的数量
        << "relayIP"               //4 继电器组的IP头---
        << "str_ip"                //5 声光报警器的网络IP
        << "area"                  //6 管辖的防区
        << "len_start"             //7 声光报警器管辖的长度-开始
        << "len_end"               //8 声光报警器管辖的长度-结束
        << "relay_ID"              //9 该区域内的继电器ID，其中ID的百位/千位代表继电器组的编号，个位和十位代表驱动闭合的端口，
        << "other";                // 例如1202 代表第12个继电器组的02号端口进行闭合
    return;
}

void Class_KND_Control::parameter_init_net_BBJ()
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    //连接涉及的参数
    m_is_parameter_init = false;     //基本参数是否进行了初始化
    m_int_port = 0;                  //侦听的端口
    m_secs_warning_keep = 30;        //持续报警的时间（毫秒）
    m_count_net_bbj_grounp = 0;      //配置的继电器组数量
    m_stringList_net_BBJ_IP.clear(); //继电器组的IP链表
    m_list_stru_net_bbj_use_once_port.clear();
    m_list_stru_net_bbj_wait_to_open.clear();
    m_list_stru_net_bbj_wait_to_close.clear();
    m_stringList_net_BBJ.clear(); //网络声光报警器相关参数表头的字符串链表
    m__stru_net_bbj_wait_to_openOrClose = nullptr;
    m_timer_wait_to_open = nullptr;
    m_timer_wait_to_close = nullptr;
    m_timer_all_to_close = nullptr;
    m_commandSend22KND.clear(); //发送命令
    m_commandRece22KND.clear(); //结束命令
    m_tcpSocketConnectIsOk = false;
    m_tcpSocket = nullptr;

    stringList_init_net_BBJ();
    load_parameter_from_file_net_bbj();
    create_timer();
    createTcpSocket222ConnectSignalSlot();
    slot_TimeWorking222CloseAll(); //初始化后关闭所有继电器DO口
    return;
}

void Class_KND_Control::load_parameter_from_file_net_bbj()
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    /*
     * 一开始默认基本参数初始化失败
     * 文件读取错误，就可以保证该变量为false
     * 如果文件读取成功，函数执行完最后一步改为true
     * 保证逻辑正确
     */
    m_is_parameter_init = false;

    QFileInfo t_fileInfo(INIFILEPATH_NET_BBJ);
    if (!t_fileInfo.exists())
    {
        return;
    }

    //Qt中使用QSettings类读写ini文件
    //QSettings构造函数的第一个参数是ini文件的路径,第二个参数表示针对ini文件,第三个参数可以缺省
    QSettings *configIniRead = new QSettings(INIFILEPATH_NET_BBJ, QSettings::IniFormat);
    configIniRead->setIniCodec(QTextCodec::codecForName("GBK"));

    //首先要读取的是控制的总部分：端口号，报警持续时间，继电器组的数量
    configIniRead->beginGroup(m_stringList_net_BBJ.at(0));
    m_int_port = configIniRead->value(m_stringList_net_BBJ.at(1)).toInt();
    m_secs_warning_keep = configIniRead->value(m_stringList_net_BBJ.at(2)).toInt();
    m_count_net_bbj_grounp = configIniRead->value(m_stringList_net_BBJ.at(3)).toInt();
    configIniRead->endGroup();


#if 1
    qDebug() << "m_int_port = " << m_int_port
             << "m_secs_warning_keep = " << m_secs_warning_keep
             << "m_count_net_bbj_grounp = " << m_count_net_bbj_grounp;
#endif

    //需要判断相关参数是不是合法
    if (m_int_port <= 1)
    {
        return;
    }
    if (m_secs_warning_keep <= 1)
    {
        m_secs_warning_keep = 30;
        return;
    }
    if (m_count_net_bbj_grounp < 1)
    {
        return;
    }
    if (m_count_net_bbj_grounp >= NET_BBJ_GROUNP_COUNT_MAX)
    {
        return;
    }

    //然后按照继电器组的数量，读取每个继电器组的IP地址
    configIniRead->beginGroup(m_stringList_net_BBJ.at(4));
    for (int i = 0; i < m_count_net_bbj_grounp; i++)
    {
        QString t_str_key = QString("%2%3").arg(m_stringList_net_BBJ.at(5)).arg(i);
        m_stringList_net_BBJ_IP.push_back(configIniRead->value(t_str_key).toString());

        qDebug() << m_count_net_bbj_grounp << " ip = " << m_stringList_net_BBJ_IP.at(i);
    }
    configIniRead->endGroup();

    //最后按照继电器组的数量*每个继电器可以控制的端口数量，读取可能全部的端口控制信息
    for (int area = 0; area < m_count_net_bbj_grounp * NET_BBJ_COUNT_MAX_PER_GROUNP; area++)
    {
        QString t_str_key = m_stringList_net_BBJ.at(6);
        t_str_key += QString::number(area);

        configIniRead->beginGroup(t_str_key);
        if(configIniRead->contains(m_stringList_net_BBJ.at(7))) {
            double t_len_start = configIniRead->value(m_stringList_net_BBJ.at(7)).toDouble();
            double t_len_end = configIniRead->value(m_stringList_net_BBJ.at(8)).toDouble();
            int t_relay_ID = configIniRead->value(m_stringList_net_BBJ.at(9)).toInt();

            STRU_NET_BBJ_USE_ONCE_PORT *t_stru_net_bbj_use_once_port = new STRU_NET_BBJ_USE_ONCE_PORT(t_len_start,t_len_end,t_relay_ID);
            qDebug() << t_stru_net_bbj_use_once_port->s_len_start << t_stru_net_bbj_use_once_port->s_len_end << t_stru_net_bbj_use_once_port->s_relay_ID;
            m_list_stru_net_bbj_use_once_port.push_back(t_stru_net_bbj_use_once_port);

        }
        configIniRead->endGroup();
        continue;
    }

    delete configIniRead;

    /*基本参数初始化成功*/
    m_is_parameter_init = true;
    return;
}

void Class_KND_Control::create_timer()
{

    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    /*
     * m_timer_wait_to_open 控制继电器DO口打开
     * 500毫秒
     */
    m_timer_wait_to_open = new QTimer(this);
    m_timer_wait_to_open->setInterval(1000);

    connect(m_timer_wait_to_open, SIGNAL(timeout()),
            this, SLOT(slot_TimeWorking222Open()));

    m_timer_wait_to_open->start();

    /*
     * m_timer_wait_to_close 控制继电器DO口关闭
     * 1秒执行一次
     */
    m_timer_wait_to_close = new QTimer(this);
    m_timer_wait_to_close->setInterval(1000);
    connect(m_timer_wait_to_close, SIGNAL(timeout()),
            this, SLOT(slot_TimeWorking222Close()));

    m_timer_wait_to_close->start();

    /*
     * m_timer_all_to_close 控制所有继电器DO口关闭
     * 10分钟执行一次
     */
    m_timer_all_to_close = new QTimer(this);
    m_timer_all_to_close->setInterval(1000 * 60 *10);
    connect(m_timer_all_to_close, SIGNAL(timeout()),
            this, SLOT(slot_TimeWorking222CloseAll()));

    m_timer_all_to_close->start();
    return;
}

void Class_KND_Control::createTcpSocket222ConnectSignalSlot()
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    m_tcpSocketConnectIsOk = false;
    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket, SIGNAL(connected()),
            this, SLOT(slot_tcpSocketConnected()));

    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(slot_tcpSocketDisconnected()));

    connect(m_tcpSocket, SIGNAL(disconnected()),
            this, SLOT(slot_tcpSocketDisconnected()));

    connect(m_tcpSocket, SIGNAL(readyRead()),
            this, SLOT(slot_tcpSocketReadData()));
}

bool Class_KND_Control::setTcpSocketInfo22Connect(const QString &t_ip, int t_port)
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    if (m_tcpSocket == nullptr)
    {
        return false;
    }
    emit sig_tcpISConnectInfo(t_ip);

    m_tcpSocket->abort();
    m_tcpSocket->connectToHost(QHostAddress(t_ip), t_port);
    return m_tcpSocket->waitForConnected(500);
}

void Class_KND_Control::slot_tcpSocketConnected()
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    m_tcpSocketConnectIsOk = true;
    //    append(0, "服务器连接");
    emit sig_tcpISConnectState(m_tcpSocketConnectIsOk);
}

void Class_KND_Control::slot_tcpSocketDisconnected()
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    m_tcpSocketConnectIsOk = false;
    //    append(1, "服务器断开");
    emit sig_tcpISConnectState(m_tcpSocketConnectIsOk);
}

void Class_KND_Control::slot_tcpSocketReadData()
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    int compare_result = 9999;
    m_commandRece22KND.clear();
    QByteArray readData = m_tcpSocket->readAll();
    bool t_BBJControl = false;
    QString t_BBJControlInfo = QString("[ReadData]-->IP:%1、DON:%2、relay_ID:%3 -->[State:%4]").arg(
                m__stru_net_bbj_wait_to_openOrClose->s_str_IP).arg(
                m__stru_net_bbj_wait_to_openOrClose->s_DON_channal+1).arg(
                m__stru_net_bbj_wait_to_openOrClose->s_relay_ID);

    if (readData.length() <= 0)
    {
        t_BBJControl = false;
        goto ReadData_end;
    }
    m_commandRece22KND = byteArrayToHexStr(readData);
    qDebug() << m_commandRece22KND;

    if (m_commandSend22KND.length() != m_commandRece22KND.length())
    {
        t_BBJControl = false;
        goto ReadData_end;
    }
    compare_result = QString::compare(m_commandRece22KND, m_commandSend22KND, Qt::CaseInsensitive);
    if (compare_result == 0)
    {
        t_BBJControl = true;
        if (m__stru_net_bbj_wait_to_openOrClose->s_wait_to_open)
        {
            //打开成功 等待关闭
            m__stru_net_bbj_wait_to_openOrClose->s_wait_to_open = false;
            m__stru_net_bbj_wait_to_openOrClose->s_wait_to_close = true;
            goto ReadData_end;
        }
        if (m__stru_net_bbj_wait_to_openOrClose->s_wait_to_close)
        {
            //关闭成功 释放内存
            m__stru_net_bbj_wait_to_openOrClose->s_wait_to_open = false;
            m__stru_net_bbj_wait_to_openOrClose->s_wait_to_close = false;
            goto ReadData_end;
        }
    }
    else
    {
        t_BBJControl = false;
        if (m__stru_net_bbj_wait_to_openOrClose->s_wait_to_open)
        {
            //打开失败 等待再次打开
            m__stru_net_bbj_wait_to_openOrClose->s_wait_to_open = true;
            m__stru_net_bbj_wait_to_openOrClose->s_wait_to_close = false;
            goto ReadData_end;
        }
        if (m__stru_net_bbj_wait_to_openOrClose->s_wait_to_close)
        {
            //关闭失败
            m__stru_net_bbj_wait_to_openOrClose->s_wait_to_open = false;
            m__stru_net_bbj_wait_to_openOrClose->s_wait_to_close = true;
            goto ReadData_end;
        }
    }

ReadData_end:
    emit sig_tcp22BBJControlInfo(t_BBJControlInfo.arg(t_BBJControl));
    if (m__stru_net_bbj_wait_to_openOrClose->s_wait_to_open)
    {
        emit sig_AddBBJControlInfoList22Show(
                    m__stru_net_bbj_wait_to_openOrClose->s_relay_ID,false);

        slot_BBJControl222WaitOpenList(m__stru_net_bbj_wait_to_openOrClose);
        m__stru_net_bbj_wait_to_openOrClose = nullptr;
        return;
    }
    if (m__stru_net_bbj_wait_to_openOrClose->s_wait_to_close)
    {
        emit sig_AddBBJControlInfoList22Show(
                    m__stru_net_bbj_wait_to_openOrClose->s_relay_ID,true);

        slot_BBJControl222WaitCloseList(m__stru_net_bbj_wait_to_openOrClose);
        m__stru_net_bbj_wait_to_openOrClose = nullptr;
        return;
    }

    emit sig_AddBBJControlInfoList22Show(
                m__stru_net_bbj_wait_to_openOrClose->s_relay_ID,false);

    delete m__stru_net_bbj_wait_to_openOrClose;
    m__stru_net_bbj_wait_to_openOrClose = nullptr;
    return;
}

void Class_KND_Control::slot_tcpSocketSendData(STRU_NET_BBJ_CONTROL_STATUS *each_struct)
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();

    if (m_tcpSocket == nullptr)
    {
        delete m__stru_net_bbj_wait_to_openOrClose;
        m__stru_net_bbj_wait_to_openOrClose = nullptr;
        return;
    }

    bool t_isConnect333 = false;
    t_isConnect333 = setTcpSocketInfo22Connect(each_struct->s_str_IP, each_struct->s_port);
    if (!t_isConnect333)
    {
        QString t_BBJControlInfo = QString("[TCP Connect Fail]-->IP:%1、DON:%2").arg(
                    m__stru_net_bbj_wait_to_openOrClose->s_str_IP).arg(
                    m__stru_net_bbj_wait_to_openOrClose->s_DON_channal+1);

        emit sig_tcp22BBJControlInfo(t_BBJControlInfo);
        delete m__stru_net_bbj_wait_to_openOrClose;
        m__stru_net_bbj_wait_to_openOrClose = nullptr;
        return;
    }

    QString t_BBJControlInfo = QString("[SendData]-->IP:%1、DON:%2、relay_ID:%3 -->[State:%4]").arg(
                m__stru_net_bbj_wait_to_openOrClose->s_str_IP).arg(
                m__stru_net_bbj_wait_to_openOrClose->s_DON_channal+1).arg(
                m__stru_net_bbj_wait_to_openOrClose->s_relay_ID);

    QString senddata = "00 01 00 00 00 06 FF 05 00 %1 %2 00";

    m_commandSend22KND.clear();
    QString t_closeOrOpen = "FF";
    if (each_struct->s_wait_to_open)
    {
        //打开
        t_closeOrOpen = "FF";
        goto SendData_end;
    }
    if (each_struct->s_wait_to_close)
    {
        //关闭
        t_closeOrOpen = "00";
        goto SendData_end;
    }

SendData_end:
    emit sig_tcp22BBJControlInfo(t_BBJControlInfo.arg(t_closeOrOpen));
    m_commandSend22KND = senddata.arg(64 + each_struct->s_DON_channal).arg(t_closeOrOpen);
    m_tcpSocket->write(hexStrToByteArray(m_commandSend22KND));
    return;
}

void Class_KND_Control::slot_messageParse222BBJControl(double len)
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    /*校验*/
    if (!m_is_parameter_init)
    {
        qDebug() << "m_is_parameter_init = false";
        return;
    }

    if (m_list_stru_net_bbj_use_once_port.isEmpty())
    {
        qDebug() << "m_list_stru_net_bbj_use_once_port is Empty";
        return;
    }

    if (len < 0)
    {
        qDebug() << "len < 0";
        return;
    }

    if (m_stringList_net_BBJ_IP.isEmpty())
    {
        qDebug() << "m_stringList_net_BBJ_IP is Empty";
        return;
    }

    for (const auto each_done : m_list_stru_net_bbj_use_once_port)
    {
        //判断区间的时候采用的是左闭区间，即是[start,len)
        if (len < each_done->s_len_start)
        {
            continue;
        }
        if (len >= each_done->s_len_end)
        {
            continue;
        }
        if (each_done->s_relay_ID < 0)
        {
            continue;
        }

        int t_grounp = each_done->s_relay_ID / 100;
        if (t_grounp >= m_count_net_bbj_grounp)
        {
            break;
        }
        if (t_grounp >= m_stringList_net_BBJ_IP.size())
        {
            break;
        }

        int t_channal = each_done->s_relay_ID % 100;
        if (t_channal >= (0xff - 0x64))
        {
            break;
        }

        QString str22_net_BBJ_IP = m_stringList_net_BBJ_IP.at(t_grounp);
        if (str22_net_BBJ_IP.isEmpty())
        {
            break;
        }

        STRU_NET_BBJ_CONTROL_STATUS *t_only_BBJ = new STRU_NET_BBJ_CONTROL_STATUS;
        t_only_BBJ->s_relay_ID = each_done->s_relay_ID;
        t_only_BBJ->s_DON_channal = t_channal;
        t_only_BBJ->s_str_IP = str22_net_BBJ_IP;
        t_only_BBJ->s_port = m_int_port;
        t_only_BBJ->s_wait_to_open = true;
        slot_BBJControl222WaitOpenList(t_only_BBJ);
    }
    return;
}
