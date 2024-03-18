#ifndef STARTRECT_H
#define STARTRECT_H

#include <QGraphicsItem>
#include <QPainter>
#include <QDebug>


class startRectItem : public QGraphicsItem
{
public:
    startRectItem(QPointF p, qreal w, qreal h, qreal rotation);
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

private:
    QPointF center;
    qreal w, h, rotation;
};

#endif // STARTRECT_H
