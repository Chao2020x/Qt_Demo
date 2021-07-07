#include "class_knd_control.h"

void Class_KND_Control::slot_TimeWorking222Open()
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();

    if (m__stru_net_bbj_wait_to_openOrClose != nullptr)
    {
        return;
    }
    if (m_list_stru_net_bbj_wait_to_open.isEmpty())
    {
        return;
    }

    m__stru_net_bbj_wait_to_openOrClose = m_list_stru_net_bbj_wait_to_open.first();
    m_list_stru_net_bbj_wait_to_open.removeFirst();

    if (m__stru_net_bbj_wait_to_openOrClose->IsTimeout())
    {
        //连续5分钟操作失败的那组DO口、删除不要
        delete m__stru_net_bbj_wait_to_openOrClose;
        m__stru_net_bbj_wait_to_openOrClose = nullptr;
        return;
    }
    relay_control_done_Open();
    return;
}

void Class_KND_Control::relay_control_done_Open()
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    if (m__stru_net_bbj_wait_to_openOrClose == nullptr)
    {
        return;
    }
    m__stru_net_bbj_wait_to_openOrClose->s_wait_to_close = false;
    m__stru_net_bbj_wait_to_openOrClose->s_wait_to_open = true;
    slot_tcpSocketSendData(m__stru_net_bbj_wait_to_openOrClose);
    return;
}

void Class_KND_Control::slot_BBJControl222WaitOpenList(STRU_NET_BBJ_CONTROL_STATUS *each_struct)
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    //先判断是否是在 待关闭容器里
    if (!m_list_stru_net_bbj_wait_to_close.isEmpty())
    {
        for (auto each_close : m_list_stru_net_bbj_wait_to_close)
        {
            if (each_close->s_relay_ID == each_struct->s_relay_ID)
            {
                qDebug() << "=================================================";
                each_close->addAlarmTime();
                delete each_struct;
                each_struct = nullptr;
                return;
            }
        }
    }

    //再判断是否是在 待打开容器里
    if (!m_list_stru_net_bbj_wait_to_open.isEmpty())
    {
        for (auto each_open : m_list_stru_net_bbj_wait_to_open)
        {
            if (each_open->s_relay_ID == each_struct->s_relay_ID)
            {
                each_open->addAlarmTime();
                delete each_struct;
                each_struct = nullptr;
                return;
            }
        }
    }

    m_list_stru_net_bbj_wait_to_open.append(each_struct);
    return;
}
