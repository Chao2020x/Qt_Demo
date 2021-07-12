#include "inc/classifiertype.h"
#include <QPainter>
#include <algorithm>
#include <QPainterPath>
#include <QtMath>
using namespace std;
ClassifierType::ClassifierType(QWidget*)
{

}

int ClassifierType::type() const
{
    return mType;
}

void ClassifierType::setType(int type)
{
    mType = type;
}

void ClassifierType::paintEvent(QPaintEvent *)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::black);
    QPoint p(rect().width()/2,rect().height()/2);
    int w=min(rect().width(),rect().height())*2/3;
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
    painter.end();

}
