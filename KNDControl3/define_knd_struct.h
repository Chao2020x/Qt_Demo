#ifndef DEFINE_KND_STRUCT_H
#define DEFINE_KND_STRUCT_H

#include <QDateTime>

//以下是网络声光报警器列表相关的参数
#define VERSION_NET_BBJ_COUNT_MAX_IS_8 1 //最多可以控制的继电器个数为8个，
#if VERSION_NET_BBJ_COUNT_MAX_IS_8
#define NET_BBJ_COUNT_MAX_PER_GROUNP 8 //最多可以控制的继电器个数为8个，
#else
#define VERSION_NET_BBJ_COUNT_MAX_IS_16 1 //最多可以控制的继电器个数为8个，
#if VERSION_NET_BBJ_COUNT_MAX_IS_16
#define NET_BBJ_COUNT_MAX_PER_GROUNP 16 //最多可以控制的继电器个数为8个，
#endif
#endif

#define DEBUG_NET_BBJ_ENABLE 1      //打印调试信息
#define BBJ_COUNT_MAX 32            //99      //允许控制的网络声光报警器最大数量
#define BBJ_COUNT_PER_CH 8          //允许配置防区数
#define BBJ_COUNT_PER_ROW 32        //64      //表格最多显示声光报警器数量
#define COUNT_NET_BBJ_MAX_PER_CH 32 //8    //一个防区最多配置的个声光报警器
#define INIFILEPATH_NET_BBJ "config\\net_BBJ.ini"
#define BBJ_COUNT_PER_EQU 8 //每个继电器可以驱动的声光报警器数量
#define DIRNETBBJ "net_BBJ"
#define NET_BBJ_GROUNP_COUNT_MAX 250 //最多仅仅支持250个继电器组

//单个区域涉及的继电器信息
struct STRU_NET_BBJ_USE_ONCE_PORT
{
    /*距离（区域）控制及对应信息
     * s_len_start 起始长度
     * s_len_end 终止长度
     * 左闭右开区间 [s_len_start,s_len_end)
     * s_relay_ID 继电器信息编号
     */
    double s_len_start;
    double s_len_end;
    int s_relay_ID;

    STRU_NET_BBJ_USE_ONCE_PORT()
    {
        s_len_start = 0;
        s_len_end = 1;
        s_relay_ID = 0;
    }

    STRU_NET_BBJ_USE_ONCE_PORT(double t_start,double t_end ,int t_id)
    {
        s_len_start = t_start;
        s_len_end = t_end;
        s_relay_ID = t_id;
    }
};

//2021年6月6日 新增加
struct STRU_NET_BBJ_CONTROL_STATUS
{
    /*套接字的IP地址*/
    QString s_str_IP;
    /*端口号*/
    int s_port;
    /*继电器信息编号*/
    int s_relay_ID;
    /*DO口*/
    int s_DON_channal;
    /*
     * 待打开的继电器DO口
     * true:表示待打开、打开失败等待下一次打开
     * false:表示打开完成、不需要打开
     */
    bool s_wait_to_open;
    /*
     * 待关闭的继电器DO口
     * true:表示待关闭、关闭失败等待下一次关闭
     * false:表示关闭完成、不需要关闭
     */
    bool s_wait_to_close;
    /*
     * 每个端口DO口剩余的关闭时间
     * 打开后的继电器DO口到关闭之间的时间间隔
     * 最长时间不能超过2分钟
     */
    int s_second_remain_to_close;
    /*
     * 是否延长声光报警时间 控制 s_second_remain_to_close
     */
    bool s_isCanOvertime;
    /*
     * 报警发生时间
     * 作用:用于一直打开失败超过一定时间时，不在做打开操作
     * （不用于）一直关闭失败超过一定时间时
     */
    QDateTime s_dataTime;
    /*
     * 若打开失败则继续打开的循环中，时间不需超过该值
     * 一般设置为5分钟，连续5分钟操作失败，即不在操作
     */
    int s_minuteTimeoutLimit;

    STRU_NET_BBJ_CONTROL_STATUS()
    {
        s_str_IP = ""; //套接字的IP地址
        s_port = 0;
        s_relay_ID = 0;
        s_DON_channal = 0;

        s_wait_to_close = false;
        s_wait_to_open = false;
        s_second_remain_to_close = 20;
        s_isCanOvertime = true;

        s_dataTime = QDateTime::currentDateTime();
        s_minuteTimeoutLimit = 5;
    }

    void addAlarmTime(int secs = 20)
    {
        if(!s_isCanOvertime){
            return;
        }

        s_second_remain_to_close += secs;
        if (s_second_remain_to_close >= 120)
        {
            s_second_remain_to_close = 120;
            s_isCanOvertime = false;
        }
    }

    bool IsTimeout()
    {
        QDateTime t_dataTime_now = QDateTime::currentDateTime();
        QDateTime t_dataTime_add = s_dataTime.addSecs(s_minuteTimeoutLimit * 60);
        return (t_dataTime_now > t_dataTime_add);
    }

    bool IsClose()
    {
        if (s_second_remain_to_close <= 0)
        {
            return true;
        }
        return false;
    }
};

/*配置的方法：
 * 文件名称：net_BBJ.ini
 * [net_BBJ]
 * port=502
 * second_warning_remain = 30
 * count_grounp=10
 *
 * [relayIP]
 * str_ip0=192.168.1.53
 * str_ip2=192.168.1.55
 * ...
 * str_ipN=192.168.1.155
 *
 * [area0]
 * len_start=0
 * len_end=1
 * relay_ID=1
 *
 * [area1]
 * len_start=0
 * len_end=1
 * relay_ID=2
 *
 * ...
 *
 * [area12]
 * len_start=1
 * len_end=2
 * relay_ID=103
 *
 * [area13]
 * len_start=1
 * len_end=2
 * relay_ID=104
 * ...
 * [areaN]
 * len_start=N
 * len_end=(N+1)
 * relay_ID=((N+1)*100 + M)
 *
 *
 *
 *
 */

#endif // DEFINE_KND_STRUCT_H
