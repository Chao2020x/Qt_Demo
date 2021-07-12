#ifndef SURFACE_H
#define SURFACE_H

#include <QWidget>
#include <QMutex>
#include <QPixmap>
class BPNetworkQt;
class SurfaceNode;
class MainWindow;
class TrainThread;
class PaintThread;
namespace Ui {
class Surface;
}

class Surface : public QWidget
{
    Q_OBJECT

public:
    explicit Surface(QWidget *parent = nullptr);
    void deleteNodeByClassCnt(int cnt);
    void startTrain(int type);
    ~Surface();

    QImage *image() const;
    void setImage(QImage *image);

    void setPixmap(const QPixmap &pixmap);

    QList<SurfaceNode> nodeList() const;

    void setShowMap(bool showMap);

    void setNodeList(const QList<SurfaceNode> &nodeList);

private:
    Ui::Surface *ui;
    void drawHint(QPainter&);
    void drawMap(QPainter&);
    void drawNode(QPainter&);
    void drawAdder(QPainter&);
    MainWindow* mainWindow;
    int mType;
    bool mEraseFlag;
    QColor mColor;
    QPointF mPos;
    QList<SurfaceNode> mNodeList;
    BPNetworkQt* mNetwork;
    QPixmap mPixmap;
    bool mShowMap;
    TrainThread* mTrainThread;
    PaintThread* mPaintThread;
    QMutex *mMutex;

protected:
    void paintEvent(QPaintEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void leaveEvent(QEvent*);
    void enterEvent(QEvent*);
signals:
    void notifyDataChanged();
public slots:
    void onTrainOnce(int time,double err);
    void onTrainFinished();
    void onException(QString s);
    void onStop();
    void onResetSurface();
    void onPaintOnce(QPixmap);
    void onClearSamples();




};

#endif // SURFACE_H
