#include "zipwork.h"
#include <windows.h>
#include <stdio.h>
#include <locale.h>
#include <QThread>
#include "WinRarInterface.h"
#include <QDebug>
using namespace Pan;

ZIPWork::ZIPWork(QObject *parent) : QObject(parent)
{
qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
}

void ZIPWork::slot_SetCompressedInfo(const QString &file22, const QString &item22)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    if(file22.isEmpty() || item22.isEmpty())
    {
        return;
    }

    const wchar_t *zip = reinterpret_cast<const wchar_t *>(
                file22.utf16());

    const wchar_t *item = reinterpret_cast<const wchar_t *>(
                item22.utf16());

//    const wchar_t *zip = L"F:\\test.zip";
//    const wchar_t *item = L"G:\\";
    WinRarInterface *wi = WinRarInterface::autoDetect();
    if (wi == nullptr)
    {
//        ::wprintf(L"WinRarInterface::autoDetect nullptr!\n");
        emit sig_CompressedOK(false);
        return;
    }

    int code = wi->winrar_add(zip, item);

    bool isOK = (code >= 0) ? true : false;
    emit sig_CompressedOK(isOK);

//    ::wprintf(L"code: %d\n", code);

    delete wi, wi = nullptr;
}


