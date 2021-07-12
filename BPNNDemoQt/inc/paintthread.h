#ifndef PAINTTHREAD_H
#define PAINTTHREAD_H

#include <QThread>
#include <QPixmap>
class BPNetworkQt;
class MainWindow;
class Surface;
class QMutex;
class QPixmap;
class PaintThread : public QThread
{
    Q_OBJECT

public:
    PaintThread(MainWindow* window,Surface* surface,BPNetworkQt* network,QMutex* mutex);
    ~PaintThread();
    void copyWeightAndBias(BPNetworkQt* other);

    bool getIsAlive() const;
    void setIsAlive(bool value);

protected:
    void run();
signals:
    void paintOnce(QPixmap);
private:
    BPNetworkQt *mNetwork;
    BPNetworkQt *mOtherNetwork;
    MainWindow *mWindow;
    Surface* mSurface;
    QMutex *mMutex;
    QRgb getColor(double *);
    QRgb interpolate(QRgb color, double d);
    void setData(uchar *data, int x, int y, int width, uchar r, uchar g, uchar b);
    double findMax(double*,int*);
    int mClassCnt;
    double mMin;
    QRgb* mColors;
    bool isAlive;
public slots:
    void setColor(int i,QRgb color);

};

#endif // PAINTTHREAD_H
