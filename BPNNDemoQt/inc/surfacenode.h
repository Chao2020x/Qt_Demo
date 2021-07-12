#ifndef SURFACENODE_H
#define SURFACENODE_H

#include <QPoint>
#include <QColor>
class SurfaceNode
{
public:
    SurfaceNode(int type,QPointF pos);
    int type() const;
    void setType(int type);


    QPointF pos() const;
    void setPos(const QPointF &pos);

private:
    int mType;
    QPointF mPos;


};

#endif // SURFACENODE_H
