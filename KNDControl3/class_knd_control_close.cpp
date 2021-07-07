#include "class_knd_control.h"

void Class_KND_Control::slot_TimeWorking222Close()
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();

    if (m__stru_net_bbj_wait_to_openOrClose != nullptr)
    {
        return;
    }
    if (m_list_stru_net_bbj_wait_to_close.isEmpty())
    {
        return;
    }

    int t_count = 0;    //循环
    int t_count222 = 0; //保存最小的一个
    int t_second_remain_to_close = 9999;
    for (auto each_close : m_list_stru_net_bbj_wait_to_close)
    {
        //减时间
        each_close->s_second_remain_to_close -= 1;
        //删除过期的 超过5分钟没有操作成功视为过期
        if (t_second_remain_to_close > each_close->s_second_remain_to_close)
        {
            t_second_remain_to_close = each_close->s_second_remain_to_close;
            t_count222 = t_count;
        }

        t_count++;
    }

    m__stru_net_bbj_wait_to_openOrClose = m_list_stru_net_bbj_wait_to_close.at(t_count222);

    qDebug() << Q_FUNC_INFO << __LINE__;
    if (m__stru_net_bbj_wait_to_openOrClose->IsTimeout())
    {
        delete m__stru_net_bbj_wait_to_openOrClose;
        m_list_stru_net_bbj_wait_to_close.removeAt(t_count222);
        m__stru_net_bbj_wait_to_openOrClose = nullptr;
        return;
    }
    qDebug() << Q_FUNC_INFO << __LINE__;
    if (m__stru_net_bbj_wait_to_openOrClose->IsClose())
    {
        m_list_stru_net_bbj_wait_to_close.removeAt(t_count222);
        relay_control_done_Close();
        return;
    }

    m__stru_net_bbj_wait_to_openOrClose = nullptr;
    return;
}

void Class_KND_Control::relay_control_done_Close()
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    if (m__stru_net_bbj_wait_to_openOrClose == nullptr)
    {
        return;
    }
    m__stru_net_bbj_wait_to_openOrClose->s_wait_to_close = true;
    m__stru_net_bbj_wait_to_openOrClose->s_wait_to_open = false;
    slot_tcpSocketSendData(m__stru_net_bbj_wait_to_openOrClose);
    return;
}

void Class_KND_Control::slot_BBJControl222WaitCloseList(STRU_NET_BBJ_CONTROL_STATUS *each_struct)
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    //先判断是否是在 待关闭容器里
    if (!m_list_stru_net_bbj_wait_to_close.isEmpty())
    {
        for (auto each_close : m_list_stru_net_bbj_wait_to_close)
        {
            if (each_close->s_relay_ID == each_struct->s_relay_ID)
            {
                each_close->addAlarmTime(20);
                delete each_struct;
                each_struct = nullptr;
                return;
            }
        }
    }

    m_list_stru_net_bbj_wait_to_close.append(each_struct);
    return;
}

void Class_KND_Control::slot_TimeWorking222CloseAll()
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

    if (m_stringList_net_BBJ_IP.isEmpty())
    {
        qDebug() << "m_stringList_net_BBJ_IP is Empty";
        return;
    }

    for (const auto each_22close : m_list_stru_net_bbj_use_once_port)
    {
        if (each_22close->s_len_start < 0)
        {
            continue;
        }
        if (each_22close->s_relay_ID < 0)
        {
            continue;
        }

        //判断再待打开列表中是否存在
        bool t_is_existOpenList = false;
        if (!m_list_stru_net_bbj_wait_to_open.isEmpty())
        {
            for (const auto each_done : m_list_stru_net_bbj_wait_to_open)
            {
                if (each_22close->s_relay_ID == each_done->s_relay_ID)
                {
                    t_is_existOpenList = true;
                    break;
                }
            }
        }
        if (t_is_existOpenList)
        {
            continue;
        }

        //判断在待关闭列表中是否存在
        bool t_is_existCloseList = false;
        if (!m_list_stru_net_bbj_wait_to_close.isEmpty())
        {
            for (const auto each_done : m_list_stru_net_bbj_wait_to_close)
            {
                if (each_22close->s_relay_ID == each_done->s_relay_ID)
                {
                    t_is_existCloseList = true;
                    break;
                }
            }
        }
        if (t_is_existCloseList)
        {
            continue;
        }

        int t_grounp = each_22close->s_relay_ID / 100;
        if (t_grounp >= m_count_net_bbj_grounp)
        {
            continue;
        }
        if (t_grounp >= m_stringList_net_BBJ_IP.size())
        {
            continue;
        }

        int t_channal = each_22close->s_relay_ID % 100;
        if (t_channal >= (0xff - 0x64))
        {
            continue;
        }

        QString str22_net_BBJ_IP = m_stringList_net_BBJ_IP.at(t_grounp);
        if (str22_net_BBJ_IP.isEmpty())
        {
            continue;
        }

        STRU_NET_BBJ_CONTROL_STATUS *t_only_BBJ = new STRU_NET_BBJ_CONTROL_STATUS;
        t_only_BBJ->s_relay_ID = each_22close->s_relay_ID;
        t_only_BBJ->s_DON_channal = t_channal;
        t_only_BBJ->s_str_IP = str22_net_BBJ_IP;
        t_only_BBJ->s_port = m_int_port;
        t_only_BBJ->s_wait_to_close = true;
        t_only_BBJ->s_second_remain_to_close = 1;
        qDebug() << "toCloseAll:" << t_only_BBJ->s_str_IP
                 << t_only_BBJ->s_port << t_only_BBJ->s_relay_ID;
        slot_BBJControl222WaitCloseList(t_only_BBJ);
    }
    return;
}
