#ifndef NETWORK_H
#define NETWORK_H
#include <iostream>

class BPNetwork
{
public:
    enum TYPE{
        REGRESSION,NORMAL
    };
    BPNetwork(int inputNodeCnt, int outputNodeCnt);
    virtual ~BPNetwork();

    int inputNodeCnt() const;
    void setInputNodeCnt(int inputNodeCnt);

    int outputNodeCnt() const;
    void setOutputNodeCnt(int outputNodeCnt);

    int hiddenLayerCnt() const;
    void setHiddenLayer(int hiddenLayerCnt,...);
    void setHiddenLayer(int hiddenLayerCnt,int value[]);

    void setSampleDim(int sampleDim);
    void setInput(int n,double* input);
    void setOutput(int n,double* output);

    void randWeight();
    void bp();
    void fp();
    void reset();

    double getLearnRate() const;
    void setLearnRate(double learnRate);

    int getTrainCnt() const;
    void resetTrainCnt();

    virtual void trainByTime(int);
    virtual void trainByErr(double);
    virtual void trainByErrM(int);
    void test(double* input);

    int getType() const;
    void setType(TYPE type);
    bool checkSample();
    double train();

    void copyWeightAndBias(BPNetwork* other);
    void copyInit(BPNetwork* other);
    double *getTestOutput() const;
    double getTestOutput(int);

    int *getHiddenNodeCnt() const;

private:
    int mInputNodeCnt=0;
    int mOutputNodeCnt=0;
    int mHiddenLayerCnt=0;
    int mSampleDim=0;
    double mLearnRate=1;
    double err=0.01;
    int* mHiddenNodeCnt=nullptr; //隐层节点数
    double ***mWeight;
    double **mHiddenIn;
    double **mHiddenOut;
    double **mHiddenTmp;
    double **mInput;
    double *mInputTmp;
    double *mOutputTmp;
    double **mOutput;
    double *mOutputIn;
    double *mActualOutput;
    double *mErr;
    double *mTestOutput;
    double **mBias;
    TYPE mType=REGRESSION;
    int mTrainCnt=0;
    double mTotalError=0;
    //double (*activeFunc)(int,double);
    double activeFunc(double,int);
    virtual void exception(std::string ex="Default Exception");
    void calcErr();
    double getError();



};

#endif // NETWORK_H
