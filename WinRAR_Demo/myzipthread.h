#ifndef MYZIPTHREAD_H
#define MYZIPTHREAD_H

#include <QThread>
#include <QDebug>

class MyZipThread : public QThread
{
    Q_OBJECT
public:
    explicit MyZipThread(QObject *parent = nullptr);
    bool IsRunning() const;

signals:
    /***
     * 压缩工作结束、true成功、false失败
     */
    void sig_CompressedOK(bool isOK);

public slots:
    /***
     * @param file 指定的压缩文件的路径
     * @param item 要压缩的条目，可以是文件或者是一个文件夹
     */
    void slot_SetCompressedInfo(const QString&file22,const QString&item22);

    void run() override;

private:
    /***
    * @param compressed_file 指定的压缩文件的路径
    * @param item 要压缩的条目，可以是文件或者是一个文件夹
    * */
    QString m_compressed_file;
    QString m_item;
    bool isRun;
};

#endif // MYZIPTHREAD_H
