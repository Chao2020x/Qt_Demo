#include "inc/paintthread.h"
#include "inc/mainwindow.h"
#include "inc/surface.h"
#include "inc/bpnetworkqt.h"
#include <QMutex>

PaintThread::PaintThread(MainWindow *window, Surface *surface, BPNetworkQt *network,QMutex* mutex):
    mOtherNetwork(network),
    mWindow(window),
    mSurface(surface),
    mMutex(mutex)

{

    mNetwork=new BPNetworkQt(mWindow,mOtherNetwork->inputNodeCnt(),mOtherNetwork->outputNodeCnt());
    mNetwork->copyInit(mOtherNetwork);
    mClassCnt=mWindow->getClassifierCnt();
    mMin=1.0/mClassCnt;
    mColors=new QRgb[static_cast<uint>(mClassCnt)];
    for(int i=0;i<mClassCnt;i++)
    {
        mColors[i]=mWindow->getClassifierColor(i).rgb();

    }
    mSurface->setShowMap(true);
}

PaintThread::~PaintThread()
{
    delete mNetwork;
    delete[] mColors;
}

void PaintThread::copyWeightAndBias(BPNetworkQt *other)
{
    mNetwork->copyWeightAndBias(other);
}

void PaintThread::run()
{
    setIsAlive(true);
    while(getIsAlive()){
        mMutex->lock();
        copyWeightAndBias(mOtherNetwork);
        mMutex->unlock();
        int height=mSurface->height();
        int width=mSurface->width();
        height=height/4*4;
        width=width/4*4;
        QImage image(width,height,QImage::Format_RGB888);
        uchar* data=image.bits();
        for(int i=0;i<width;i++)
            for(int j=0;j<height;j++)
            {
                double input[2];
                input[0]=i*1.0/width;
                input[1]=1.0-j*1.0/height;
                mNetwork->test(input);
                double* output=mNetwork->getTestOutput();
                QRgb color=getColor(output);
                //image.setPixelColor(i,j,QColor(color));
                setData(data,i,j,width,
                        static_cast<uchar>(qRed(color)),
                        static_cast<uchar>(qGreen(color)),
                        static_cast<uchar>(qBlue(color)));
            }
        emit paintOnce(QPixmap::fromImage(image));
    }

}

QRgb PaintThread::getColor(double* output)
{

    int idx;
    findMax(output,&idx);
    //return interpolate(mColors[idx],value);
    return mColors[idx];
}
double PaintThread::findMax(double *d, int *i)
{
    int idx=-1;
    double value=-1;
    for(int i=0;i<mClassCnt;i++)
    {
        if(d[i]>value)
        {
            value=d[i];
            idx=i;
        }
    }
    *i=idx;
    return value;
}

bool PaintThread::getIsAlive() const
{
    return isAlive;
}

void PaintThread::setIsAlive(bool value)
{
    isAlive = value;
}

void PaintThread::setColor(int i, QRgb color)
{
    mColors[i]=color;
}
QRgb PaintThread::interpolate(QRgb color,double d)
{

    int r=qRed(color);
    int g=qGreen(color);
    int b=qBlue(color);
    int r1=static_cast<int>(255-(255-r)*(d-mMin)/(1.0-mMin));
    int g1=static_cast<int>(255-(255-g)*(d-mMin)/(1.0-mMin));
    int b1=static_cast<int>(255-(255-b)*(d-mMin)/(1.0-mMin));
    r1=r1>255?255:r1;
    g1=g1>255?255:g1;
    b1=b1>255?255:b1;
    return qRgb(r1,g1,b1);

}

void PaintThread::setData(uchar *data, int x, int y, int width,uchar r,uchar g,uchar b)
{
    *(data+(y*width*3)+x*3)=r;
    *(data+(y*width*3)+x*3+1)=g;
    *(data+(y*width*3)+x*3+2)=b;
}
