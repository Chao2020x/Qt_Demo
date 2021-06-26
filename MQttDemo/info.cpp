#include "info.h"
#include <windows.h>
Info::Info()
{

}

float Info::GetCPUPercent()
{
    static FILETIME preidleTime;
    static FILETIME prekernelTime;
    static FILETIME preuserTime;

    FILETIME idleTime;
    FILETIME kernelTime;
    FILETIME userTime;

    GetSystemTimes(&idleTime, &kernelTime, &userTime);

    quint64 a, b;
    int idle, kernel, user;

    a = (preidleTime.dwHighDateTime << 31) | preidleTime.dwLowDateTime;
    b = (idleTime.dwHighDateTime << 31) | idleTime.dwLowDateTime;
    idle = b -a;

    a = (prekernelTime.dwHighDateTime << 31) | prekernelTime.dwLowDateTime;
    b = (kernelTime.dwHighDateTime << 31) | kernelTime.dwLowDateTime;
    kernel = b -a;

    a = (preuserTime.dwHighDateTime << 31) | preuserTime.dwLowDateTime;
    b = (userTime.dwHighDateTime << 31) | userTime.dwLowDateTime;
    user = b -a;

    float cpuPercent = (float)(kernel + user - idle) / (float)(kernel + user);

    preidleTime = idleTime;
    prekernelTime = kernelTime;
    preuserTime = userTime ;

#if DEBUG_FUNC
    qDebug()<<"CPU利用率:"<<cpuPercent;
#endif
    return cpuPercent;

}


const QString Info::GetLocalMachineName()
{
    return QHostInfo::localHostName();
}


const QString Info::GetIP()
{
    QString ip = "";
    QList<QNetworkInterface> interFaceList = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface var, interFaceList) {
        if(!var.flags().testFlag(QNetworkInterface::IsRunning)){ continue;}
        QList<QNetworkAddressEntry> entryList = var.addressEntries();
        foreach(QNetworkAddressEntry entry, entryList){
            if(QAbstractSocket::IPv4Protocol == entry.ip().protocol() &&
                   entry.ip() != QHostAddress::LocalHost &&
                    entry.ip().toString().startsWith("192.168."))
            {
                ip = entry.ip().toString();
                break;
            }
        }
    }
#if DEBUG_FUNC
    qDebug()<<"本地IP:"<<ip;
#endif
    return ip;
}


const QString Info::GetCPUType()
{
    QSettings CPU("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",QSettings::NativeFormat);
    QString m_cpuDescribe = CPU.value("ProcessorNameString").toString();
#if DEBUG_FUNC
    qDebug()<<"CPU类型:"<<m_cpuDescribe;
#endif
    return m_cpuDescribe;
}

const QString Info::GetMemoryInfo()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx(&statex);
    m_totalMem = statex.ullTotalPhys  * 1.0/ GB;
    m_freeMem = statex.ullAvailPhys * 1.0 / GB;

    QString m_memDescribe = QString("可用 %1 GB / 共 %2 GB" ).
            arg(QString::asprintf("%.2f", m_freeMem)).
            arg(QString::asprintf("%.2f", m_totalMem));
#if DEBUG_FUNC
    qDebug()<<"内存使用情况:"<<m_memDescribe;
#endif
    return m_memDescribe;
}

float Info::GetMemoryUsedPercent()
{
    GetMemoryInfo();
    float cc = (m_totalMem - m_freeMem) / m_totalMem;
    return cc;
}


const QString Info::GetDiskInfo()
{
    m_useDisk = 0.0; m_freeDisk = 0.0; m_allDisk = 0.0;

    QFileInfoList list = QDir::drives();
    foreach (QFileInfo dir, list)
    {
        QString dirName = dir.absolutePath();
        LPCWSTR lpcwstrDriver = (LPCWSTR)dirName.utf16();
        ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;
        if(GetDiskFreeSpaceEx(lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes) )
        {
            m_freeDisk += (float) liTotalFreeBytes.QuadPart / GB;
            m_allDisk += (float) liTotalBytes.QuadPart / GB;
        }
    }
    m_useDisk = m_allDisk - m_freeDisk;

    QString m_diskDescribe = QString("总磁盘容量 :%1 G;已用磁盘容量: %2 G;剩余磁盘容量 %3 G").
            arg(m_allDisk).arg(m_useDisk).arg(m_freeDisk);
#if DEBUG_FUNC
    qDebug()<<"磁盘使用情况:"<<m_diskDescribe;
#endif
    return m_diskDescribe;
}

float Info::GetDiskUsedPercent()
{
    GetDiskInfo();
    return m_useDisk / m_allDisk;
}


bool Info::ISIPLive()
{
    //能接通百度IP说明可以通外网
    QTcpSocket tcpClient;
    tcpClient.abort();
    tcpClient.connectToHost("202.108.22.5", 80);
    //100毫秒没有连接上则判断不在线
    return tcpClient.waitForConnected(100);
}

#define LEN_DIR_FILE_SAVE_MIN           24      //保存的文件的最小长度，小于本长度说明此文件为无效文件
#define LEN_DIR_FILE_SAVE_YEAR          6       //sizeof("201801")  会计算结果为7.....
#define LEN_DIR_FILE_SAVE_MONTH         2       //sizeof("01")
#define LEN_DIR_FILE_SAVE_DAY           2       //sizeof("01")
#define SIZE_FILE_NAME_TO_STRINGLIST    6       //数据文件中有5个‘-’，所以其剪裁之后会分成6部分

QString Info::analysis_dir_name_from_file(const QString &file_name)
{
    //文件名不能为空、、、、如:   202101-29-12-11-18-305.bin
    if(file_name.isEmpty()) {return NULL;}

    //文件名长度限制
    if(file_name.size() < LEN_DIR_FILE_SAVE_MIN) {return NULL;}
    QStringList t_stringList_file_split = file_name.split('-');
    if(t_stringList_file_split.isEmpty()) {return NULL;}
    if(t_stringList_file_split.size() != SIZE_FILE_NAME_TO_STRINGLIST) {return NULL;}

    QStringList t_2222 = file_name.split('.');
    if(t_2222.size() != 2) {return NULL;}
    QDateTime m_cccc = QDateTime::fromString(t_2222.at(0),"yyyyMM-dd-HH-mm-ss-zzz");
    return m_cccc.toString("yyyy-MM-dd HH-mm-ss");
}


bool Info::is_file_name_up_to_standard(const QString &file_name)
{
    //文件名不能为空
    if(file_name.isEmpty()) {return false;}
    //文件名长度限制
    if(file_name.size() < LEN_DIR_FILE_SAVE_MIN) {return false;}

    QStringList t_stringList_file_split = file_name.split('-');
    if(t_stringList_file_split.isEmpty()) {return false;}

    if(t_stringList_file_split.size() != SIZE_FILE_NAME_TO_STRINGLIST)
    {
        return false;
    }
    return true;
}
