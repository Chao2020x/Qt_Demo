#include "myzipthread.h"

MyZipThread::MyZipThread(QObject *parent) : QThread(parent)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    this->isRun = false;
}

bool MyZipThread::IsRunning() const
{
    return this->isRun;
}

#include <windows.h>
#include <stdio.h>
#include <locale.h>
#include "WinRarInterface.h"

void MyZipThread::slot_SetCompressedInfo(const QString &file22, const QString &item22)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();

    if(this->isRun){return;}

    if(file22.isEmpty() || item22.isEmpty())
    {
        return;
    }

    m_compressed_file = file22;
    m_item = item22;
    this->start();
}

void MyZipThread::run()
{
    this->isRun = true;

    if(m_compressed_file.isEmpty() || m_item.isEmpty())
    {
        this->isRun = false;
        return;
    }

    const wchar_t *zip = reinterpret_cast<const wchar_t *>(
                m_compressed_file.utf16());

    const wchar_t *item = reinterpret_cast<const wchar_t *>(
                m_item.utf16());

    using namespace Pan;
    WinRarInterface *wi = WinRarInterface::autoDetect();
    if (wi == nullptr)
    {
        emit sig_CompressedOK(false);
        this->isRun = false;
        return;
    }
    int code = wi->winrar_add(zip, item);
    bool isOK = (code >= 0) ? true : false;
    emit sig_CompressedOK(isOK);
    delete wi, wi = nullptr;
    this->isRun = false;
    m_compressed_file.clear();
    m_item.clear();
}
