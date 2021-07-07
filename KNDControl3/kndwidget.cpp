#include "kndwidget.h"
#include "ui_kndwidget.h"

KNDWidget::KNDWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::KNDWidget)
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    ui->setupUi(this);
    CreateKNDControlThread22ConnectSignalSlot();
    m_listBJControlInfo.clear();

    m_listLabelBJControlInfo.clear();
    m_listLabelBJControlInfo << ui->label_1 << ui->label_2 << ui->label_3
                            << ui->label_4 << ui->label_5 << ui->label_6
                            << ui->label_7 << ui->label_8 << ui->label_9
                            << ui->label_10 ;
}

KNDWidget::~KNDWidget()
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    m_KND_Control_Thread->quit();
    m_KND_Control_Thread->wait();
    delete ui;
}

void KNDWidget::on_btn_open_clicked()
{
    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    double num = ui->lineEdit_DOn_channal->text().toDouble();
    emit sig_add_warning_len(num);
}

void KNDWidget::slot_tcpISConnectState(bool t_isConnect)
{
    if(t_isConnect){
        ui->label_tcpConnectState->setStyleSheet("background-color: rgb(0, 255, 0);");
    }else{
        ui->label_tcpConnectState->setStyleSheet("background-color: rgb(255, 255, 255);");
    }
}

void KNDWidget::slot_tcp22BBJControlInfo(const QString &text)
{
    static int count22 = 0;
    ui->textBrowser->append(text);
    count22 ++;
    if(count22 >= 99){
        ui->textBrowser->clear();
        count22 = 0;
    }
}

void KNDWidget::slot_AddBBJControlInfoList22Show(int relay_ID, bool isOpenOrClose)
{
    ControlInfo t_ControlInfo = ControlInfo(relay_ID,isOpenOrClose);
    if(m_listBJControlInfo.size() >= m_listLabelBJControlInfo.size()){
        m_listBJControlInfo.removeFirst();
    }

    for(auto &each_done:m_listBJControlInfo){
        if(each_done.s_relay_ID == t_ControlInfo.s_relay_ID){
            each_done.s_isOpenOrClose = t_ControlInfo.s_isOpenOrClose;
            BBJControlInfoList22Show();
            return;
        }
    }
    BBJControlInfoList22Show();
    m_listBJControlInfo.append(t_ControlInfo);
    return;
}

void KNDWidget::slot_tcpISConnectInfo(const QString &t_ip)
{
    ui->label_tcpConnectState->setStyleSheet("background-color: rgb(255, 255, 255);");
    ui->label_tcpConnectState->setText(t_ip);
    return;
}

void KNDWidget::CreateKNDControlThread22ConnectSignalSlot()
{

    qDebug() << Q_FUNC_INFO << "--->" << QThread::currentThreadId();
    m_KND_Control_Thread = new QThread(this);
    m_KND_Control_Do = new Class_KND_Control();
    m_KND_Control_Do->moveToThread(m_KND_Control_Thread);

    connect(m_KND_Control_Thread, SIGNAL(finished()),
            m_KND_Control_Do, SLOT(deleteLater()));

    connect(this, SIGNAL(sig_add_warning_len(double)),
            m_KND_Control_Do, SLOT(slot_messageParse222BBJControl(double)));

    connect(m_KND_Control_Do,SIGNAL(sig_tcpISConnectInfo(QString)),
            this,SLOT(slot_tcpISConnectInfo(QString)));

    connect(m_KND_Control_Do,SIGNAL(sig_tcpISConnectState(bool)),
            this,SLOT(slot_tcpISConnectState(bool)));

    connect(m_KND_Control_Do,SIGNAL(sig_tcp22BBJControlInfo(QString)),
            this,SLOT(slot_tcp22BBJControlInfo(QString)));

    connect(m_KND_Control_Do,SIGNAL(sig_AddBBJControlInfoList22Show(int,bool)),
            this,SLOT(slot_AddBBJControlInfoList22Show(int,bool)));

    m_KND_Control_Thread->start();
}

void KNDWidget::BBJControlInfoList22Show()
{
    int t_count = 0;
    for(QLabel * each_label:m_listLabelBJControlInfo){
        if(m_listBJControlInfo.size() <= t_count){
            break;
        }

        each_label->setText(QString("控制编码:%1").arg(
                                m_listBJControlInfo.at(t_count).s_relay_ID));

        if(m_listBJControlInfo.at(t_count).s_isOpenOrClose){
            each_label->setStyleSheet("background-color: rgb(0, 255, 0);");
        }else{
            each_label->setStyleSheet("background-color: rgb(255, 255, 255);");
        }

        t_count ++;
    }
    return;
}
