#ifndef ZIPWORK_H
#define ZIPWORK_H

#include <QObject>

class ZIPWork : public QObject
{
    Q_OBJECT
public:
    explicit ZIPWork(QObject *parent = nullptr);
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
};

#endif // ZIPWORK_H
