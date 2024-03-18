#ifndef GRAPHICSTYPE_H
#define GRAPHICSTYPE_H

#include <QObject>
#include <QAbstractGraphicsShapeItem>
#include <QGraphicsWidget>
#include <QCursor>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QMetaObject>
#include <QMenu>
#include <QSpinBox>
#include <QWidgetAction>
#include <QLabel>
#include <QKeyEvent>
#include <QKeySequence>
#include <cmath>

#include <QDebug>
#include <QMessageBox>
#include <vector>
#include <map>

#include "../dialog/attributeAddDialog.h"

//struct attribute{
//    int welt;
//    int bypass;
//    int seismic;
//    int turnAround;
//    int trash;
//    int charge;
//    std::map<QString, QString> self_define;
//    attribute(int *a) :
//        welt(a[0]), bypass(a[1]), seismic(a[2]), turnAround(a[3]), trash(a[4]), charge(a[5]) {}
//    bool operator==(const attribute& b) const {
//        if (welt == b.welt &&
//            bypass == b.bypass &&
//            seismic == b.seismic &&
//            turnAround == b.turnAround &&
//            trash == b.trash &&
//            charge == b.charge &&
//            self_define == b.self_define )
//            return true;
//        else
//            return false;
//    }
//};//define in "../dialog/attributeAddDialog.h"
inline int seed_num[8] = {3300, 1700, 25625, 900, 52345, 27648, 13888, 15488};
inline int get_color_seed(int arr[8])
{
    int color_seed = 0;
    for(int i = 0; i < 8; i ++ )
    {
        color_seed += arr[i] * seed_num[i];
        color_seed %= 65536;
    }
    return color_seed;
}
//##################################//
//********        1        *********//
//##################################//
class BPointItem : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT

public:
//    enum PointType {
//        Center = 0, // 中心点
//        Edge,       // 边缘点（可拖动改变图形的形状、大小）
//        Special     // 特殊功能点
//    };

//    BPointItem(QAbstractGraphicsShapeItem* parent, QPointF p, PointType type);
    attribute *attr = nullptr;
    int color_seed = 481;   //1,30,225
    BPointItem(QAbstractGraphicsShapeItem* parent, QPointF p);
    ~BPointItem();
    void colorChange();
    void initAttr(int*,std::map<QString, QString>);
    void getAttr(int*,std::map<QString, QString>&);

    QPointF getPoint() { return m_point; }
    void setPoint(QPointF p) { m_point = p; }
    void setX(qreal x) { m_point.setX(x); }
    void setY(qreal y) { m_point.setY(y); }

    inline QPointF point() const { return m_point; }
    inline QPointF& rpoint() { return m_point; }

protected:

    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;

    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

signals:
    void del_Point(BPointItem*);
    void add_Next_Point(BPointItem*,qreal);

public slots:
    void set_Point_Attribute(BPointItem*);
    void getAttributeData(std::string);

private:
    QPointF m_point;
//    PointType m_type;
    QPen m_pen_isSelected;
    QPen m_pen_noSelected;
};

//##################################//
//********        2        *********//
//##################################//
class BPathItem : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT

public:
    inline BPointItem* getStart() { if(pList.empty()) return nullptr; else return pList[0]; }
//    void setStart(QPointF p) { p_start = p; }

    inline int pointsNumber() { return p_count; }
    inline int getSequence( BPointItem* point ) { return find(pList.begin(), pList.end(), point) - pList.begin(); }
    inline QPointF getPoint( int index ) { return pList[index]->getPoint(); }
    inline BPointItem* getElement( int index ) { return pList[index]; }

    void addNextPoint(QPointF point);
    void addNextPoint(QPointF, int*, std::map<QString, QString>);
    void setPoint(BPointItem* point, QPointF p);
//    void print_pList();
    void clear();
    void drawPath();
    void setPathWidth(qreal w);
    void move(QPointF);
    void trans(qreal);

    BPathItem();
    ~BPathItem();

protected:

//    virtual void focusInEvent(QFocusEvent *event) override;
//    virtual void focusOutEvent(QFocusEvent *event) override;

    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

public slots:
    void pushPoint(QPointF p, QList<QPointF> list, bool isFinished);
    void deletePoint(BPointItem* point);
    void insertPoint(BPointItem* point,qreal);

protected:
//    QPointF p_start;
//    QPointF m_edge;
    int p_count;
    std::vector<BPointItem*> pList;

//    QPen m_pen_isSelected;
//    QPen m_pen_noSelected;
    QPen m_pen;
};

//##################################//
//********        3        *********//
//##################################//
class BQGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    BQGraphicsScene(QObject *parent = nullptr);

    void startCreate();
    void drawLine2img();
    void create_mapSelect_polygon();
    void drawWhitePolygon();
    void create_pointSelect_polygon();
    void create_areaSelect_polygon();
    QGraphicsScene* toQGraphicsScene();
    void select_startPoint();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void updatePoint(QPointF p, QList<QPointF> list, bool isFinished);
    void createFinished();
    void drawLine( QList<QPointF> );
    void map_ploygon_addPoint(QPointF p,bool isFinished);
    void createFinished_mp();
    void curMousePos(QPointF);
    void erase_ploygon_addPoint(QPointF p);
    void createFinished_ep(QList<QPointF>);
    void pointSelect_polygon_addPoint(QPointF p,bool isFinished);
    void createFinished_pp();
    void areaSelect_polygon_addPoint(QPointF p, bool isFinished);
    void createFinished_ap();
    void selected_startPoint_accept(QPointF);
    void selected_startPoint_reject();

protected:
    QList<QPointF> m_list;
    bool is_creating_path;
    bool is_creating_virtual_wall;
    bool is_creating_mapSelect_ploygon;
    bool is_creating_erase_polygon;
    bool is_creating_pointSelect_polygon;
    bool is_creating_areaSelect_polygon;
    bool is_selecting_startPoint;
};


void delete_point(BPathItem*,BPointItem*);

#endif // GRAPHICSTYPE_H
