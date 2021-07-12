#include "inc/trainthread.h"
#include "inc/bpnetworkqt.h"
#include "inc/mainwindow.h"
#include "inc/surface.h"
#include "inc/surfacenode.h"
#include <QMutex>


TrainThread::TrainThread(MainWindow* window, Surface* surface, BPNetworkQt *network, QMutex* mutex):
    mNetwork(network),
    mWindow(window),
    mSurface(surface),
    mMutex(mutex)
{

    mChanged=false;
    mPause=false;
    mClassCnt=mWindow->getClassifierCnt();

}

TrainThread::~TrainThread()
{
}

int TrainThread::type() const
{
    return mType;
}

void TrainThread::setType(int type)
{
    mType = type;
}

double TrainThread::err() const
{
    return mErr;
}

void TrainThread::setErr(double err)
{
    mErr = err;
}

int TrainThread::time() const
{
    return mTime;
}

void TrainThread::setTime(int time)
{
    mTime = time;
}


void TrainThread::onDataChanged()
{
    mChanged=true;
}

void TrainThread::onResetTrainCnt()
{
    mNetwork->reset();
}

void TrainThread::onStop()
{
    emit sigStop();

}

void TrainThread::onPause(int i)
{
    if(i==0)
    {
        mPause=true;
    }else{
        mPause=false;

    }
}

void TrainThread::onLearnRateChanged(double value)
{
    mNetwork->setLearnRate(value);
}

bool TrainThread::getIsAlive() const
{
    return isAlive;
}

void TrainThread::setIsAlive(bool value)
{
    isAlive = value;
}

void TrainThread::train()
{

    if(mChanged)
    {
        mChanged=false;
        QList<SurfaceNode> mNodeList=mSurface->nodeList();
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
        int typecnt=mWindow->getClassifierCnt();
        for(int j=0;j<typecnt;j++)
        {
            d=new double[dim];
            for(int i=0;i<dim;i++)
            {
                d[i]=mNodeList.at(i).type()==j?1.0:0;
            }
            mNetwork->setOutput(j,d);
        }
    }
    mMutex->lock();
    mCurrentErr=mNetwork->train();
    mMutex->unlock();
    emit trainOnce(mNetwork->getTrainCnt(),mCurrentErr);
    //QThread::usleep(0);
}


void TrainThread::run()
{
    setIsAlive(true);
    if(mType==0)
    {
        if(mNetwork->checkSample())
        {
            mNetwork->reset();
            while(getIsAlive() && mNetwork->getTrainCnt() < mTime){
                while(mPause);
                train();
            }
            emit trainFinished();
        }
    }else if(mType==1){

        if(mNetwork->checkSample())
        {
            mNetwork->reset();
            mCurrentErr=mErr+1;
            while(getIsAlive() && mCurrentErr >= mErr){
                while(mPause);
                train();

            }
            emit trainFinished();
        }

    }else{
        if(mNetwork->checkSample())
        {
            mNetwork->reset();
            while(getIsAlive()) {
                while(mPause);
                train();
            }
            emit trainFinished();
        }
    }
}
