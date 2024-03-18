#include "startRect.h"

startRectItem::startRectItem(QPointF p, qreal w, qreal h, qreal rotation)
{
    this->center = p;
    this->w = w;
    this->h = h;
    this->rotation = rotation;
    this->setPos(center);
}

QRectF startRectItem::boundingRect() const
{
//    return QRectF(center,center);
    return QRectF( - h / 2 - 2, - w / 2 - 2, h + 4, w + 4 );
}

void startRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::red);
    painter->setBrush(Qt::NoBrush);
//    this->setPos(center);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->rotate(rotation);

    painter->drawRect(QRectF(-h/2, -w/2, h, w));
    painter->drawPoint(0,0);
}
