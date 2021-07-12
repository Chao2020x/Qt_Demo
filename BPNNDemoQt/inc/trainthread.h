#ifndef TRAINTHREAD_H
#define TRAINTHREAD_H

#include <QThread>
#include <QRgb>
class Surface;
class BPNetworkQt;
class MainWindow;
class QMutex;
class TrainThread : public QThread
{
    Q_OBJECT
public:
    TrainThread(MainWindow *window, Surface *surface, BPNetworkQt *network, QMutex *mutex);
    ~TrainThread();
    int type() const;
    void setType(int type);

    double err() const;
    void setErr(double err);

    int time() const;
    void setTime(int time);

    bool getIsAlive() const;
    void setIsAlive(bool value);

public slots:
    void onDataChanged();
    void onResetTrainCnt();
    void onStop();
    void onPause(int i);
    void onLearnRateChanged(double value);

private:
    BPNetworkQt* mNetwork;
    MainWindow* mWindow;
    Surface* mSurface;
    QMutex* mMutex;
    int mType;
    double mErr;
    int mTime;
    int mClassCnt;
    double mCurrentErr;
    bool mChanged;
    bool mPause;
    bool isAlive;
    void train();

protected:
    void run();
signals:
    void trainOnce(int,double);
    void trainFinished();
    void sigStop();
};

#endif // TRAINTHREAD_H
