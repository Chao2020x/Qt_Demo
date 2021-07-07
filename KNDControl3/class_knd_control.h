#ifndef CLASS_KND_CONTROL_H
#define CLASS_KND_CONTROL_H

#include <QObject>
#include <QTimer>
#include <QFileInfo>
#include <QFile>
#include <QSettings>
#include <QTextCodec>
#include <QTcpSocket>
#include <QThread>
#include <QHostAddress>
#include <QEventLoop>
#include "define_knd_struct.h"

class Class_KND_Control : public QObject
{
    Q_OBJECT
public:
    explicit Class_KND_Control(QObject *parent = nullptr);
    ~Class_KND_Control();

private:
    //-----------------------连接涉及的参数--------------------------
    bool m_is_parameter_init;                                              //基本参数是否进行了初始化
    int m_int_port;                                                        //侦听的端口
    int m_secs_warning_keep;                                               //持续报警的时间（秒）
    int m_count_net_bbj_grounp;                                            //配置的继电器组数量
    QStringList m_stringList_net_BBJ_IP;                                   //继电器组的IP链表
    QList<STRU_NET_BBJ_USE_ONCE_PORT *> m_list_stru_net_bbj_use_once_port; //单个控制信息

    QStringList m_stringList_net_BBJ; //网络声光报警器相关参数表头的字符串链表
    void stringList_init_net_BBJ();   //初始化m_stringList_net_BBJ

    void load_parameter_from_file_net_bbj(); //从配置文件中导入信息
    void create_timer();                     //创建定时器。打开的定时器、关闭的定时器

    QList<STRU_NET_BBJ_CONTROL_STATUS *> m_list_stru_net_bbj_wait_to_open;
    QList<STRU_NET_BBJ_CONTROL_STATUS *> m_list_stru_net_bbj_wait_to_close;
    STRU_NET_BBJ_CONTROL_STATUS *m__stru_net_bbj_wait_to_openOrClose;
    QTimer *m_timer_wait_to_open;  //等待打开
    QTimer *m_timer_wait_to_close; //等待关闭
    QTimer *m_timer_all_to_close;  //关闭所有

    QString m_commandSend22KND; //发送命令
    QString m_commandRece22KND; //结束命令

    void relay_control_done_Open();
    void relay_control_done_Close();

    bool m_tcpSocketConnectIsOk;
    QTcpSocket *m_tcpSocket;

    /*创建TcpSocket并连接信号槽*/
    void createTcpSocket222ConnectSignalSlot();
    /*
     * 设置TcpSocket连接信息
     * 连接成功返回 true 连接失败返回false
     */
    bool setTcpSocketInfo22Connect(const QString &t_ip, int t_port);

signals:
    void sig_parameter_init_net_BBJ(); //基本参数是否进行了初始化
    void sig_tcpISConnectInfo(const QString &t_ip);
    void sig_tcpISConnectState(bool t_isConnect);
    void sig_tcp22BBJControlInfo(const QString &t_ip);
    void sig_AddBBJControlInfoList22Show(int relay_ID,bool isOpenOrClose);

private slots:
    void parameter_init_net_BBJ(); //基本参数是否进行了初始化

    void slot_BBJControl222WaitOpenList(STRU_NET_BBJ_CONTROL_STATUS *each_struct);
    void slot_BBJControl222WaitCloseList(STRU_NET_BBJ_CONTROL_STATUS *each_struct);

    void slot_TimeWorking222Close();    //等待关闭
    void slot_TimeWorking222Open();     //等待打开
    void slot_TimeWorking222CloseAll(); //关闭所有

    void slot_tcpSocketConnected();
    void slot_tcpSocketDisconnected();
    void slot_tcpSocketReadData();
    /*
     * 作用打开或关闭单个继电器的指定DO口
     */
    void slot_tcpSocketSendData(STRU_NET_BBJ_CONTROL_STATUS *each_struct); //打开或关闭单个

public slots:
    void slot_messageParse222BBJControl(double len);

private:
    QString byteArrayToHexStr(const QByteArray &data)
    {
        QString temp = "";
        QString hex = data.toHex();

        for (int i = 0; i < hex.length(); i = i + 2)
        {
            temp += hex.mid(i, 2) + " ";
        }

        return temp.trimmed().toUpper();
    }

    char hexStrToChar(char data)
    {
        if ((data >= '0') && (data <= '9'))
        {
            return data - 0x30;
        }
        else if ((data >= 'A') && (data <= 'F'))
        {
            return data - 'A' + 10;
        }
        else if ((data >= 'a') && (data <= 'f'))
        {
            return data - 'a' + 10;
        }
        else
        {
            return (-1);
        }
    }

    QByteArray hexStrToByteArray(const QString &data)
    {
        QByteArray senddata;
        int hexdata, lowhexdata;
        int hexdatalen = 0;
        int len = data.length();
        senddata.resize(len / 2);
        char lstr, hstr;

        for (int i = 0; i < len;)
        {
            hstr = data.at(i).toLatin1();
            if (hstr == ' ')
            {
                i++;
                continue;
            }

            i++;
            if (i >= len)
            {
                break;
            }

            lstr = data.at(i).toLatin1();
            hexdata = hexStrToChar(hstr);
            lowhexdata = hexStrToChar(lstr);

            if ((hexdata == 16) || (lowhexdata == 16))
            {
                break;
            }
            else
            {
                hexdata = hexdata * 16 + lowhexdata;
            }

            i++;
            senddata[hexdatalen] = (char)hexdata;
            hexdatalen++;
        }

        senddata.resize(hexdatalen);
        return senddata;
    }
};

#endif // CLASS_KND_CONTROL_H
