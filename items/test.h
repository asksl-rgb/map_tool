#ifndef TEST_H
#define TEST_H

#include <QGraphicsItem>
#include <QPainter>
#include <QDebug>

class carRoundCircle : public QGraphicsItem
{
public:
    carRoundCircle(QPointF p,std::vector<QPoint> a);
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);
private:
    QPointF center;
    std::vector<QPoint> carRound;
};

class graphShow : public QGraphicsItem
{
public:
    graphShow(QPainterPath*);
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);
private:
    QPainterPath* path;
};

#endif // TEST_H
