#include "test.h"

carRoundCircle::carRoundCircle(QPointF p,std::vector<QPoint> a):
    carRound(a)
{
    this->center = p;
    this->setPos(center);
    qDebug()<<"carRoundCircle created";
}

QRectF carRoundCircle::boundingRect() const
{
    return QRectF( -25, -25, 50, 50 );
}


void carRoundCircle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::green);
    painter->setBrush(Qt::NoBrush);

    for ( int i = 0; i < carRound.size(); ++ i )
    {
        painter->drawPoint(carRound[i] - center);
    }
}

graphShow::graphShow(QPainterPath* path)
    :path(path)
{

}

QRectF graphShow::boundingRect() const
{
    return QRectF( 0, 0, 1000, 1000 );
}

void graphShow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(QColor(20,20,180,100)));

    painter->drawPath(*path);
}
