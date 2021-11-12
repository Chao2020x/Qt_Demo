#include "global_set.h"
#include <QSettings>

QString Global_Set::AutoZipDir;
bool Global_Set::IsAutoZip = true;
bool Global_Set::IsAutoClear = true;
int Global_Set::DayInterval = 1;

QString Global_Set::configName = "GYZip_Conf.ini";//配置文件名

void Global_Set::ReadDBConfig()
{
    QSettings conf_read(Global_Set::configName,QSettings::IniFormat);
    conf_read.beginGroup("ZipConfig");
    if(!conf_read.contains("AutoZipDir")){
        conf_read.endGroup();
        Global_Set::WriteDBConfig();
        return;
    }
    Global_Set::AutoZipDir = conf_read.value("AutoZipDir").toString();
    Global_Set::IsAutoZip = conf_read.value("IsAutoZip").toBool();
    Global_Set::IsAutoClear = conf_read.value("IsAutoClear").toBool();
    Global_Set::DayInterval = conf_read.value("DayInterval").toInt();
    conf_read.endGroup();
    return;
}

void Global_Set::WriteDBConfig()
{
    QSettings conf_write(Global_Set::configName,QSettings::IniFormat);
    conf_write.beginGroup("ZipConfig");
    conf_write.setValue("AutoZipDir",Global_Set::AutoZipDir);
    conf_write.setValue("IsAutoZip",Global_Set::IsAutoZip);
     conf_write.setValue("IsAutoClear",Global_Set::IsAutoClear);
    conf_write.setValue("DayInterval",Global_Set::DayInterval);
    conf_write.endGroup();
}

