#ifndef ERASEPOLYGON_H
#define ERASEPOLYGON_H

#include <QGraphicsItem>
#include <QPainter>

class erasePolygon : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    erasePolygon();

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

public slots:
    void pushPoint(QPointF);

private:
    QPolygonF *polygon = NULL;
};

#endif // ERASEPOLYGON_H
