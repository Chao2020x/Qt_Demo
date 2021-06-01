#pragma execution_character_set("utf-8")

#include "frmemailtool.h"
#include "ui_frmemailtool.h"
#include "qfiledialog.h"
#include "qmessagebox.h"


frmEmailTool::frmEmailTool(QWidget *parent) : QWidget(parent), ui(new Ui::frmEmailTool)
{
    ui->setupUi(this);
    this->initForm();
}

frmEmailTool::~frmEmailTool()
{
    delete ui;
}

void frmEmailTool::initForm()
{    
    ui->cboxServer->setCurrentIndex(1);
    connect(SendEmailThread::Instance(), SIGNAL(receiveEmailResult(EmailSendResult *)),
            this, SLOT(receiveEmailResult(EmailSendResult *)));
    SendEmailThread::Instance()->start();
}

void frmEmailTool::on_btnSend_clicked()
{
    if (!check()) {
        return;
    }

    ui->label_send_info_state->setText("邮件发送中...");
    ui->label_send_info_state->setStyleSheet(SetLabelColor::Color_White);

    SendEmailThread::Instance()->setEmailTitle(ui->txtTitle->text());
    SendEmailThread::Instance()->setSendEmailAddr(ui->txtSenderAddr->text());
    SendEmailThread::Instance()->setSendEmailPwd(ui->txtSenderPwd->text());
    SendEmailThread::Instance()->setReceiveEmailAddr(ui->txtReceiverAddr->text());

    //设置好上述配置后,以后只要调用Append方法即可发送邮件
    SendEmailThread::Instance()->append(ui->txtContent->toHtml(), ui->txtFileName->text());
}

void frmEmailTool::on_btnSelect_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);

    if (dialog.exec()) {
        ui->txtFileName->clear();
        QStringList files = dialog.selectedFiles();
        ui->txtFileName->setText(files.join(";"));
    }
}

bool frmEmailTool::check()
{
    if (ui->txtSenderAddr->text().trimmed().isEmpty()) {
        ui->label_send_info_state->setText("用户名不能为空!");
        ui->label_send_info_state->setStyleSheet(SetLabelColor::Color_Red);
        ui->txtSenderAddr->setFocus();
        return false;
    }else{
        if(! ui->txtSenderAddr->text().trimmed().contains("@")){
            ui->label_send_info_state->setText("用户名输入有误");
            ui->label_send_info_state->setStyleSheet(SetLabelColor::Color_Red);
            ui->txtSenderAddr->setFocus();
            return false;
        }
    }

    if (ui->txtSenderPwd->text().trimmed().isEmpty()) {
        ui->label_send_info_state->setText("用户密码不能为空!");
        ui->label_send_info_state->setStyleSheet(SetLabelColor::Color_Red);
        ui->txtSenderPwd->setFocus();
        return false;
    }

    if (ui->txtReceiverAddr->text().trimmed().isEmpty()) {
        ui->label_send_info_state->setText("收件人不能为空!");
        ui->label_send_info_state->setStyleSheet(SetLabelColor::Color_Red);
        ui->txtReceiverAddr->setFocus();
        return false;
    }else{
        if(! ui->txtReceiverAddr->text().trimmed().contains("@")){
            ui->label_send_info_state->setText("收件人输入有误");
            ui->label_send_info_state->setStyleSheet(SetLabelColor::Color_Red);
            ui->txtReceiverAddr->setFocus();
            return false;
        }
    }

    if (ui->txtTitle->text().isEmpty()) {
        ui->label_send_info_state->setText("邮件标题不能为空!");
        ui->label_send_info_state->setStyleSheet(SetLabelColor::Color_Red);
        ui->txtTitle->setFocus();
        return false;
    }

    return true;
}

void frmEmailTool::on_cboxServer_currentIndexChanged(int index)
{
    if (index == 2) {
        ui->cboxPort->setCurrentIndex(1);
        ui->ckSSL->setChecked(true);
    } else {
        ui->cboxPort->setCurrentIndex(0);
        ui->ckSSL->setChecked(false);
    }
}

void frmEmailTool::receiveEmailResult(EmailSendResult * result)
{
    if(result == nullptr) {return;}
    ui->label_send_info_state->setText(result->BriefInfo);
    if(result->IsSendSuccess){
        ui->label_send_info_state->setStyleSheet(SetLabelColor::Color_Green);
    }else{
        ui->label_send_info_state->setStyleSheet(SetLabelColor::Color_Red);
    }
    delete result;
    result = nullptr;
    return;
}
