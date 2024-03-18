#ifndef MAPSELECTAREA_H
#define MAPSELECTAREA_H

#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <vector>
#include <QCursor>
#include <QDebug>
#include <random>

//##################################//
//********        1        *********//
//##################################//
class MPointItem : public QObject, public QAbstractGraphicsShapeItem
{
public:
    MPointItem(QAbstractGraphicsShapeItem* parent, QPointF p);

    QPointF getPoint() { return m_point; }
    void setPoint(QPointF p) { m_point = p; }

protected:

    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;

    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override; // 正确，现代 C++ 推荐的方式
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override; // virtual 关键字在这里是多余的

private:
    QPointF m_point;
    QPen m_pen_isSelected;
    QPen m_pen_noSelected;

    double m_scaleFactor = 1.0;
};

//##################################//
//********        2        *********//
//##################################//
class MapSelectItem : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT

public:
    MapSelectItem();
    ~MapSelectItem();
    QList<QPointF> getPolygonsStart();
    void setText(QString);
    QString getText();

    std::vector<std::vector<MPointItem*> > polygons;
    std::vector<MPointItem*> *curpolygon = new std::vector<MPointItem*>;

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

public slots:
    void cur_ploygon_addPoint(QPointF,bool);

private:
//    QList<QGraphicsPolygonItem*> polygons;
//    QList<QPolygonF*> polygons;
//    QPolygonF *curpolygon = new QPolygonF;

    double m_scaleFactor = 1.0;
    QString areaText = "";
};

//##################################//
//********        3        *********//
//##################################//
class smoothedCoverPath : public QGraphicsItem
{
public:
    smoothedCoverPath(std::vector<std::vector<QPointF>>&);

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    QPainterPath *path = NULL;
//    std::mt19937 gen;  // 随机数生成器对象
//    std::uniform_int_distribution<int> dis;  // 随机数分布对象
};

//##################################//
//********        4        *********//
//##################################//
class BPolygonItem : public QGraphicsItem
{
public:
    BPolygonItem(QPolygonF&);

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    QPolygonF *polygon = nullptr;
};
//##################################//
//********        5        *********//
//##################################//
class test : public QGraphicsItem
{
public:
    test(QPointF center, int out_radius, int inner_radius);
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

private:
    int out_radius;
    int inner_radius;
};

////##################################//
////********        3        *********//
////##################################//
//class MapSelectItem : public QGraphicsItem
//{
//public:
//    MapSelectItem(QPointF p);
//    virtual QRectF boundingRect() const;
//    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

//protected:
//    void mousePressEvent(QGraphicsSceneMouseEvent *event);
//    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
//    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
//private:
//    QRectF m_leftTopRect;
//    QRectF m_leftBottomRect;
//    QRectF m_rightTopRect;
//    QRectF m_rightBottomRect;
//    QRectF m_baseRectF;
//    bool m_mousePressed = false;
//    bool m_lfTopSelectFg = false;
//    bool m_lfBottomSelectFg = false;
//    bool m_rtTopSelectFg = false;
//    bool m_rtBottomSelectFg = false;
//    int m_selectAreaSize = 0;
//    QPointF m_lastPointF;
//    double m_scaleFactor = 0.0;
//    int m_w = 0;
//    int m_h = 0;
//};

////##################################//
////********        4        *********//
////##################################//
//class MapSelectArea : public QGraphicsRectItem
//{
//public:
//    MapSelectArea(QGraphicsScene *scene, QObject *parent);

//public:
//    void setPos(QPointF p);
//    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

//    void mousePressEvent(QGraphicsSceneMouseEvent *event);
//    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
//    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
//private:
//    double m_w;
//    double m_h;
//    QRectF m_baseRectF;
//    QRectF m_leftTopRect;
//    QRectF m_leftBottomRect;
//    QRectF m_rightTopRect;
//    QRectF m_rightBottomRect;
//    int m_selectAreaSize = 0;
//    double m_scaleFactor = 0.0;

//    bool m_mousePressed = false;
//    bool m_lfTopSelectFg = false;
//    bool m_lfBottomSelectFg = false;
//    bool m_rtTopSelectFg = false;
//    bool m_rtBottomSelectFg = false;
//    QPointF m_lastPointF;
//};

#endif // MAPSELECTAREA_H
