#ifndef INFO_H
#define INFO_H

#include <QObject>
#include <QString>
#include <windows.h>
#include <QDebug>


#include <QHostAddress>
#include <QNetworkInterface>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSysInfo>
#include "QSettings"
#include "QDebug"
#include <QDesktopWidget>
#include <QFileInfoList>
#include <QDir>
#include <QLibrary>
#include <QTimer>
#include <QHostInfo>


#ifdef Q_OS_WIN
#include "windows.h"
#endif
#define GB (1024 * 1024 * 1024)
#define MB (1024 * 1024)
#define KB (1024)

#define DEBUG_FUNC 1

class Info
{
public:
    Info();
    float m_totalMem,m_freeMem; //总内存、可用内存
    float m_useDisk,m_freeDisk,m_allDisk;//已用磁盘容量、可用磁盘容量、总磁盘容量

public slots:
    bool ISIPLive();                     //判断计算机是否可以联网
    float GetCPUPercent(void);           //获取CPU占用百分比
    const QString GetCPUType();          //获取计算机CPU信息
    const QString GetLocalMachineName(); //获取计算机名称
    const QString GetIP();               //本地IP地址

    const QString GetMemoryInfo();       //获取计算机内存
    float GetMemoryUsedPercent();        //获取计算机已用内存占比
    const QString GetDiskInfo();         //获取磁盘信息
    float GetDiskUsedPercent();          //获取计算机已用磁盘占比
    QString analysis_dir_name_from_file(const QString &file_name); //从文件名中获取时间
    bool is_file_name_up_to_standard(const QString &file_name);//判断文件名称是否符合标准
};

#endif // INFO_H
