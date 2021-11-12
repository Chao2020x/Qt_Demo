#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QDate>
#include "global_set.h"
#include <QDir>

//Windows进程相关
#include <windows.h>
#include <TlHelp32.h>

namespace SetLabelBgColor {
const QString Color_Green = "QLabel{background-color:rgb(0,255,0);}"; //绿色
const QString Color_Red = "QLabel{background-color:rgb(255,0,0);}"; //红色
const QString Color_White = "QLabel{background-color:rgb(255,255,255);}"; //红色
const QString Color_Transparent = "QLabel{background:transparent;}";//透明
const QString Color_Grey = "QLabel{background:grey;}";//透明
}

bool FindProcess22Close(const QString &str_name)
{
    qDebug() << Q_FUNC_INFO <<__LINE__;
    if(str_name.isEmpty() || (str_name.size() >= 256))
    {
        return false;
    }
    qDebug()<<Q_FUNC_INFO<<"查询软件:"<<str_name;

    int i = 0;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        i += 0;
    }
    bool bMore = ::Process32First(hProcessSnap, &pe32);
    while (bMore)
    {
        LPTSTR pf = (LPTSTR)(LPCTSTR)pe32.szExeFile;
        char *pFileName = (char *)malloc(2 * wcslen(pf) + 1);
        wcstombs(pFileName, pf, 2 * wcslen(pf) + 1);
//        printf (" 进程名称：%s \n", pe32.szExeFile);
        QString tt_runningName(pFileName);
        if(tt_runningName.contains(str_name,Qt::CaseInsensitive)){
            qDebug()<<Q_FUNC_INFO<<"查询到软件:"<<str_name<<"关闭";
            i += 1;
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
            if(hProcess != NULL){
                TerminateProcess(hProcess, 0);
            }
            return true;
        }
        bMore = ::Process32Next(hProcessSnap, &pe32);
    }
    if (i > 0)
    {
        //大于1，排除自身
        return true;
    }
    return false;
}
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_CurrentCompresseDir.clear();

    m_CompressedThread = new MyZipThread(this);
    connect(m_CompressedThread,&MyZipThread::sig_CompressedOK,
                this,&Widget::slot_CompressedOK);
    ui->lcdNumber->start(1000);

    connect(ui->bottom_widget,&BottomWidget::sig_Over_A_Day,this,&Widget::slot_Over_A_Day);
    ui->bottom_widget->start(1000);
    this->setWindowTitle("数据压缩处理软件");
    initConf();

    m_SystemTrayIcon = new QSystemTrayIcon(this);

     connect(m_SystemTrayIcon , SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
             this, SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));
     FindProcess22Close("WinRAR");
}

Widget::~Widget()
{
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
        ui->label_cinfo->setStyleSheet(SetLabelBgColor::Color_White);
        slot_textBrowserShowAlarmInfo(QString("已选择待压缩文件夹:%1").arg(file));
    }
}

void Widget::on_btn_Compressed_clicked()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    if(m_CompressedThread->isRunning()) {return;}

    QString Path = ui->lineEdit_file_dir->text();
    QString File = ui->lineEdit_file->text();
    if(Path.isEmpty() || File.isEmpty()){
        ui->label_cinfo->setText("请选择待压缩的文件夹");
        ui->label_cinfo->setStyleSheet(SetLabelBgColor::Color_White);
        slot_textBrowserShowAlarmInfo("请选择待压缩的文件夹");
        return;
    }
    slot_textBrowserShowAlarmInfo(QString("开始生成压缩文件:%1").arg(File));
    m_CompressedThread->slot_SetCompressedInfo(File,Path);
    ui->btn_Compressed->setEnabled(false);
    ui->btn_select_file->setEnabled(false);
}

#include <QtConcurrent>

bool DeleteFileOrFolder(const QString &strPath)//要删除的文件夹或文件的路径
{
    if (strPath.isEmpty() || !QDir().exists(strPath))//是否传入了空的路径||路径是否存在
        return false;

    QFileInfo FileInfo(strPath);

    if (FileInfo.isFile())//如果是文件
        QFile::remove(strPath);
    else if (FileInfo.isDir())//如果是文件夹
    {
        QDir qDir(strPath);
        qDir.removeRecursively();
    }
    return true;
}

void Widget::slot_CompressedOK(bool isOK)
{
    QString info = isOK ? "压缩成功" : "压缩失败";
    ui->label_cinfo->setText(info);

    if(isOK){
        ui->label_cinfo->setStyleSheet(SetLabelBgColor::Color_Green);
    }else{
        ui->label_cinfo->setStyleSheet(SetLabelBgColor::Color_Red);
    }

    ui->btn_Compressed->setEnabled(true);
    ui->btn_select_file->setEnabled(true);


    if((!m_CurrentCompresseDir.isEmpty()) && Global_Set::IsAutoClear){
        QString gg_dir = m_CurrentCompresseDir;
        m_CurrentCompresseDir.clear();
        QtConcurrent::run(DeleteFileOrFolder,gg_dir);
        slot_textBrowserShowAlarmInfo(QString("开始清理文件夹:%1").arg(gg_dir));
    }
}

auto CompressedFileName = [](const QString &dirName) -> QString{
    for(int i = 0; i < 1000; i ++){
        QString ggg = dirName;
        if(i > 0){
            ggg.append(QString::number(i));
        }
        ggg.append(".rar");

        if(QFileInfo(ggg).exists()) {continue;}
        return ggg;
    }
    return "";
};

void Widget::slot_Over_A_Day()
{
    if(!ui->checkBox->isChecked()) {return;}
    int day_interval = ui->spinBox->value();
    QDate tt_CompressedDate = QDate::currentDate().addDays(0-day_interval);
    if(!tt_CompressedDate.isValid()){return;}

    FindProcess22Close("WinRAR");
    QString tt_dir = Global_Set::AutoZipDir;
    tt_dir.append("/");
    tt_dir.append(tt_CompressedDate.toString("yyyy-MM-dd"));

    QDir dir(tt_dir);
    if(!dir.exists()) {return;}

    m_CurrentCompresseDir = tt_dir;

    //文件存在
    QString file111 = CompressedFileName(m_CurrentCompresseDir);
    QString ggg_info;
    if(!m_CompressedThread->isRunning()) {
        ggg_info = QString("开始生成压缩文件:%1").arg(file111);
        m_CompressedThread->slot_SetCompressedInfo(file111,m_CurrentCompresseDir);
    }else{
        ggg_info = QString("开始生成压缩文件:%1").arg(file111);
    }
    slot_textBrowserShowAlarmInfo(ggg_info);
    return;
}

#include <QDateTime>
void Widget::slot_textBrowserShowAlarmInfo(const QString &t_alarmInfo)
{
    static int tt_count = 0;
    if(t_alarmInfo.isEmpty()) {return;}

    if((++ tt_count) >= 40){
        ui->textBrowser->clear();
        tt_count = 0;
    }

    QString ggg_data = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss] ");
    ui->textBrowser->insertPlainText(ggg_data);
    ui->textBrowser->insertPlainText("\n");
    ui->textBrowser->insertPlainText(t_alarmInfo);
    ui->textBrowser->insertPlainText("\n");
    ui->textBrowser->moveCursor(QTextCursor::End);
}

//自动压缩
void Widget::on_btn_auto_yasuo_clicked()
{
    qDebug()<<Q_FUNC_INFO<<"--->"<<QThread::currentThreadId();
    QString path = QFileDialog::getExistingDirectory(this, "选择目录", "./",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!path.isEmpty()) {
        ui->lineEdit_auto_yasuo->setText(path.trimmed());
    }
}


void Widget::slot_SaveConf()
{
    Global_Set::AutoZipDir = ui->lineEdit_auto_yasuo->text().trimmed();
    Global_Set::IsAutoZip = ui->checkBox->isChecked();
    Global_Set::IsAutoClear = ui->checkBox_auto_clear->isChecked();
    Global_Set::DayInterval = ui->spinBox->value();
    Global_Set::WriteDBConfig();
}

void Widget::iconIsActived(QSystemTrayIcon::ActivationReason)
{
    m_SystemTrayIcon->hide();
    this->showNormal();
}

#include <QCloseEvent>
void Widget::closeEvent(QCloseEvent *event)
{
//    this->close();
//    return;
    event->ignore();
    this->hide();
    QIcon icon = QIcon(":/clean.png");
    //将icon设到QSystemTrayIcon对象中
    m_SystemTrayIcon->setIcon(icon);
    //当鼠标移动到托盘上的图标时，会显示此处设置的内容
    m_SystemTrayIcon->setToolTip(QObject::trUtf8("数据压缩处理软件"));
    //在系统托盘显示此对象
    m_SystemTrayIcon->show();
}

void Widget::initConf()
{
    Global_Set::ReadDBConfig();
    ui->lineEdit_auto_yasuo->setText(Global_Set::AutoZipDir);
    ui->checkBox->setChecked(Global_Set::IsAutoZip);
    ui->checkBox_auto_clear->setChecked(Global_Set::IsAutoClear);
    ui->spinBox->setValue(Global_Set::DayInterval);

    connect(ui->lineEdit_auto_yasuo,&QLineEdit::textChanged,this,&Widget::slot_SaveConf);
    connect(ui->checkBox_auto_clear,&QCheckBox::stateChanged,this,&Widget::slot_SaveConf);
    connect(ui->checkBox,&QCheckBox::stateChanged,this,&Widget::slot_SaveConf);
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(slot_SaveConf()));
}
