#include "erasePolygon.h"

erasePolygon::erasePolygon()
{
    polygon = new QPolygonF;
}
QRectF erasePolygon::boundingRect() const
{
    return polygon->boundingRect();
}

void erasePolygon::pushPoint(QPointF p)
{
    *polygon << p;
    this->update();
}
void erasePolygon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen pen;
    pen.setColor(Qt::magenta);
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);
    painter->drawPolygon(*polygon);
}
