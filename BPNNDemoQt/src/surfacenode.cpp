#include "inc/surfacenode.h"

SurfaceNode::SurfaceNode(int type, QPointF pos)
{
    mType=type;
    mPos=pos;
}

int SurfaceNode::type() const
{
    return mType;
}

void SurfaceNode::setType(int type)
{
    mType = type;
}



QPointF SurfaceNode::pos() const
{
    return mPos;
}

void SurfaceNode::setPos(const QPointF &pos)
{
    mPos = pos;
}


