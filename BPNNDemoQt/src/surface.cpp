#include "ui_surface.h"
#include "inc/surface.h"
#include "inc/mainwindow.h"
#include "inc/surfacenode.h"
#include "inc/bpnetworkqt.h"
#include "inc/trainthread.h"
#include "inc/paintthread.h"
#include "inc/classifiercontroller.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QtMath>
#include <QMessageBox>
#include <QDoubleSpinBox>
#define NODE_WIDTH 20
Surface::Surface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Surface)
{

    ui->setupUi(this);
    mNodeList.clear();
    mNetwork=nullptr;
    mTrainThread=nullptr;
    mPaintThread=nullptr;
    mEraseFlag=false;
    mShowMap=false;
    mMutex=nullptr;
    mType=-1;
    mainWindow=static_cast<MainWindow*>(parent->parentWidget());
    setMouseTracking(true);
    connect(mainWindow,SIGNAL(sigResetSurface()),this,SLOT(onResetSurface()));
    connect(mainWindow,SIGNAL(sigClearSamples()),this,SLOT(onClearSamples()));



}

void Surface::deleteNodeByClassCnt(int cnt)
{
    int j=mNodeList.count();
    for(int i=0;i<j;)
    {
        SurfaceNode s=mNodeList.at(i);
        if(s.type()>=cnt)
        {
            mNodeList.removeAt(i);
            j--;
            continue;
        }
        i++;
    }
    update();
}

void Surface::startTrain(int type)
{

    if (mMutex != nullptr){
        delete mMutex;
    }

    mMutex = new QMutex();
    if(mTrainThread!=nullptr)
    {
        if(mTrainThread->isRunning())
        {
            mTrainThread->setIsAlive(false);
        }
        mTrainThread->wait();
        delete mTrainThread;
    }
    if(mPaintThread!=nullptr)
    {
        if(mPaintThread->isRunning())
        {
            mPaintThread->setIsAlive(false);
        }
        mPaintThread->wait();
        delete mPaintThread;
    }
    if(mNetwork!=nullptr)
    {
        delete mNetwork;
        mNetwork=nullptr;
    }
    int typecnt=mainWindow->getClassifierCnt();
    mNetwork=new BPNetworkQt(mainWindow,2,typecnt);
    mNetwork->setType(BPNetwork::TYPE::REGRESSION);
    mNetwork->setLearnRate(mainWindow->getLearnRate());
    int cnt=mainWindow->getHiddenLayerCnt();
    int* nodecnt=new int[cnt];
    for(int i=0;i<cnt;i++)
    {
        nodecnt[i]=mainWindow->getHiddenNodeCntByNum(i);
    }
    mNetwork->setHiddenLayer(cnt,nodecnt);
    delete[] nodecnt;
    mNetwork->randWeight();
    int dim=mNodeList.count();
    mNetwork->setSampleDim(dim);
    double* d=new double[dim];
    for(int i=0;i<dim;i++)
    {

        d[i]=mNodeList.at(i).pos().x();
    }
    mNetwork->setInput(0,d);
    d=new double[dim];
    for(int i=0;i<dim;i++)
    {
        d[i]=1.0-mNodeList.at(i).pos().y();
    }
    mNetwork->setInput(1,d);

    for(int j=0;j<typecnt;j++)
    {
        d=new double[dim];
        for(int i=0;i<dim;i++)
        {
            d[i]=mNodeList.at(i).type()==j?1.0:0;
        }
        mNetwork->setOutput(j,d);
    }

    mTrainThread=new TrainThread(mainWindow,this,mNetwork,mMutex);
    if(type==0)
    {
        if(mainWindow->trainByTime())
        {
            mTrainThread->setType(0);
            mTrainThread->setTime(mainWindow->getTrainTime());
        }else
        {
            mTrainThread->setType(1);
            mTrainThread->setErr(mainWindow->getTrainErr());
        }
    }else{
        mTrainThread->setType(2);
    }
    mPaintThread=new PaintThread(mainWindow,this,mNetwork,mMutex);

    connect(mTrainThread,SIGNAL(trainOnce(int,double)),this,SLOT(onTrainOnce(int,double)));
    connect(mNetwork,SIGNAL(sigException(QString)),this,SLOT(onException(QString)));
    connect(this,SIGNAL(notifyDataChanged()),mTrainThread,SLOT(onDataChanged()));
    connect(mTrainThread,SIGNAL(trainFinished()),this,SLOT(onTrainFinished()));
    connect(mainWindow,SIGNAL(sigResetTrainCnt()),mTrainThread,SLOT(onResetTrainCnt()));
    connect(mainWindow,SIGNAL(sigPause(int)),mTrainThread,SLOT(onPause(int)));
    connect(mainWindow,SIGNAL(sigStop()),mTrainThread,SLOT(onStop()));
    connect(mTrainThread,SIGNAL(sigStop()),mainWindow,SLOT(afterStopTrain()));
    connect(mTrainThread,SIGNAL(sigStop()),this,SLOT(onStop()));
    connect(mainWindow->getSbLearnRate(),SIGNAL(valueChanged(double)),mTrainThread,SLOT(onLearnRateChanged(double)));
    connect(mPaintThread,SIGNAL(paintOnce(QPixmap)),this,SLOT(onPaintOnce(QPixmap)));
    connect(mainWindow->getController(),SIGNAL(colorChanged(int,QRgb)),mPaintThread,SLOT(setColor(int,QRgb)));
    mTrainThread->start();
    mPaintThread->start();





}

Surface::~Surface()
{
    delete ui;

    if(mTrainThread!=nullptr)
    {
        if(mTrainThread->isRunning())
        {
            mTrainThread->setIsAlive(false);
        }
        mTrainThread->wait();
        delete mTrainThread;
    }
    if(mPaintThread!=nullptr)
    {
        if(mPaintThread->isRunning())
        {
            mPaintThread->setIsAlive(false);
        }
        mPaintThread->wait();
        delete mPaintThread;
    }
    if(mNetwork!=nullptr)
        delete mNetwork;
    if(mMutex!=nullptr)
        delete mMutex;
}

void Surface::drawHint(QPainter &painter)
{
    QRect r;
    painter.setFont(QFont("微软雅黑",10));
    painter.setPen(QPen(QColor(Qt::black)));
    r.setTopLeft(QPoint(0,0));
    r.setBottomRight(QPoint(40,30));
    painter.drawText(r,Qt::AlignCenter,QStringLiteral("(0,1)"));
    r.setTopLeft(QPoint(rect().width()-40,0));
    r.setBottomRight(QPoint(rect().width(),30));
    painter.drawText(r,Qt::AlignCenter,QStringLiteral("(1,1)"));
    r.setTopLeft(QPoint(0,rect().height()-30));
    r.setBottomRight(QPoint(40,rect().height()));
    painter.drawText(r,Qt::AlignCenter,QStringLiteral("(0,0)"));
    r.setTopLeft(QPoint(rect().width()-40,rect().height()-30));
    r.setBottomRight(QPoint(rect().width(),rect().height()));
    painter.drawText(r,Qt::AlignCenter,QStringLiteral("(1,0)"));
}

void Surface::drawMap(QPainter &painter)
{
    if(!mPixmap.isNull()&&mShowMap)
    {
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.drawPixmap(rect(),mPixmap);
    }
    else
    {
        painter.fillRect(rect(),Qt::white);
    }
}

void Surface::drawNode(QPainter &painter)
{
    painter.setPen(Qt::black);
    for(int i=0;i<mNodeList.count();i++)
    {
        SurfaceNode s=mNodeList.at(i);
        painter.setBrush(mainWindow->getClassifierColor(s.type()));
        QPoint p;
        p.rx()=static_cast<int>(s.pos().x()*rect().width());
        p.ry()=static_cast<int>(s.pos().y()*rect().height());
        int w=NODE_WIDTH;
        QPainterPath path;
        QPointF p2;
        p2.rx()=p.x();
        p2.ry()=p.y()-(w/2);
        path.moveTo(p2);
        for(int i=1;i<=s.type()+3;i++)
        {
            p2.rx()=p.x()+(w/2)*qFastSin(i*M_PI/(s.type()+3)*2);
            p2.ry()=p.y()-(w/2)*qFastCos(i*M_PI/(s.type()+3)*2);
            path.lineTo(p2);
        }
        painter.drawPath(path);
    }
}

void Surface::drawAdder(QPainter &painter)
{
    if(mType!=-1&&!mEraseFlag)
    {
        painter.setPen(Qt::black);
        painter.setBrush(mColor);
        QPoint p;
        p.rx()=static_cast<int>(mPos.x()*rect().width());
        p.ry()=static_cast<int>(mPos.y()*rect().height());
        int w=NODE_WIDTH*3/2;
        QPainterPath path;
        QPointF p2;
        p2.rx()=p.x();
        p2.ry()=p.y()-(w/2);
        path.moveTo(p2);
        for(int i=1;i<=mType+3;i++)
        {
            p2.rx()=p.x()+(w/2)*qFastSin(i*M_PI/(mType+3)*2);
            p2.ry()=p.y()-(w/2)*qFastCos(i*M_PI/(mType+3)*2);
            path.lineTo(p2);
        }
        painter.drawPath(path);
    }
    else if(mEraseFlag)
    {

        painter.setPen(Qt::black);
        painter.setBrush(QBrush(Qt::white));
        painter.setPen(QPen(Qt::black));
        int w=NODE_WIDTH*3/2;
        QPoint p;
        p.rx()=static_cast<int>(mPos.x()*rect().width());
        p.ry()=static_cast<int>(mPos.y()*rect().height());
        painter.drawEllipse(p.x()-w/2,p.y()-w/2,w,w);
    }
}

void Surface::setShowMap(bool showMap)
{
    mShowMap = showMap;
    if(showMap){
        mPixmap.fill(Qt::white);
    }
    update();
}

QList<SurfaceNode> Surface::nodeList() const
{
    return mNodeList;
}

void Surface::setPixmap(const QPixmap &pixmap)
{
    mPixmap = pixmap;
    update();
}


void Surface::paintEvent(QPaintEvent *)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(),Qt::white);
    drawMap(painter);
    drawNode(painter);
    drawAdder(painter);
    drawHint(painter);
    painter.end();
}

void Surface::mouseMoveEvent(QMouseEvent *event)
{
    mPos.rx()=event->pos().x()*1.0/rect().width();
    mPos.ry()=event->pos().y()*1.0/rect().height();
    if(mType!=-1)
    {
        mColor=mainWindow->getClassifierColor(mType);
    }
    if(mEraseFlag)
    {
        for(int i=0;i<mNodeList.count();i++)
        {
            SurfaceNode node=mNodeList.at(i);
            QRect r(static_cast<int>(node.pos().x()*rect().width())-NODE_WIDTH,
                    static_cast<int>(node.pos().y()*rect().height())-NODE_WIDTH,
                    NODE_WIDTH*2,NODE_WIDTH*2);
            if(r.contains(event->pos()))
            {
                mNodeList.removeAt(i);
                emit notifyDataChanged();
                break;
            }

        }
    }
    update();
}

void Surface::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        if(mType!=-1)
        {
            mNodeList.append(SurfaceNode(mType,mPos));
            emit notifyDataChanged();
            update();
        }
    }
    else if(event->button()==Qt::RightButton)
    {
        mEraseFlag=true;
        setCursor(Qt::BlankCursor);
        update();
    }
}

void Surface::mouseReleaseEvent(QMouseEvent *)
{
    if(mEraseFlag)
    {
        mEraseFlag=false;
        if(mType!=-1)
            setCursor(Qt::ClosedHandCursor);
        else
            setCursor(Qt::ArrowCursor);
        update();
    }
}

void Surface::leaveEvent(QEvent *)
{
    mType=-1;
    update();

}

void Surface::enterEvent(QEvent *)
{
    mType=mainWindow->getClassifierNum();
    if(mType!=-1)
    {
        mColor=mainWindow->getClassifierColor(mType);
        setCursor(Qt::ClosedHandCursor);
    }else
    {
        setCursor(Qt::ArrowCursor);
    }
}

void Surface::onTrainOnce(int time,double err)
{
    mainWindow->setCurrentTime(time);
    mainWindow->setCurrentErr(err);
    update();
}

void Surface::onTrainFinished()
{
    mainWindow->afterStopTrain();
}

void Surface::onException(QString s)
{
    QMessageBox::critical(mainWindow,QString::fromLocal8Bit("错误"),s);
    mainWindow->afterStopTrain();
}

void Surface::onStop()
{
    if(mTrainThread!=nullptr)
    {
        if(mTrainThread->isRunning())
        {
            mTrainThread->setIsAlive(false);
        }
        mTrainThread->wait();
        delete mTrainThread;
        mTrainThread=nullptr;
    }
    if(mPaintThread!=nullptr)
    {
        if(mPaintThread->isRunning())
        {
            mPaintThread->setIsAlive(false);
        }
        mPaintThread->wait();
        delete mPaintThread;
        mPaintThread=nullptr;
    }
}

void Surface::onResetSurface()
{
    mShowMap=false;
    update();
}

void Surface::onPaintOnce(QPixmap p)
{
    mPixmap=p;
    update();
}

void Surface::onClearSamples()
{
    mNodeList.clear();
    emit notifyDataChanged();
    update();
}

void Surface::setNodeList(const QList<SurfaceNode> &nodeList)
{
    mNodeList = nodeList;
}

