#include "inc/mainwindow.h"
#include "ui_mainwindow.h"
#include "inc/hlncntselector.h"
#include "inc/classifierselector.h"
#include "inc/classifiercontroller.h"
#include "inc/surfacenode.h"
#include <QDebug>
#include <QDoubleSpinBox>
#include <QButtonGroup>
#include <QScreen>
#include <QRegExpValidator>
#include <QFileDialog>
#include <QDataStream>
MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->sbHLCnt,SIGNAL(valueChanged(int)),this,SLOT(hiddenLayerCountChanged(int)));
    setHiddenLayerCnt(1);
    QButtonGroup* group=new QButtonGroup(this);
    group->addButton(ui->rbTrainTime);
    group->addButton(ui->rbTrainErr);
    connect(group,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(trainSettingBtnClicked(QAbstractButton*)));
    ui->leTrainTime->setEnabled(false);
    ui->rbTrainErr->setChecked(true);
    connect(ui->btnTrain,SIGNAL(clicked()),this,SLOT(onStart()));
    connect(ui->btnTrainByCond,SIGNAL(clicked()),this,SLOT(onStartByCond()));
    connect(ui->classifierController,SIGNAL(classCntChanged(int)),this,SLOT(onClassCntChanged(int)));
    connect(ui->btnPause,SIGNAL(clicked()),this,SLOT(onPause()));
    connect(ui->btnStop,SIGNAL(clicked()),this,SLOT(onStop()));
    connect(ui->btnResetSurface,SIGNAL(clicked()),this,SLOT(onResetSurface()));
    connect(ui->btnResetTrainCnt,SIGNAL(clicked()),this,SLOT(onResetTrainCnt()));
    connect(ui->btnClearSamples,SIGNAL(clicked()),this,SLOT(onClearSamples()));
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(onActionSaveTrigged()));
    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(onActionOpenTrigged()));
    QRegExp regexp("\\d{1,8}");
    QRegExpValidator *v=new QRegExpValidator(regexp);
    ui->leTrainTime->setValidator(v);
    QRegExp regexp2("0\\.\\d{0,6}");
    v=new QRegExpValidator(regexp2);
    ui->leTrainErr->setValidator(v);
    mDefBtnPalette=ui->btnTrain->palette();
    mHighlightBtnPalette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(Qt::darkGreen));
    ui->sbLearnRate->setDecimals(3);
    ui->sbLearnRate->setValue(0.01);
    ui->sbLearnRate->setMinimum(0.001);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::getClassifierNum()
{
    return ui->classifierController->getClassifierNum();
}

double MainWindow::getLearnRate()
{
    return ui->sbLearnRate->value();
}

QColor MainWindow::getClassifierColor(int type)
{
    return ui->classifierController->getClassifierColor(type);
}

int MainWindow::getHiddenLayerCnt()
{
    return ui->sbHLCnt->value();
}

int MainWindow::getHiddenNodeCntByNum(int num)
{
    HLNCntSelector* selector=static_cast<HLNCntSelector*>(ui->liHiddenLayer->itemWidget(ui->liHiddenLayer->item(num)));
    return selector->count();
}

int MainWindow::getClassifierCnt()
{
    return ui->classifierController->getClassifierCnt();
}

bool MainWindow::trainByTime()
{
    if(ui->rbTrainTime->isChecked())
        return true;
    else return false;
}

int MainWindow::getTrainTime()
{
    if(ui->leTrainTime->text().isEmpty())
        ui->leTrainTime->setText("10000");
    return ui->leTrainTime->text().toInt();
}

double MainWindow::getTrainErr()
{
    if(ui->leTrainErr->text().isEmpty())
        ui->leTrainErr->setText("0.0001");
    return ui->leTrainErr->text().toDouble();
}

void MainWindow::setCurrentTime(int time)
{
    ui->lbCurrentTime->setText(QString::number(time));
}

void MainWindow::setCurrentErr(double err)
{
    ui->lbCurrentErr->setText(QString::number(err,'f'));
}

ClassifierController *MainWindow::getController()
{
    return ui->classifierController;
}

QDoubleSpinBox* MainWindow::getSbLearnRate()
{
    return ui->sbLearnRate;
}

void MainWindow::afterStopTrain()
{
    ui->btnPause->setEnabled(false);
    ui->btnStop->setEnabled(false);
    ui->btnTrainByCond->setEnabled(true);
    ui->btnTrain->setEnabled(true);
    ui->btnResetSurface->setEnabled(true);
    ui->btnResetTrainCnt->setEnabled(false);
    ui->classifierController->setAddDelButtonEnabled(true);
    ui->sbHLCnt->setEnabled(true);
    ui->frameTrainCfg->setEnabled(true);
    ui->liHiddenLayer->setEnabled(true);
    ui->btnTrain->setPalette(mDefBtnPalette);
    ui->btnTrainByCond->setPalette(mDefBtnPalette);
    ui->btnPause->setText("暂停");
}

void MainWindow::onClearSamples()
{
    emit sigClearSamples();
}

void MainWindow::onActionSaveTrigged()
{

    QString filename=QFileDialog::getSaveFileName(this,"保存文件",QString(),"Data (*.dat)");
    if(!filename.isEmpty()){
        if(!filename.endsWith(".dat")){
            filename+=".dat";
        }
        QFile file(filename);
        if (file.open(QIODevice::WriteOnly))
        {
            QDataStream in(&file);
            int cnt = ui->classifierController->getClassifierCnt();
            in<<cnt;
            cnt=ui->surface->nodeList().count();
            in<<cnt;
            for(int i=0;i<cnt;i++){
                SurfaceNode node=ui->surface->nodeList()[i];
                in<<node.type();
                in<<node.pos().x();
                in<<node.pos().y();
            }

        }

    }

}

void MainWindow::onActionOpenTrigged()
{
    QString filename=QFileDialog::getOpenFileName(this,"打开文件",QString(),"Data (*.dat)");
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly))
    {
        QDataStream out(&file);
        int cnt;
        out>>cnt;
        ui->classifierController->setClassifierNum(cnt);
        out>>cnt;
        QList<SurfaceNode> li;
        for (int i=0;i<cnt;i++)
        {
            int type;
            qreal x,y;
            out>>type>>x>>y;
            SurfaceNode node(type,QPointF(x,y));
            li<<node;
        }
        ui->surface->setNodeList(li);
    }


    ui->surface->update();

}

void MainWindow::onStart()
{
    preStartTrain();
    ui->btnTrain->setPalette(mHighlightBtnPalette);
    ui->surface->startTrain(1);
}

void MainWindow::onStartByCond()
{
    preStartTrain();
    ui->btnTrainByCond->setPalette(mHighlightBtnPalette);
    ui->surface->startTrain(0);
}

void MainWindow::onClassCntChanged(int cnt)
{
    ui->surface->deleteNodeByClassCnt(cnt);
}

void MainWindow::onPause()
{
    if(ui->btnPause->text()==QString("暂停"))
    {
        ui->btnPause->setText("继续");
        emit sigPause(0);
    }else{

        ui->btnPause->setText("暂停");
        emit sigPause(1);
    }
}

void MainWindow::onStop()
{
    emit sigStop();
    postEndTrain();

}

void MainWindow::onResetSurface()
{
    emit sigResetSurface();
}

void MainWindow::onResetTrainCnt()
{
    emit sigResetTrainCnt();
    ui->lbCurrentTime->setText("0");
}

void MainWindow::hiddenLayerCountChanged(int i)
{
    setHiddenLayerCnt(i);
}

void MainWindow::trainSettingBtnClicked(QAbstractButton * btn)
{
    if(btn==ui->rbTrainTime){
        ui->leTrainErr->setEnabled(false);
        ui->leTrainTime->setEnabled(true);
    }
    else if(btn==ui->rbTrainErr)
    {
        ui->leTrainErr->setEnabled(true);
        ui->leTrainTime->setEnabled(false);
    }
}


void MainWindow::setHiddenLayerCnt(int cnt)
{
    int num=ui->liHiddenLayer->count();
    if(cnt<num)
    {
        for(int i=0;i<num-cnt;i++)
        {
            ui->liHiddenLayer->takeItem(ui->liHiddenLayer->count()-1);
        }
    }
    for(int i=num;i<cnt;i++)
    {
        QListWidgetItem *item=new QListWidgetItem(ui->liHiddenLayer);
        ui->liHiddenLayer->addItem(item);
        ui->liHiddenLayer->setItemWidget(item,new HLNCntSelector(i));
        item->setSizeHint(QSize(80,50));
    }
}

void MainWindow::preStartTrain()
{
    ui->btnPause->setEnabled(true);
    ui->btnStop->setEnabled(true);
    ui->btnTrainByCond->setEnabled(false);
    ui->btnTrain->setEnabled(false);
    ui->classifierController->setAddDelButtonEnabled(false);
    ui->sbHLCnt->setEnabled(false);
    ui->liHiddenLayer->setEnabled(false);
    ui->btnResetSurface->setEnabled(false);
    ui->frameTrainCfg->setEnabled(false);
    ui->btnResetTrainCnt->setEnabled(true);
    ui->classifierController->setCurrentClassNum(-1);
}

void MainWindow::postEndTrain()
{
    ui->btnPause->setEnabled(false);
    ui->btnStop->setEnabled(false);
    ui->btnTrainByCond->setEnabled(true);
    ui->btnTrain->setEnabled(true);
    ui->classifierController->setAddDelButtonEnabled(true);
    ui->sbHLCnt->setEnabled(true);
    ui->liHiddenLayer->setEnabled(true);
    ui->btnResetSurface->setEnabled(true);
    ui->frameTrainCfg->setEnabled(true);
    ui->btnResetTrainCnt->setEnabled(false);
    ui->classifierController->setCurrentClassNum(-1);
}
