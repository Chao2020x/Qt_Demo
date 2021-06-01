#include "mywidget.h"
#include "ui_mywidget.h"
#include <QTime>
#include <QStringList>
#include <QDebug>
#include <QDateTime>

MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWidget)
{
    qDebug()<< Q_FUNC_INFO << __LINE__;
    ui->setupUi(this);

    m_client = new QMqttClient(this);
    /*------------初始化函数------------*/
    InitConnect();//初始化连接
    InitSubscribe();//初始化订阅
    InitRelease();//初始化发布
    InitLog();//初始化LOG

    //信号槽
    connect(m_client, &QMqttClient::stateChanged, this, &MyWidget::updateConnectUiStatus);
    connect(m_client, &QMqttClient::disconnected, this, &MyWidget::on_btnDisconnect_clicked);
    connect(m_client, &QMqttClient::messageReceived, this, &MyWidget::UpdataMessagetoLOG);
    connect(m_client, &QMqttClient::pingResponseReceived,this,&MyWidget::UpdataMessagetoLOG_2 );
}

MyWidget::~MyWidget()
{
    qDebug()<< Q_FUNC_INFO << __LINE__;
    on_btnDisconnect_clicked();
    delete ui;
}

void MyWidget::on_btnConnect_clicked()
{
    qDebug()<< Q_FUNC_INFO << __LINE__;
    ConnectHostName = ui->lineEditServer->text();
    ConnectPort = ui->spinBoxPort->value();
    ConnectClientID = ui->lineEdit_ClientID->text();
    ConnectUsername = ui->lineEdit_Username->text();
    ConnectPassword = ui->lineEdit_Password->text();
    if(ui->cbx_CleanSession->isChecked()){
        ConnectCleanSession = true;
        qDebug()<<"ConnectCleanSession"<<1;
    }else {
        ConnectCleanSession = false;
        qDebug()<<"ConnectCleanSession"<<0;
    }
    if(ui->cbx_SSL->isChecked()){
        ConnectSSL = true;
        qDebug()<<"ConnectSSL"<<1;
    }else{
        ConnectSSL = false;
        qDebug()<<"ConnectSSL"<<0;
    }

    onConnect();
}

void MyWidget::on_btnDisconnect_clicked()
{
    qDebug()<< Q_FUNC_INFO << __LINE__;
    onDisconnect();
    updateSubscribeUiStatus();
}

void MyWidget::on_btnSubscribe_clicked()
{
    qDebug()<< Q_FUNC_INFO << __LINE__;
    SubscribeTopic.clear();
    SubscribeTopic = ui->lineEdit_Topic->text().split(",");
    if (SubscribeTopic.isEmpty()){
        qDebug()<<"空的";
        return;
    }
    //暂时放一个Topic
    ui->label_all_Topic->setText(ui->lineEdit_Topic->text());
    ui->lineEdit_Topic->clear();

    SubscribeQOS = ui->cbx_QOS->currentText().toInt();
    onSubscribe();
    updateSubscribeUiStatus();
    return;
}

void MyWidget::on_btnRelease_clicked()
{
    qDebug()<< Q_FUNC_INFO << __LINE__;
    ReleaseTopic = ui->lineEditReleaseTopic->text().split(",");
    ReleaseText = ui->textEdit_Payload->toPlainText();
    ReleaseQOS = ui->cbx_release_QOS->currentText().toInt();
    ReleaseRetained = ui->checkBox_Retained->isChecked();
    if(ReleaseText.isEmpty()){return;}
    onRelease();
}

void MyWidget::on_btnClean_clicked()
{
    qDebug()<< Q_FUNC_INFO << __LINE__;
    InitLog();
    return;
}

void MyWidget::on_btnClose_clicked()
{
    qDebug()<< Q_FUNC_INFO << __LINE__;
    QApplication::quit();
}

void MyWidget::InitConnect()
{
    qDebug()<< Q_FUNC_INFO << __LINE__;
    ui->lineEditServer->setText(tr("127.0.0.1"));
    ui->spinBoxPort->setValue(1883);
    QTime time;
    time= QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    int num = qrand() % 5;    //产生5以内的随机数
    QString ID = QString("MQClient")+QString::number(num);
    ui->lineEdit_ClientID->setText(ID);
    //Clean Session 默认被选中
    ui->cbx_CleanSession->setChecked(true);
    //SSL 默认关闭
    ui->cbx_SSL->setChecked(false);

    return;
}

void MyWidget::InitSubscribe()
{
    qDebug()<< Q_FUNC_INFO << __LINE__;
    ui->cbx_QOS->clear();
    QStringList qos_List;
    qos_List.clear();
    qos_List<<"0"<<"1"<<"2";
    ui->cbx_QOS->addItems(qos_List);
    return;
}

void MyWidget::InitRelease()
{
    qDebug()<< Q_FUNC_INFO << __LINE__;
    ui->cbx_release_QOS->clear();
    QStringList qos_List;
    qos_List.clear();
    qos_List<<"0"<<"1"<<"2";
    ui->cbx_release_QOS->addItems(qos_List);
    return;
}

void MyWidget::InitLog()
{
    qDebug()<< Q_FUNC_INFO << __LINE__;
    ui->textBrowser->clear();
    return;
}

void MyWidget::updateConnectUiStatus()
{
    qDebug()<< Q_FUNC_INFO << __LINE__;
    bool state = true;
    QString CC ;
    /***************Connect******************/
    switch (m_client->state()) {
    case QMqttClient::Disconnected:
        state = true; CC = "Disconnected"; break;
    case QMqttClient::Connecting:
        state = true; CC = "Connecting"; break;
    case QMqttClient::Connected:
        state = false;CC = "Connected"; break;
    default:
        state = true; CC = "Unknown"; break;
    }
    qDebug()<<"updateConnectUiStatus():"<<CC;
    ui->lineEditServer->setEnabled(state);
    ui->spinBoxPort->setEnabled(state);
    ui->btnConnect->setEnabled(state);
    ui->lineEdit_ClientID->setEnabled(state);
    ui->lineEdit_Username->setEnabled(state);
    ui->lineEdit_Password->setEnabled(state);
    ui->btnConnect->setEnabled(state);

    QString SS;
    SS.clear();
    SS = QString("(%1)\n%2 to %3:").arg(TIMEMS).arg(CC).arg(
                m_client->hostname());
    SS += QString::number(m_client->port());
    ui->textBrowser->append(SS);
    ui->textBrowser->append("------------------------");
    return;
}

void MyWidget::updateSubscribeUiStatus()
{
    qDebug()<< Q_FUNC_INFO << __LINE__;
    if(m_client->state() == QMqttClient::Connected){
        ui->cbx_QOS->setEnabled(false);
        ui->lineEdit_Topic->setEnabled(false);
        return;
    }
    ui->cbx_QOS->setEnabled(true);
    ui->lineEdit_Topic->setEnabled(true);
}

void MyWidget::UpdataMessagetoLOG(const QByteArray &message,
                                  const QMqttTopicName &topic)
{
    qDebug()<< Q_FUNC_INFO << __LINE__;
        const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(" Received Topic: ")
                    + topic.name()
                    + QLatin1String(" Message: ")
                    + message
                    + QLatin1Char('\n');
        ui->textBrowser->append(content);
        ui->textBrowser->append("------------------------");
}
void MyWidget::UpdataMessagetoLOG_2()
{
    const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(" PingResponse")
                + QLatin1Char('\n');
    ui->textBrowser->append(content);
    ui->textBrowser->append("------------------------");
}

void MyWidget::UpdataMessagetoLOG_3(QMqttMessage msg)
{
    UpdataMessagetoLOG(msg.payload(),msg.topic());
}

void MyWidget::add_error_log(QMqttClient::ClientError error)
{
//    enum ClientError {
//        // Protocol states
//        NoError                = 0,
//        InvalidProtocolVersion = 1,
//        IdRejected             = 2,
//        ServerUnavailable      = 3,
//        BadUsernameOrPassword  = 4,
//        NotAuthorized          = 5,
//        // Qt states
//        TransportInvalid       = 256,
//        ProtocolViolation,
//        UnknownError,
//        Mqtt5SpecificError
//    };
    qDebug()<<"ClientError:"<<error;
}
