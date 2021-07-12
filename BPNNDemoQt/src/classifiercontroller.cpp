#include "inc/classifiercontroller.h"
#include "inc/classifierselector.h"
#include "ui_classifiercontroller.h"
#include <QDebug>
ClassifierController::ClassifierController(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClassifierController)
{

    ui->setupUi(this);
    connect(ui->btnAddClass,SIGNAL(clicked()),this,SLOT(onBtnAddClassClicked()));
    connect(ui->btnRemoveClass,SIGNAL(clicked()),this,SLOT(onBtnRemoveClassClicked()));
    connect(ui->listWidget,SIGNAL(colorChanged(int,QRgb)),this,SLOT(onColorChanged(int,QRgb)));
    for(int i=0;i<mCnt;i++)
    {
        ui->listWidget->addItemByNum(i);
    }

}

ClassifierController::~ClassifierController()
{
    delete ui;
}

int ClassifierController::getClassifierNum()
{
    return ui->listWidget->currentRow();
}

QColor ClassifierController::getClassifierColor(int type)
{
    return ui->listWidget->getItem(type)->color();
}

int ClassifierController::getClassifierCnt()
{
    return ui->listWidget->count();
}

void ClassifierController::setAddDelButtonEnabled(bool b)
{
    ui->btnAddClass->setEnabled(b);
    ui->btnRemoveClass->setEnabled(b);
}

void ClassifierController::setCurrentClassNum(int i)
{
    ui->listWidget->setCurrentRow(i);
}

void ClassifierController::setClassifierNum(int i)
{
    int c=mCnt;
    if (i<c){
        while(i++<c){
            onBtnRemoveClassClicked();
        }
    }else{
        while(i-->c){
            onBtnAddClassClicked();
        }
    }
}

void ClassifierController::onBtnAddClassClicked()
{
    if(mCnt<6)
    {
        ui->listWidget->addItemByNum(mCnt);
        mCnt++;

    }
}

void ClassifierController::onBtnRemoveClassClicked()
{
    if(mCnt>2)
    {
        ui->listWidget->removeItem();
        mCnt--;
        emit(classCntChanged(mCnt));

    }
}

void ClassifierController::onColorChanged(int i , QRgb color)
{
    emit colorChanged(i,color);
}
