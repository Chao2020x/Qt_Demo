#ifndef GLOBAL_SET_H
#define GLOBAL_SET_H

#include <QString>

class Global_Set
{
    static QString configName;
public:
    static QString AutoZipDir;
    static bool IsAutoZip;
    static bool IsAutoClear;
    static int DayInterval;
    static void ReadDBConfig();
    static void WriteDBConfig();
};

#endif // GLOBAL_SET_H
