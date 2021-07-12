#include "inc/classifiercolor.h"
#include <QPainter>
ClassifierColor::ClassifierColor(QWidget* )
{

}

QColor ClassifierColor::color() const
{
    return mColor;
}

void ClassifierColor::setColor(const QColor &color)
{
    mColor = color;
}


void ClassifierColor::paintEvent(QPaintEvent *)
{
    QPainter painter;
    painter.begin(this);
    painter.setPen(Qt::gray);
    painter.drawRect(rect().adjusted(0,0,-1,-1));
    painter.fillRect(rect().adjusted(1,1,-1,-1),QBrush(mColor));
    painter.end();
}
