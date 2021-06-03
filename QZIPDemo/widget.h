#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QThread>
#include "zipwork.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_btn_select_file_clicked();

    void on_btn_Compressed_clicked();
    /***
     * 压缩工作结束、true成功、false失败
     */
    void slot_CompressedOK(bool isOK);
signals:
    /***
     * @param file 指定的压缩文件的路径
     * @param item 要压缩的条目，可以是文件或者是一个文件夹
     */
    void sig_SetCompressedInfo(const QString&file,const QString&item);
private:
    Ui::Widget *ui;
    ZIPWork *m_Compressed;
    QThread *m_CompressedThread;

};
#endif // WIDGET_H
