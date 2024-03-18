#include "mapSelectItem.h"
#include <QGraphicsScene>
#include <QDebug>
#include <QPainter>
#define SelectAreaSize 30

//##################################//
//********        1        *********//
//##################################//
MPointItem::MPointItem(QAbstractGraphicsShapeItem* parent, QPointF p)
    : QAbstractGraphicsShapeItem(parent)
    , m_point(p)
{
    m_pen_noSelected.setColor(QColor(0, 0, 160, 230));
    m_pen_noSelected.setWidth(1);
    m_pen_isSelected.setColor(QColor(0, 255, 0, 255));
    m_pen_isSelected.setWidth(1);
    this->setPos(m_point);
    this->setPen(m_pen_noSelected);
    this->setFlags(QGraphicsItem::ItemIsSelectable |
                   QGraphicsItem::ItemIsMovable |
                   QGraphicsItem::ItemIsFocusable);

    this->setCursor(Qt::PointingHandCursor);
}

void MPointItem::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    this->setPen(m_pen_isSelected);
    qDebug()<<"MPointItem"<<m_point<<"selected";
}
void MPointItem::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    this->setPen(m_pen_noSelected);
    qDebug()<<"MPointItem"<<m_point<<"not selected";
}
QRectF MPointItem::boundingRect() const
{
    return QRect(-4, -4, 8, 8);
}

void MPointItem::paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    m_scaleFactor = painter->matrix().m11();

    painter->setPen(this->pen());
    painter->setBrush(this->brush());
    this->setPos(m_point);

    painter->drawRect(-3 / m_scaleFactor - 3, -3 / m_scaleFactor - 3, 6 / m_scaleFactor + 6, 6 / m_scaleFactor + 6);
    painter->drawPoint(0, 0);
}

void MPointItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug()<<" MPointItem::mousePressEvent";
    qreal currentZ = zValue();
    setData(0, QVariant(currentZ)); // 保存当前Z值
    setZValue(1000); // 临时设置一个较高的Z值
    QAbstractGraphicsShapeItem::mousePressEvent(event); // 调用基类的事件处理
}

void MPointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug()<<" MPointItem::mouseReleaseEvent";
    qreal originalZ = data(0).toReal();
    setZValue(originalZ); // 恢复原始Z值
    QAbstractGraphicsShapeItem::mouseReleaseEvent(event); // 调用基类的事件处理
}

void MPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if ( event->buttons() == Qt::LeftButton )
    {
        QPointF pos(event->scenePos().x(), event->scenePos().y());
        this->setPoint(pos);
        this->scene()->update();
        qDebug()<<"MPointItem::mouseMoveEvent"<<pos;
    }
    qDebug()<<"MPointItem::mouseMoveEvent out";
    QAbstractGraphicsShapeItem::mouseMoveEvent(event);
}

//##################################//
//********        2        *********//
//##################################//
MapSelectItem::MapSelectItem()
{
//    this->setFlags(QGraphicsItem::ItemIsSelectable |  //为什么注释这三行代码可参考 BPathItem::BPathItem() 的注释
//                   QGraphicsItem::ItemIsMovable |
//                   QGraphicsItem::ItemIsFocusable);
    qDebug()<<(void*)this<<"MapSelectItem created";
}


MapSelectItem::~MapSelectItem()
{
    qDebug()<<(void*)this<<"mapSelectedItem deleted;";
}

QList<QPointF> MapSelectItem::getPolygonsStart()
{
    QList<QPointF> ans;
    for ( auto& it : polygons )
        if ( !it.empty() )
            ans.push_back( it[0]->getPoint() );

    return ans;
}

void MapSelectItem::setText(QString str)
{
    areaText = str;
    this->update();
}

QString MapSelectItem::getText()
{
    return areaText;
}

//MapSelectItem::~MapSelectItem(){}

QRectF MapSelectItem::boundingRect() const
{
    return childrenBoundingRect();
}

void MapSelectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::blue);
    painter->setBrush(Qt::NoBrush);

    QPainterPath path;

    for ( size_t i = 0; i < polygons.size(); i ++ )
    {
        QPolygonF tmp;
        for ( size_t j = 0; j < polygons[i].size(); j ++ )
        {
            tmp << polygons[i][j]->getPoint();
        }
        painter->drawPolygon(tmp);
        path.addPolygon(tmp);
    }

    painter->setPen(Qt::green);
    QPolygonF tmp;
    for ( size_t i = 0; i < curpolygon->size(); i ++ )
    {
        tmp << (*curpolygon)[i]->getPoint();
    }
    painter->drawPolygon(tmp);
    path.addPolygon(tmp);

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(255,140,0,45));

    painter->drawPath(path);

    m_scaleFactor = painter->matrix().m11();
    QPen textPen;
    textPen.setColor(QColor(225, 75, 25, 240));
//    textPen.setWidth(1);
    painter->setPen(textPen);
    QFont font;
    font.setPointSize(16 + 4 / m_scaleFactor);
    painter->setFont(font);
    painter->setBrush(Qt::NoBrush);
    for ( auto& it : getPolygonsStart() )
    {
        painter->drawText(it - QPointF(0, 10), areaText);
    }
}

void MapSelectItem::cur_ploygon_addPoint(QPointF p, bool isFinished)
{
    qDebug()<<"1:"<<p<<' '<<isFinished;
    if(isFinished)
    {
        polygons.push_back(*curpolygon);
        curpolygon = new std::vector<MPointItem*>;
    }
    else
    {
        curpolygon->push_back(new MPointItem(this, p));
        qDebug()<<"--->接到左键："<<(*curpolygon).size();
    }
    this->scene()->update();
}

//##################################//
//********        3        *********//
//##################################//
smoothedCoverPath::smoothedCoverPath(std::vector<std::vector<QPointF>>& contours2d)
{
    path = new QPainterPath;
    for ( size_t i = 0; i < contours2d.size(); ++ i )
    {
        QPolygonF polygon;
        for ( size_t j = 0; j < contours2d[i].size(); ++ j )
        {
            polygon << contours2d[i][j];
        }
        path->addPolygon(polygon);
    }
//    // 创建一个随机数生成器对象
//    std::random_device rd;
//    gen = std::mt19937(rd());

//    // 定义随机数分布范围
//    int min = 0;
//    int max = 255;

//    // 创建一个均匀分布的随机数分布对象
//    dis = std::uniform_int_distribution<int>(min, max);
}

QRectF smoothedCoverPath::boundingRect() const
{
    return path->boundingRect();
}
void smoothedCoverPath::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(Qt::NoPen);
//    painter->setBrush(QColor(dis(gen),dis(gen),dis(gen),75));
    painter->setBrush(QColor(0, 160, 80, 40));

    painter->drawPath(*path);
}

//##################################//
//********        4        *********//
//##################################//
BPolygonItem::BPolygonItem(QPolygonF& po) :
    polygon(new QPolygonF(po))
{}
QRectF BPolygonItem::boundingRect() const
{
    return polygon->boundingRect();
}
void BPolygonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(160,50,50,40));

    painter->drawPolygon(*polygon);

}

//##################################//
//********        5        *********//
//##################################//
test::test(QPointF center, int out_radius, int inner_radius)
{
    this->inner_radius = inner_radius;
    this->out_radius = out_radius;
    this->setPos(center);
}
QRectF test::boundingRect() const
{
    return QRectF(-out_radius, -out_radius, 2*out_radius, 2*out_radius);
}
void test::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QPainterPath *path = new QPainterPath();
    path->addEllipse(QRectF(-out_radius, -out_radius, 2*out_radius, 2*out_radius));
    path->addEllipse(QRectF(-out_radius*0.75, -out_radius*0.75, 2*out_radius*0.75, 2*out_radius*0.75));
    path->addEllipse(QRectF(-inner_radius, -inner_radius, 2*inner_radius, 2*inner_radius));

    painter->setPen(Qt::black);
    painter->setBrush(Qt::white);
    painter->drawPath(*path);
}

////##################################//
////********        3        *********//
////##################################//
//MapSelectItem::MapSelectItem(QPointF p)
//{
//    this->setFlag(QGraphicsItem::ItemIsMovable, true);
//    //    this->setAcceptHoverEvents(true);

//    m_baseRectF = QRectF(p.x(),p.y(),100,100);
//    m_w = 100;
//    m_h = 100;
//}

//QRectF MapSelectItem::boundingRect() const
//{
//    return m_baseRectF;
//}

//void MapSelectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
//{
//    painter->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);

//    m_scaleFactor = painter->matrix().m11();
//    m_selectAreaSize = SelectAreaSize/m_scaleFactor;
//    QPen pen = painter->pen();
//    pen.setWidthF(pen.widthF() * 2 / m_scaleFactor);  //等比例设置线宽，当前设置为2倍线宽

//    m_baseRectF.setWidth(m_w/m_scaleFactor);
//    m_baseRectF.setHeight(m_h/m_scaleFactor);

//    //背景框
//    painter->setPen(Qt::NoPen);
//    painter->setBrush(QBrush(QColor("#20ff0000")));
//    painter->drawRect(m_leftTopRect.center().x(),m_rightTopRect.center().y(),m_rightBottomRect.center().x() - m_leftTopRect.center().x(),
//                      m_rightBottomRect.center().y() - m_leftTopRect.center().y());

//    pen.setColor(QColor("#ff0000"));
//    painter->setPen(pen);
//    painter->setBrush(QBrush(QColor("#ff0000")));

//    m_leftTopRect = QRectF(m_baseRectF.left(),m_baseRectF.top(),m_selectAreaSize,m_selectAreaSize);
//    m_leftBottomRect = QRectF(m_baseRectF.left(),m_baseRectF.bottom() - m_selectAreaSize,m_selectAreaSize,m_selectAreaSize);
//    m_rightTopRect = QRectF(m_baseRectF.right() - m_selectAreaSize,m_baseRectF.top(),m_selectAreaSize,m_selectAreaSize);
//    m_rightBottomRect = QRectF(m_baseRectF.right() - m_selectAreaSize,m_baseRectF.bottom() - m_selectAreaSize,m_selectAreaSize,m_selectAreaSize);
//    painter->drawEllipse(m_leftTopRect);
//    painter->drawEllipse(m_leftBottomRect);
//    painter->drawEllipse(m_rightTopRect);
//    painter->drawEllipse(m_rightBottomRect);

//    pen.setStyle(Qt::DashLine);
//    painter->setPen(pen);
//    painter->drawLine(m_leftTopRect.center(),m_leftBottomRect.center());
//    painter->drawLine(m_leftTopRect.center(),m_rightTopRect.center());
//    painter->drawLine(m_rightTopRect.center(),m_rightBottomRect.center());
//    painter->drawLine(m_leftBottomRect.center(),m_rightBottomRect.center());

//}

//void MapSelectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
//{
//    QGraphicsItem::mousePressEvent(event);
//    if (m_leftTopRect.contains(event->scenePos()))
//        m_lfTopSelectFg = true;
//    else if (m_leftBottomRect.contains(event->scenePos()))
//        m_lfBottomSelectFg = true;
//    else if (m_rightTopRect.contains(event->scenePos()))
//        m_rtTopSelectFg = true;
//    else if (m_rightBottomRect.contains(event->scenePos()))
//        m_rtBottomSelectFg = true;
//    else
//        m_mousePressed = true;
//    m_lastPointF = event->scenePos();
//}

//void MapSelectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
//{
//    QGraphicsItem::mouseReleaseEvent(event);
//    m_mousePressed = false;
//    m_lfTopSelectFg = false;
//    m_lfBottomSelectFg = false;
//    m_rtTopSelectFg = false;
//    m_rtBottomSelectFg = false;
//}

///*add in 220622 */
//void MapSelectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
//{
//    QPointF pos = event->scenePos();
//    if (m_mousePressed) {
//        m_baseRectF = QRectF(m_baseRectF.x() + pos.x() - m_lastPointF.x(),m_baseRectF.y() + pos.y() - m_lastPointF.y()
//                             ,m_baseRectF.width(),m_baseRectF.height());
//    }
//    else if (m_lfTopSelectFg)
//    {
//        double x = m_baseRectF.x();
//        double y = m_baseRectF.y();
//        m_baseRectF.setX(x + pos.x() - x);
//        m_baseRectF.setY(y + pos.y() - y);
//        if (m_baseRectF.width() <= m_selectAreaSize*2)
//            m_baseRectF.setX(x);
//        if (m_baseRectF.height() <= m_selectAreaSize*2)
//            m_baseRectF.setY(y);
//    }
//    else if (m_lfBottomSelectFg)
//    {
//        double x = m_baseRectF.x();
//        double h = m_baseRectF.height();
//        m_baseRectF.setX(m_baseRectF.x() + pos.x() - m_lastPointF.x());
//        m_baseRectF.setHeight(m_baseRectF.height() + pos.y() - m_lastPointF.y());
//        if (m_baseRectF.width() <= m_selectAreaSize*2)
//            m_baseRectF.setX(x);
//        if (m_baseRectF.height() <= m_selectAreaSize*2)
//            m_baseRectF.setHeight(h);
//    }
//    else if (m_rtTopSelectFg)
//    {
//        double y = m_baseRectF.y();
//        double w = m_baseRectF.width();
//        m_baseRectF.setY(m_baseRectF.y() + pos.y() - m_lastPointF.y());
//        m_baseRectF.setWidth(m_baseRectF.width() + pos.x() - m_lastPointF.x());
//        if (m_baseRectF.width() <= m_selectAreaSize*2)
//            m_baseRectF.setWidth(w);
//        if (m_baseRectF.height() <= m_selectAreaSize*2)
//            m_baseRectF.setY(y);
//    }
//    else if (m_rtBottomSelectFg)
//    {
//        double w = m_baseRectF.width();
//        double h = m_baseRectF.height();
//        m_baseRectF.setWidth(m_baseRectF.width() + pos.x() - m_lastPointF.x());
//        m_baseRectF.setHeight(m_baseRectF.height() + pos.y() - m_lastPointF.y());
//        if (m_baseRectF.width() <= m_selectAreaSize*2)
//            m_baseRectF.setWidth(w);
//        if (m_baseRectF.height() <= m_selectAreaSize*2)
//            m_baseRectF.setHeight(h);
//    }
//    m_w = m_baseRectF.width()*m_scaleFactor;
//    m_h = m_baseRectF.height()*m_scaleFactor;
//    m_lastPointF = event->scenePos();
//    this->scene()->update();
//}

////##################################//
////********        4        *********//
////##################################//
//MapSelectArea::MapSelectArea(QGraphicsScene *scene, QObject *parent)
//{
//    scene->addItem(this);
//    this->setFlag(QGraphicsItem::ItemIsMovable, true);
//    this->setAcceptHoverEvents(true);
//    this->setBrush(Qt::red);

//}

//void MapSelectArea::setPos(QPointF p)
//{
//    this->setRect(p.x(),p.y(),100,100);
//    m_w = 100;
//    m_h = 100;
//}

//void MapSelectArea::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
//{
//    painter->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);

//    QPen pen;
//    m_scaleFactor = painter->matrix().m11();
//    m_selectAreaSize = SelectAreaSize/m_scaleFactor;
//    m_baseRectF = rect();
//    this->setRect(m_baseRectF.x(),m_baseRectF.y(),m_w/m_scaleFactor,m_h/m_scaleFactor);
//    pen.setWidthF(pen.widthF() * 2 / m_scaleFactor);  //等比例设置线宽，当前设置为2倍线宽


//    //背景框
//    painter->setPen(Qt::NoPen);
//    painter->setBrush(QBrush(QColor("#2048CBD4")));
//    painter->drawRect(m_leftTopRect.center().x(),m_rightTopRect.center().y(),m_rightBottomRect.center().x() - m_leftTopRect.center().x(),
//                      m_rightBottomRect.center().y() - m_leftTopRect.center().y());

//    pen.setColor(QColor("#48CBD4"));
//    painter->setPen(pen);
//    painter->setBrush(QBrush(QColor("#48CBD4")));

//    m_leftTopRect = QRectF(m_baseRectF.left(),m_baseRectF.top(),m_selectAreaSize,m_selectAreaSize);
//    m_leftBottomRect = QRectF(m_baseRectF.left(),m_baseRectF.bottom() - m_selectAreaSize,m_selectAreaSize,m_selectAreaSize);
//    m_rightTopRect = QRectF(m_baseRectF.right() - m_selectAreaSize,m_baseRectF.top(),m_selectAreaSize,m_selectAreaSize);
//    m_rightBottomRect = QRectF(m_baseRectF.right() - m_selectAreaSize,m_baseRectF.bottom() - m_selectAreaSize,m_selectAreaSize,m_selectAreaSize);
//    painter->drawEllipse(m_leftTopRect);
//    painter->drawEllipse(m_leftBottomRect);
//    painter->drawEllipse(m_rightTopRect);
//    painter->drawEllipse(m_rightBottomRect);

//    pen.setStyle(Qt::DashLine);
//    painter->setPen(pen);
//    painter->drawLine(m_leftTopRect.center(),m_leftBottomRect.center());
//    painter->drawLine(m_leftTopRect.center(),m_rightTopRect.center());
//    painter->drawLine(m_rightTopRect.center(),m_rightBottomRect.center());
//    painter->drawLine(m_leftBottomRect.center(),m_rightBottomRect.center());
//}

//void MapSelectArea::mousePressEvent(QGraphicsSceneMouseEvent *event)
//{
//    QGraphicsItem::mousePressEvent(event);
//    if (m_leftTopRect.contains(event->scenePos()))
//        m_lfTopSelectFg = true;
//    else if (m_leftBottomRect.contains(event->scenePos()))
//        m_lfBottomSelectFg = true;
//    else if (m_rightTopRect.contains(event->scenePos()))
//        m_rtTopSelectFg = true;
//    else if (m_rightBottomRect.contains(event->scenePos()))
//        m_rtBottomSelectFg = true;
//    else
//        m_mousePressed = true;
//    m_lastPointF = event->scenePos();
//}

//void MapSelectArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
//{
//    QGraphicsItem::mouseReleaseEvent(event);
//    m_mousePressed = false;
//    m_lfTopSelectFg = false;
//    m_lfBottomSelectFg = false;
//    m_rtTopSelectFg = false;
//    m_rtBottomSelectFg = false;
//}

///*add in 220622 */
//void MapSelectArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
//{
//    QPointF pos = event->scenePos();
//    if (m_mousePressed) {
//        m_baseRectF = QRectF(m_baseRectF.x() + pos.x() - m_lastPointF.x(),m_baseRectF.y() + pos.y() - m_lastPointF.y()
//                             ,m_baseRectF.width(),m_baseRectF.height());
//    }
//    else if (m_lfTopSelectFg)
//    {
//        double x = m_baseRectF.x();
//        double y = m_baseRectF.y();
//        m_baseRectF.setX(x + pos.x() - x);
//        m_baseRectF.setY(y + pos.y() - y);
//        if (m_baseRectF.width() <= m_selectAreaSize*2)
//            m_baseRectF.setX(x);
//        if (m_baseRectF.height() <= m_selectAreaSize*2)
//            m_baseRectF.setY(y);
//    }
//    else if (m_lfBottomSelectFg)
//    {
//        double x = m_baseRectF.x();
//        double h = m_baseRectF.height();
//        m_baseRectF.setX(m_baseRectF.x() + pos.x() - m_lastPointF.x());
//        m_baseRectF.setHeight(m_baseRectF.height() + pos.y() - m_lastPointF.y());
//        if (m_baseRectF.width() <= m_selectAreaSize*2)
//            m_baseRectF.setX(x);
//        if (m_baseRectF.height() <= m_selectAreaSize*2)
//            m_baseRectF.setHeight(h);
//    }
//    else if (m_rtTopSelectFg)
//    {
//        double y = m_baseRectF.y();
//        double w = m_baseRectF.width();
//        m_baseRectF.setY(m_baseRectF.y() + pos.y() - m_lastPointF.y());
//        m_baseRectF.setWidth(m_baseRectF.width() + pos.x() - m_lastPointF.x());
//        if (m_baseRectF.width() <= m_selectAreaSize*2)
//            m_baseRectF.setWidth(w);
//        if (m_baseRectF.height() <= m_selectAreaSize*2)
//            m_baseRectF.setY(y);
//    }
//    else if (m_rtBottomSelectFg)
//    {
//        double w = m_baseRectF.width();
//        double h = m_baseRectF.height();
//        m_baseRectF.setWidth(m_baseRectF.width() + pos.x() - m_lastPointF.x());
//        m_baseRectF.setHeight(m_baseRectF.height() + pos.y() - m_lastPointF.y());
//        if (m_baseRectF.width() <= m_selectAreaSize*2)
//            m_baseRectF.setWidth(w);
//        if (m_baseRectF.height() <= m_selectAreaSize*2)
//            m_baseRectF.setHeight(h);
//    }
//    m_w = m_baseRectF.width()*m_scaleFactor;
//    m_h = m_baseRectF.height()*m_scaleFactor;
//    this->setRect(m_baseRectF.x(),m_baseRectF.y(),m_w/m_scaleFactor,m_h/m_scaleFactor);
//    m_lastPointF = event->scenePos();
//    this->scene()->update();
//}

