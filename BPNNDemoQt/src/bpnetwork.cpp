#include "inc/bpnetwork.h"
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

BPNetwork::BPNetwork(int inputNodeCnt,int outputNodeCnt)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    mInputNodeCnt=inputNodeCnt;
    mOutputNodeCnt=outputNodeCnt;
    mHiddenNodeCnt=nullptr;
    mHiddenIn=nullptr;
    mHiddenOut=nullptr;
    mHiddenTmp=nullptr;
    mBias=nullptr;
    mHiddenLayerCnt=0;
    mTrainCnt=0;
    mWeight=nullptr;
    mInputTmp=new double[inputNodeCnt];
    mOutputTmp=new double[outputNodeCnt];
    mInput=new double*[inputNodeCnt];
    for(int i=0;i<inputNodeCnt;i++)
        mInput[i]=nullptr;
    mOutput=new double*[outputNodeCnt];
    for(int i=0;i<outputNodeCnt;i++)
        mOutput[i]=nullptr;
    mOutputIn=new double[mOutputNodeCnt];
    mActualOutput=new double[mOutputNodeCnt];
    mErr=new double[mOutputNodeCnt];
    mTestOutput=new double[mOutputNodeCnt];
}

BPNetwork::~BPNetwork()
{
    if(mHiddenLayerCnt!=0)
    {
        if(mWeight!=nullptr)
        {
            for(int i=0;i<mHiddenLayerCnt+1;i++)
            {
                if(i==0)
                {
                    for(int j=0;j<mInputNodeCnt;j++)
                    {
                        delete[] mWeight[i][j];
                    }
                }
                else{
                    for(int j=0;j<mHiddenNodeCnt[i-1];j++)
                    {
                        delete[] mWeight[i][j];
                    }
                }
                delete[] mWeight[i];
            }
            delete[] mWeight;
        }
        for(int i=0;i<mHiddenLayerCnt;i++)
        {
            delete[] mHiddenIn[i];
            delete[]  mHiddenOut[i];
            delete[] mHiddenTmp[i];
            delete[] mBias[i];
        }
        delete[] mBias[mHiddenLayerCnt];
        delete[] mHiddenIn;
        delete[] mHiddenOut;
        delete[] mHiddenTmp;
        delete[] mBias;
        delete[] mHiddenNodeCnt;

    }
    delete[] mInputTmp;
    delete[] mOutputTmp;
    for(int i=0;i<mInputNodeCnt;i++)
        if(mInput[i]!=nullptr)
            delete[] mInput[i];
    delete[] mInput;
    for(int i=0;i<mOutputNodeCnt;i++)
        if(mOutput[i]!=nullptr)
            delete[] mOutput[i];
    delete[] mOutput;
    delete[] mOutputIn;
    delete[] mActualOutput;
    delete[] mErr;
    delete[] mTestOutput;
}

int BPNetwork::inputNodeCnt() const
{
    return mInputNodeCnt;
}

void BPNetwork::setInputNodeCnt(int inputNodeCnt)
{
    mInputNodeCnt = inputNodeCnt;
}

int BPNetwork::outputNodeCnt() const
{
    return mOutputNodeCnt;
}

void BPNetwork::setOutputNodeCnt(int outputNodeCnt)
{
    mOutputNodeCnt = outputNodeCnt;
}

int BPNetwork::hiddenLayerCnt() const
{
    return mHiddenLayerCnt;
}

void BPNetwork::setHiddenLayer(int hiddenLayerCnt,...)
{
    int oriCnt=mHiddenLayerCnt;
    mHiddenLayerCnt = hiddenLayerCnt;
    if(mHiddenNodeCnt!=nullptr){
        delete[] mHiddenNodeCnt;
        mHiddenNodeCnt=nullptr;
    }
    mHiddenNodeCnt=new int[mHiddenLayerCnt];
    va_list li;
    va_start(li,hiddenLayerCnt);
    for(int i=0;i<hiddenLayerCnt;i++){
        mHiddenNodeCnt[i]=va_arg(li,int);
    }
    va_end(li);
    if(mWeight!=nullptr)
    {
        for(int i=0;i<oriCnt+1;i++)
        {
            if(i==0)
            {
                for(int j=0;j<mInputNodeCnt;j++)
                {
                    delete[] mWeight[i][j];
                }
            }
            else{
                for(int j=0;j<mHiddenNodeCnt[i-1];j++)
                {
                    delete[] mWeight[i][j];
                }
            }
            delete[] mWeight[i];
        }
        delete[] mWeight;
    }
    mWeight=new double**[mHiddenLayerCnt+1];
    mWeight[0]=new double*[mInputNodeCnt];
    for(int i=0;i<mInputNodeCnt;i++)
    {
        mWeight[0][i]=new double[mHiddenNodeCnt[0]];
    }
    for(int k=1;k<mHiddenLayerCnt;k++)
    {
        mWeight[k]=new double*[mHiddenNodeCnt[k-1]];
        for(int i=0;i<mHiddenNodeCnt[k-1];i++)
        {
            mWeight[k][i]=new double[mHiddenNodeCnt[k]];
        }
    }
    mWeight[mHiddenLayerCnt]=new double*[mHiddenNodeCnt[mHiddenLayerCnt-1]];
    for(int i=0;i<mHiddenNodeCnt[mHiddenLayerCnt-1];i++)
    {
        mWeight[mHiddenLayerCnt][i]=new double[mOutputNodeCnt];
    }
    if(mHiddenIn!=nullptr)
    {
        for(int i=0;i<oriCnt;i++)
        {
            delete[] mHiddenIn[i];
        }
        delete[] mHiddenIn;
    }
    if(mHiddenOut!=nullptr)
    {
        for(int i=0;i<oriCnt;i++)
        {
            delete[] mHiddenOut[i];
        }
        delete[] mHiddenOut;
    }
    if(mHiddenTmp!=nullptr)
    {
        for(int i=0;i<oriCnt;i++)
        {
            delete[] mHiddenTmp[i];
        }
        delete[] mHiddenTmp;
    }
    if(mBias!=nullptr)
    {
        for(int i=0;i<oriCnt;i++)
        {
            delete[] mBias[i];
        }
        delete[] mBias;
    }


    mHiddenIn=new double*[mHiddenLayerCnt];
    mHiddenOut=new double*[mHiddenLayerCnt];
    mHiddenTmp=new double*[mHiddenLayerCnt];
    mBias=new double*[mHiddenLayerCnt+1];
    for(int i=0;i<mHiddenLayerCnt;i++)
    {
        mHiddenIn[i]=new double[mHiddenNodeCnt[i]];
        mHiddenOut[i]=new double[mHiddenNodeCnt[i]];
        mHiddenTmp[i]=new double[mHiddenNodeCnt[i]];
        mBias[i]=new double[mHiddenNodeCnt[i]];
    }
    mBias[mHiddenLayerCnt]=new double[mOutputNodeCnt];
}

void BPNetwork::setHiddenLayer(int hiddenLayerCnt, int value[])
{
    int oriCnt=mHiddenLayerCnt;
    mHiddenLayerCnt = hiddenLayerCnt;
    if(mHiddenNodeCnt!=nullptr){
        delete[] mHiddenNodeCnt;
        mHiddenNodeCnt=nullptr;
    }
    mHiddenNodeCnt=new int[mHiddenLayerCnt];

    for(int i=0;i<hiddenLayerCnt;i++){
        mHiddenNodeCnt[i]=value[i];
    }
    if(mWeight!=nullptr)
    {
        for(int i=0;i<oriCnt+1;i++)
        {
            if(i==0)
            {
                for(int j=0;j<mInputNodeCnt;j++)
                {
                    delete[] mWeight[i][j];
                }
            }
            else{
                for(int j=0;j<mHiddenNodeCnt[i-1];j++)
                {
                    delete[] mWeight[i][j];
                }
            }
            delete[] mWeight[i];
        }
        delete[] mWeight;
    }
    mWeight=new double**[mHiddenLayerCnt+1];
    mWeight[0]=new double*[mInputNodeCnt];
    for(int i=0;i<mInputNodeCnt;i++)
    {
        mWeight[0][i]=new double[mHiddenNodeCnt[0]];
    }
    for(int k=1;k<mHiddenLayerCnt;k++)
    {
        mWeight[k]=new double*[mHiddenNodeCnt[k-1]];
        for(int i=0;i<mHiddenNodeCnt[k-1];i++)
        {
            mWeight[k][i]=new double[mHiddenNodeCnt[k]];
        }
    }
    mWeight[mHiddenLayerCnt]=new double*[mHiddenNodeCnt[mHiddenLayerCnt-1]];
    for(int i=0;i<mHiddenNodeCnt[mHiddenLayerCnt-1];i++)
    {
        mWeight[mHiddenLayerCnt][i]=new double[mOutputNodeCnt];
    }
    if(mHiddenIn!=nullptr)
    {
        for(int i=0;i<oriCnt;i++)
        {
            delete[] mHiddenIn[i];
        }
        delete[] mHiddenIn;
    }
    if(mHiddenOut!=nullptr)
    {
        for(int i=0;i<oriCnt;i++)
        {
            delete[] mHiddenOut[i];
        }
        delete[] mHiddenOut;
    }
    if(mHiddenTmp!=nullptr)
    {
        for(int i=0;i<oriCnt;i++)
        {
            delete[] mHiddenTmp[i];
        }
        delete[] mHiddenTmp;
    }
    if(mBias!=nullptr)
    {
        for(int i=0;i<oriCnt;i++)
        {
            delete[] mBias[i];
        }
        delete[] mBias;
    }


    mHiddenIn=new double*[mHiddenLayerCnt];
    mHiddenOut=new double*[mHiddenLayerCnt];
    mHiddenTmp=new double*[mHiddenLayerCnt];
    mBias=new double*[mHiddenLayerCnt+1];
    for(int i=0;i<mHiddenLayerCnt;i++)
    {
        mHiddenIn[i]=new double[mHiddenNodeCnt[i]];
        mHiddenOut[i]=new double[mHiddenNodeCnt[i]];
        mHiddenTmp[i]=new double[mHiddenNodeCnt[i]];
        mBias[i]=new double[mHiddenNodeCnt[i]];
    }
    mBias[mHiddenLayerCnt]=new double[mOutputNodeCnt];
}

void BPNetwork::setSampleDim(int sampleDim)
{
    mSampleDim = sampleDim;

}

void BPNetwork::setInput(int n, double* input)
{
    if(n<mInputNodeCnt)
    {
        if(mInput[n]!=nullptr)
        {
            delete[] mInput[n];
            mInput[n]=nullptr;
        }
        mInput[n]=input;
    }else
    {
        exception("Input count error!");
    }
}

void BPNetwork::setOutput(int n, double* output)
{
    if(n<mOutputNodeCnt)
    {
        if(mOutput[n]!=nullptr)
        {
            delete[] mOutput[n];
            mOutput[n]=nullptr;
        }
        mOutput[n]=output;
    }else
    {
        exception("Output count error!");
    }
}



void BPNetwork::reset()
{
    mTrainCnt=0;
}


double BPNetwork::getLearnRate() const
{
    return mLearnRate;
}

void BPNetwork::setLearnRate(double learnRate)
{
    mLearnRate = learnRate;
}

int BPNetwork::getTrainCnt() const
{
    return mTrainCnt;
}

void BPNetwork::resetTrainCnt()
{
    mTrainCnt = 0;
}

double BPNetwork::activeFunc(double value,int dir)
{
    if(dir==0)
    {
        return 1.0/(1.0+exp(-value));
    }else
    {
        return value*(1.0-value);
    }
}

void BPNetwork::randWeight()
{
    for(int i=0;i<mInputNodeCnt;i++)
    {
        for(int j=0;j<mHiddenNodeCnt[0];j++)
            mWeight[0][i][j]=static_cast<double>(rand())/RAND_MAX;
    }
    for(int k=1;k<mHiddenLayerCnt;k++)
    {
        for(int i=0;i<mHiddenNodeCnt[k-1];i++)
        {
            for(int j=0;j< mHiddenNodeCnt[k];j++)
                mWeight[k][i][j]=static_cast<double>(rand())/RAND_MAX;
        }
    }
    for(int i=0;i<mHiddenNodeCnt[mHiddenLayerCnt-1];i++)
    {
        for(int j=0;j<mOutputNodeCnt;j++)
            mWeight[mHiddenLayerCnt][i][j]=static_cast<double>(rand())/RAND_MAX;
    }
    for(int i=0;i<mHiddenLayerCnt;i++)
    {
        for(int j=0;j<mHiddenNodeCnt[i];j++)
        {
            mBias[i][j]=static_cast<double>(rand())/RAND_MAX;
        }

    }
    for(int j=0;j<mOutputNodeCnt;j++)
    {
        mBias[mHiddenLayerCnt][j]=static_cast<double>(rand())/RAND_MAX;
    }

}

void BPNetwork::exception(std::string msg)
{
    //  qDebug()<<QString::fromStdString("Exception:"+msg);
    throw "Exception:"+msg;


}


void BPNetwork::test(double *input)
{
    //reset
    for(int i=0;i<mHiddenLayerCnt;i++)
    {
        for(int j=0;j<mHiddenNodeCnt[i];j++)
        {

            mHiddenIn[i][j]=0;

        }
    }

    for(int i=0;i<mOutputNodeCnt;i++)
    {

        mOutputIn[i]=0;

    }
    //reset end

    for(int i=0;i<mHiddenNodeCnt[0];i++)
    {
        for(int j=0;j<mInputNodeCnt;j++)
        {
            mHiddenIn[0][i]+=mWeight[0][j][i]*input[j];
        }
        mHiddenIn[0][i]+=mBias[0][i];

    }

    for(int m=0;m<mHiddenLayerCnt-1;m++)
    {
        for(int i=0;i<mHiddenNodeCnt[m];i++)
        {
            mHiddenOut[m][i]=activeFunc(mHiddenIn[m][i],0);
        }
        for(int i=0;i<mHiddenNodeCnt[m+1];i++)
        {
            for(int j=0;j<mHiddenNodeCnt[m];j++)
            {
                mHiddenIn[m+1][i]+=mWeight[m+1][j][i]*mHiddenOut[m][j];
            }

            mHiddenIn[m+1][i]+=mBias[m+1][i];
        }
    }

    for(int i=0;i<mHiddenNodeCnt[mHiddenLayerCnt-1];i++)
    {

        mHiddenOut[mHiddenLayerCnt-1][i]=activeFunc(mHiddenIn[mHiddenLayerCnt-1][i],0);

    }

    for(int i=0;i<mOutputNodeCnt;i++)
    {
        for(int j=0;j<mHiddenNodeCnt[mHiddenLayerCnt-1];j++)
        {
            mOutputIn[i]+=mWeight[mHiddenLayerCnt][j][i]*mHiddenOut[mHiddenLayerCnt-1][j];
        }

        mOutputIn[i]+=mBias[mHiddenLayerCnt][i];
    }

    for(int i=0;i<mOutputNodeCnt;i++)
    {
        if(mType==TYPE::REGRESSION)
            mTestOutput[i]=activeFunc(mOutputIn[i],0);
        else if(mType==TYPE::NORMAL)
            mTestOutput[i]=mOutputIn[i];

    }

}

int BPNetwork::getType() const
{
    return mType;
}

void BPNetwork::setType(TYPE type)
{
    mType = type;
}


double BPNetwork::getError()
{
    double temp=0;

    for(int j=0;j<mOutputNodeCnt;j++)
    {
        temp+=mErr[j]*mErr[j]/2;
    }

    return temp/mOutputNodeCnt;

}
void BPNetwork::fp()
{

    //reset

    for(int i=0;i<mHiddenLayerCnt;i++)
    {
        for(int j=0;j<mHiddenNodeCnt[i];j++)
        {

            mHiddenIn[i][j]=0;

        }
    }

    for(int i=0;i<mOutputNodeCnt;i++)
    {

        mOutputIn[i]=0;

    }
    //reset end

    for(int i=0;i<mHiddenNodeCnt[0];i++)
    {
        for(int j=0;j<mInputNodeCnt;j++)
        {
            mHiddenIn[0][i]+=mWeight[0][j][i]*mInputTmp[j];
        }
        mHiddenIn[0][i]+=mBias[0][i];

    }

    for(int m=0;m<mHiddenLayerCnt-1;m++)
    {
        for(int i=0;i<mHiddenNodeCnt[m];i++)
        {
            mHiddenOut[m][i]=activeFunc(mHiddenIn[m][i],0);
        }
        for(int i=0;i<mHiddenNodeCnt[m+1];i++)
        {
            for(int j=0;j<mHiddenNodeCnt[m];j++)
            {
                mHiddenIn[m+1][i]+=mWeight[m+1][j][i]*mHiddenOut[m][j];
            }

            mHiddenIn[m+1][i]+=mBias[m+1][i];
        }
    }

    for(int i=0;i<mHiddenNodeCnt[mHiddenLayerCnt-1];i++)
    {

        mHiddenOut[mHiddenLayerCnt-1][i]=activeFunc(mHiddenIn[mHiddenLayerCnt-1][i],0);

    }

    for(int i=0;i<mOutputNodeCnt;i++)
    {
        for(int j=0;j<mHiddenNodeCnt[mHiddenLayerCnt-1];j++)
        {
            mOutputIn[i]+=mWeight[mHiddenLayerCnt][j][i]*mHiddenOut[mHiddenLayerCnt-1][j];
        }

        mOutputIn[i]+=mBias[mHiddenLayerCnt][i];
    }

    for(int i=0;i<mOutputNodeCnt;i++)
    {
        if(mType==TYPE::REGRESSION)
            mActualOutput[i]=activeFunc(mOutputIn[i],0);
        else if(mType==TYPE::NORMAL)
            mActualOutput[i]=mOutputIn[i];
    }
}

void BPNetwork::bp()
{

    //初始化
    for(int i=0;i<mHiddenLayerCnt;i++)
    {
        for(int j=0;j<mHiddenNodeCnt[i];j++)
        {

            mHiddenTmp[i][j]=0;
        }
    }
    //计算输出层到最后一个隐藏层权重更新
    double o=0;
    for(int i=0;i<mOutputNodeCnt;i++)
    {
        if(mType==TYPE::REGRESSION)
            o=activeFunc(mActualOutput[i],1)*mErr[i];
        else if(mType==TYPE::NORMAL)
            o=mErr[i];
        for(int j=0;j<mHiddenNodeCnt[mHiddenLayerCnt-1];j++)
        {
            mHiddenTmp[mHiddenLayerCnt-1][j]+=o*mWeight[mHiddenLayerCnt][j][i];
            mWeight[mHiddenLayerCnt][j][i]-=mLearnRate*o*mHiddenOut[mHiddenLayerCnt-1][j];

        }
        mBias[mHiddenLayerCnt][i]-=mLearnRate*o;

    }

    //计算最后一个隐藏层到第一个隐藏层的权重更新
    for(int i=mHiddenLayerCnt-1;i>0;i--)
    {
        for(int j=0;j<mHiddenNodeCnt[i];j++)
        {
            o=activeFunc(mHiddenOut[i][j],1)*mHiddenTmp[i][j];
            for(int l=0;l<mHiddenNodeCnt[i-1];l++)
            {
                mHiddenTmp[i-1][l]+=o*mWeight[i][l][j];
                mWeight[i][l][j]-=mLearnRate*o*mHiddenOut[i-1][l];

            }
            mBias[i][j]-=mLearnRate*o;
        }

    }

    //计算第一个隐藏层到输入层的权重更新
    for(int i=0;i<mHiddenNodeCnt[0];i++)
    {
        o=activeFunc(mHiddenOut[0][i],1)*mHiddenTmp[0][i];
        for(int j=0;j<mInputNodeCnt;j++)
        {
            mWeight[0][j][i]-=mLearnRate*o*mInputTmp[j];

        }
        mBias[0][i]-=mLearnRate*o;

    }


}
void BPNetwork::calcErr()
{
    for(int i=0;i<mOutputNodeCnt;i++)
    {
        mErr[i]=mActualOutput[i]-mOutputTmp[i];
    }

}


double BPNetwork::train()
{

    double err0=0;
    mTrainCnt++;
    for(int i=0;i<mSampleDim;i++)
    {
        for(int j=0;j<mInputNodeCnt;j++)
            mInputTmp[j]=mInput[j][i];
        for(int j=0;j<mOutputNodeCnt;j++)
            mOutputTmp[j]=mOutput[j][i];
        fp();
        calcErr();
        bp();
        err0+=getError();
    }
    return err0;

}

void BPNetwork::copyWeightAndBias(BPNetwork *other)
{
    for(int i=0;i<mInputNodeCnt;i++)
    {
        for(int j=0;j<mHiddenNodeCnt[0];j++)
            mWeight[0][i][j]=other->mWeight[0][i][j];
    }
    for(int k=1;k<mHiddenLayerCnt;k++)
    {
        for(int i=0;i<mHiddenNodeCnt[k-1];i++)
        {
            for(int j=0;j< mHiddenNodeCnt[k];j++)
                mWeight[k][i][j]=other->mWeight[k][i][j];
        }
    }
    for(int i=0;i<mHiddenNodeCnt[mHiddenLayerCnt-1];i++)
    {
        for(int j=0;j<mOutputNodeCnt;j++)
            mWeight[mHiddenLayerCnt][i][j]=other-> mWeight[mHiddenLayerCnt][i][j];
    }
    for(int i=0;i<mHiddenLayerCnt;i++)
    {
        for(int j=0;j<mHiddenNodeCnt[i];j++)
        {
            mBias[i][j]=other-> mBias[i][j];
        }

    }
    for(int j=0;j<mOutputNodeCnt;j++)
    {
        mBias[mHiddenLayerCnt][j]=other-> mBias[mHiddenLayerCnt][j];
    }
}

void BPNetwork::copyInit(BPNetwork *other)
{
    int t=other->hiddenLayerCnt();
    int* d=new int[t];
    for(int i=0;i<t;i++)
    {
        d[i]=other->getHiddenNodeCnt()[i];
    }
    setHiddenLayer(t,d);
}

double *BPNetwork::getTestOutput() const
{
    return mTestOutput;
}

double BPNetwork::getTestOutput(int i)
{
    return mTestOutput[i];
}

int *BPNetwork::getHiddenNodeCnt() const
{
    return mHiddenNodeCnt;
}



bool BPNetwork::checkSample()
{
    if(mSampleDim<=0||mInput==nullptr||mOutput==nullptr)
    {
        exception("Error sample!");
        return false;
    }
    else if(mInput!=nullptr)
    {
        for(int i=0;i<mInputNodeCnt;i++)
            if(mInput[i]==nullptr)
            {
                exception("Error input sample!");
                return false;
            }
    }
    else if(mOutput!=nullptr)
    {
        for(int i=0;i<mOutputNodeCnt;i++)
            if(mOutput[i]==nullptr)
            {
                exception("Error output sample!");
                return false;
            }
    }
    return true;
}

void BPNetwork::trainByTime(int time)
{
    if(checkSample())
    {
        mTrainCnt=0;
        while(time-->0)
            train();
    }
}

void BPNetwork::trainByErr(double err)
{
    if(checkSample())
    {
        mTrainCnt=0;
        double err0=err+1;
        while(err0>=err)
            err0=train();
    }
}

void BPNetwork::trainByErrM(int m)
{
    trainByErr(pow(10,-m));
}

