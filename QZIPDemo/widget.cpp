#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QDebug>

/***
 *
 *
 * WinRarInterface.h/cpp 是在gitee上找的
 * https://gitee.com/pan0/WinRarInterface
 * 压缩需要另起一个线程，否则界面容易无响应
 * 调用WINRAR去压缩文件、所有电脑需要安装WINRAR
 * 测试时，压缩10G以下是没有问题的，10G以上的没试过
 *
 *
 */


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    ui->setupUi(this);
    m_CompressedThread = new QThread(this);
    m_Compressed = new ZIPWork();
    m_Compressed->moveToThread(m_CompressedThread);
    connect(m_CompressedThread,SIGNAL(finished()),
            m_Compressed,SLOT(deleteLater()));

    connect(this,&Widget::sig_SetCompressedInfo,
                m_Compressed,&ZIPWork::slot_SetCompressedInfo);

    connect(m_Compressed,&ZIPWork::sig_CompressedOK,
                this,&Widget::slot_CompressedOK);

    m_CompressedThread->start();
}

Widget::~Widget()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    m_CompressedThread->quit();
    m_CompressedThread->wait();
    delete ui;
}

void Widget::on_btn_select_file_clicked()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    QString path = QFileDialog::getExistingDirectory(this, "选择目录", "./",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!path.isEmpty()) {
        ui->lineEdit_file_dir->setText(path);
        QString file = QString("%1.zip").arg(path);
        ui->lineEdit_file->setText(file);
        qDebug()<<path <<"------"<< file;
        ui->label_cinfo->setText("已选择待压缩文件夹");
    }
}

void Widget::on_btn_Compressed_clicked()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    QString Path = ui->lineEdit_file_dir->text();
    QString File = ui->lineEdit_file->text();
    if(Path.isEmpty() || File.isEmpty()){
        ui->label_cinfo->setText("请选择待压缩的文件夹");
        return;
    }
    emit sig_SetCompressedInfo(File,Path);
    ui->btn_Compressed->setEnabled(false);
    ui->btn_select_file->setEnabled(false);
}

void Widget::slot_CompressedOK(bool isOK)
{
    QString info = isOK ? "压缩成功" : "压缩失败";
    ui->label_cinfo->setText(info);
    ui->btn_Compressed->setEnabled(true);
    ui->btn_select_file->setEnabled(true);
}
