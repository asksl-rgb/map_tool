#include "GraphicsType.h"

//##################################//
//********        1        *********//
//##################################//
BPointItem::BPointItem(QAbstractGraphicsShapeItem* parent, QPointF p)
    : QAbstractGraphicsShapeItem(parent)
    , m_point(p)
{
    qDebug()<<"Point created"<<p;
    m_pen_noSelected.setColor(QColor(1, 30, 225, 230));
    m_pen_noSelected.setWidth(1);
    m_pen_isSelected.setColor(QColor(1, 255, 0, 255));
    m_pen_isSelected.setWidth(1);
    this->setPos(m_point);
    this->setPen(m_pen_noSelected);
    this->setFlags(QGraphicsItem::ItemIsSelectable |
                   QGraphicsItem::ItemIsMovable |
                   QGraphicsItem::ItemIsFocusable);

    this->setCursor(Qt::PointingHandCursor);

}

BPointItem::~BPointItem()
{
    qDebug()<<"Point destructed"<<this->getPoint();
//    this->scene()->clear();
//    delete this;
}

void BPointItem::colorChange()
{
    color_seed = color_seed ? color_seed : 481;
    color_seed %= 65536;
    int a = color_seed/256, b = color_seed/16%256, c = color_seed%256;
    qDebug()<<"pointColor:"<<m_pen_noSelected<<"colorSeed:"<<color_seed<<a<<b<<c;
    double uni = 256.0/(a + b + c);
    a = (int)(a*uni)%256;
    b = (int)(b*uni)%256;
    c = (int)(c*uni)%256;
    m_pen_noSelected.setColor(QColor(a, b, c, 230));
    this->setPen(m_pen_noSelected);
    qDebug()<<"pointColor:"<<m_pen_noSelected<<"colorSeed:"<<color_seed<<a<<b<<c<<uni;
}

void BPointItem::initAttr(int* arr,std::map<QString, QString> self_df)
{
    this->attr = new attribute(arr);
    this->attr->self_define = self_df;
    this->color_seed = get_color_seed(arr);
    this->colorChange();
}
void BPointItem::getAttr(int* arr,std::map<QString, QString>& self_df)
{
    arr[0]=attr->welt;
    arr[1]=attr->bypass;
    arr[2]=attr->seismic;
    arr[3]=attr->turnAround;
    arr[4]=attr->trash;
    arr[5]=attr->charge;
    arr[6]=attr->back;
    arr[7]=attr->clean_flag;
    self_df = attr->self_define;
}
void BPointItem::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    this->setPen(m_pen_isSelected);
    qDebug()<<"BPointItem"<<m_point<<"selected";
}

void BPointItem::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    this->setPen(m_pen_noSelected);
    qDebug()<<"BPointItem"<<m_point<<"not selected";
}

QRectF BPointItem::boundingRect() const
{
    return QRect(-4, -4, 8, 8);
}

void BPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(this->pen());
    painter->setBrush(this->brush());
    this->setPos(m_point);

//    painter->drawRect(QRectF(-4, -4, 8, 8));
    painter->drawEllipse(-3, -3, 6, 6);
//    painter->drawRect(QRectF(0,0,0.01,0.01));
    painter->drawPoint(0, 0);
}

void BPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if ( event->buttons() == Qt::LeftButton )
    {
        QPointF pos(event->scenePos().x(), event->scenePos().y());
        this->setPoint(pos);
        this->scene()->update();
    }
    QAbstractGraphicsShapeItem::mouseMoveEvent(event);
}
void BPointItem::keyPressEvent(QKeyEvent *event)
{
    qDebug()<<event->key();
    if( 16777223 == event->key())
    {
        emit del_Point(this);
    }
}
void BPointItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if ( !this->isSelected() )
    {
        return;
    }

    QMenu* menu = new QMenu();
//    menu->setStyleSheet("QMenu { background-color:rgb(254,254,254); border: 1px solid rgb(235,110,36); }");
    menu->setStyleSheet(
            " QMenu {\
                background-color: white; /* sets background of the menu 设置整个菜单区域的背景色，我用的是白色：white*/\
                border: 1px solid rgb(235,110,36);/*整个菜单区域的边框粗细、样式、颜色*/\
            }\
            QMenu::item {\
                /* sets background of menu item. set this to something non-transparent\
                    if you want menu color and menu item color to be different */\
                background-color: transparent;\
                padding:8px 32px;/*设置菜单项文字上下和左右的内边距，效果就是菜单中的条目左右上下有了间隔*/\
                margin:8px 8px;/*设置菜单项的外边距*/\
                border-bottom:1px solid #DBDBDB;/*为菜单项之间添加横线间隔*/\
            }\
            QMenu::item:selected { /* when user selects item using mouse or keyboard */\
                background-color: #2dabf9;/*这一句是设置菜单项鼠标经过选中的样式*/\
            }\
            QMenu::item:pressed {/*菜单项按下效果*/\
                border: 1px solid rgb(60,60,61); \
                background-color: #1d9be9; \
            }\
            QMenu::separator {\
                height:1px;\
                    background-color:rgba(123,123,123,1);\
                    margin-left:6px;\
                    margin-right:6px;\
            }\
            QMenu::item:!enabled {/* 不可用(操作)选项颜色灰色*/\
                color:rgb(128，136,145);\
                background-color:rgba(255,255,255,1);\
            }"
                );

    QDoubleSpinBox* x_spinBox = new QDoubleSpinBox(menu);
    x_spinBox->setStyleSheet("QDoubleSpinBox{ width:130px; height:30px;}");
//    x_spinBox->setRange(0, ui->pgmGraphicsView->width());
    x_spinBox->setRange(0, 100000);
    x_spinBox->setPrefix("x: ");
    x_spinBox->setSuffix(" px");
    x_spinBox->setSingleStep(0.2);
    x_spinBox->setValue(m_point.rx());
    connect(x_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double v){
        m_point.setX(v);
        this->hide();
        this->update();
        this->show();
    });
    QDoubleSpinBox* y_spinBox = new QDoubleSpinBox(menu);
    y_spinBox->setStyleSheet("QDoubleSpinBox{ width:130px; height:30px;}");
    y_spinBox->setRange(m_point.ry() - 100, m_point.ry() + 100);
    y_spinBox->setPrefix("y: ");
    y_spinBox->setSuffix(" px");
    y_spinBox->setSingleStep(0.2);
    y_spinBox->setValue(m_point.ry());
    connect(y_spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double v){
        m_point.setY(v);
        this->hide();
        this->update();
        this->show();
    });

    QWidgetAction* x_widgetAction = new QWidgetAction(menu);
    x_widgetAction->setDefaultWidget(x_spinBox);
    menu->addAction(x_widgetAction);

    QWidgetAction* y_widgetAction = new QWidgetAction(menu);
    y_widgetAction->setDefaultWidget(y_spinBox);
    menu->addAction(y_widgetAction);

    QLabel* explain_label = new QLabel(menu);
    explain_label->setText("  精调(±10px)");
    QWidgetAction* label_widgetAction = new QWidgetAction(menu);
    label_widgetAction->setDefaultWidget(explain_label);
    menu->addAction(label_widgetAction);

    menu->addSeparator();   //添加分割线

    if ( this->attr )
    {
        QLabel* attr_label = new QLabel(menu);
        attr_label->setWordWrap(true);

        attribute *attr = this->attr;
        QString str = "";
        switch (attr->welt) {
        case 0:
            str += "不贴边";
            break;
        case 1:
            str += "左贴边";
            break;
        case 2:
            str += "右贴边";
            break;
        }
        switch (attr->bypass) {
        case 0:
            str += ",避障";
            break;
        case 1:
            str += ",关闭避障";
            break;
        }
        if(attr->seismic)
            str += ",震尘点";
        if(attr->turnAround)
            str += ",掉头点";
        if(attr->trash)
            str += ",倒垃圾点";
        if(attr->charge)
            str += ",充电点";
        if(attr->back)
            str += ",倒车点";
        if(attr->clean_flag)
            str += ",清扫";
        else
            str += ",不清扫";
        if(attr->self_define.size() != 0)
        {
            for ( auto& it : attr->self_define )
            {
                str += ',' + it.first + ":" + it.second;
            }
        }

        attr_label->setText(str);
        QWidgetAction* attr_label_widgetAction = new QWidgetAction(menu);
        attr_label_widgetAction->setDefaultWidget(attr_label);
        menu->addAction(attr_label_widgetAction);

        menu->addSeparator();   //添加分割线
    }

    QAction* del_self = new QAction("删除此点", this);
    del_self->setShortcut(QKeySequence::Delete);
//    del_self->setStatusTip("删除此Point");
//    connect(del_self, &QAction::triggered, m_path, &BPathItem::deletePoint(this));
    connect(del_self, &QAction::triggered, this, [=](){ emit del_Point(this); });
    menu->addAction(del_self);

    QAction* add_next_one = new QAction("增加下一个点", this);
    add_next_one->setShortcut(tr("A"));
    connect(add_next_one, &QAction::triggered, this, [=](){ emit add_Next_Point(this, 10); });
    menu->addAction(add_next_one);

    QAction* set_attr = new QAction("设定属性", this);
    set_attr->setShortcut(tr("Q"));
    connect(set_attr, &QAction::triggered, this, [this]() { set_Point_Attribute(this); } );
    menu->addAction(set_attr);

    menu->exec(QCursor::pos());
    delete menu;

    QGraphicsItem::contextMenuEvent(event);
}

void BPointItem::set_Point_Attribute(BPointItem*)
{
    AttributeAddDialog *aad = new AttributeAddDialog(nullptr,this->attr);
    connect(aad, &AttributeAddDialog::sendData, this, &BPointItem::getAttributeData ); //这里直接用getAttributeData解决赋值问题
    aad->setAttribute(Qt::WA_QuitOnClose, false);
    aad->setAttribute(Qt::WA_DeleteOnClose);
    aad->show();
}
//slot_进行赋值
void BPointItem::getAttributeData(std::string str)
{
    int arr[8];
    std::map<QString, QString> self_df;
    QString key,val;
    bool iskey = true;
    for( uint i = 0; i < str.size(); ++ i )
    {
        if ( i < 8 )
            arr[i] = str[i] - '0';
        else
        {
            if(str[i] == ',')
            {
                if(iskey)
                    iskey=false;
                else
                {
                    self_df[key]=val;
                    key = "";
                    val = "";
                    iskey=true;
                }
                continue;
            }
            else if(iskey)
                key+=str[i];
            else
                val+=str[i];
        }
    }

    this->initAttr(arr,self_df);
}
//##################################//
//********        2        *********//
//##################################//
BPathItem::BPathItem()
    : p_count(0)
{

//    m_pen_noSelected.setColor(QColor(1, 0, 160, 230));
//    m_pen_noSelected.setWidth(2);
//    m_pen_isSelected.setColor(QColor(1, 255, 0, 255));
//    m_pen_isSelected.setWidth(2);
    m_pen.setColor(QColor(255, 0, 0, 100));

    this->setPen(m_pen);
//    this->setFlags(QGraphicsItem::ItemIsSelectable |  //这个不可轻易解注释，会导致下层的item无法选中
//                   QGraphicsItem::ItemIsMovable |
//                   QGraphicsItem::ItemIsFocusable);
/*
如果在 BPathItem 区域内的 MPointItem 无法响应 focusInEvent 和 mousePressEvent，而在区域外的相应正常，这确实暗示了可能是层叠顺序（即 Z 值）的问题。如果 BPathItem 的 Z 值高于 MPointItem，那么 BPathItem 将会拦截所有鼠标事件，从而阻止了对 MPointItem 的事件处理。
为了解决这个问题，你可以尝试以下几种方法：
1.确保 Z 值设置正确：在 MPointItem 被创建的时候，确保其 Z 值高于 BPathItem。如果 BPathItem 是 MPointItem 的父项，它的 Z 值可能默认比子项高。
2.检查事件传递：确保 BPathItem 没有重写 mousePressEvent（和/或 mouseMoveEvent、focusInEvent 等）并且停止事件传递。默认情况下，如果一个事件在 BPathItem 中被接受（即调用了 event->accept()），它将不会传递到 MPointItem。
3.调整事件传递逻辑：如果 BPathItem 重写了事件处理函数，你可以通过在 BPathItem 的事件处理函数中显式地调用 event->ignore() 来允许事件继续传递到 MPointItem。
4.使用 QGraphicsScene::itemAt：在 BPathItem 的事件处理函数中，你可以使用 QGraphicsScene::itemAt 方法来确定鼠标下面是否有 MPointItem，如果有，可以手动将事件传递给它。
5.检查父子关系：确认 MPointItem 是否确实是 BPathItem 的子项，如果不是，它们之间的 Z 值关系可能不会如你所预期那样工作。
6.使用 QGraphicsScene::sendEvent：作为最后的手段，你可以在 BPathItem 中手动发送事件到 MPointItem，使用场景的 sendEvent 方法。
调试这类问题时，一个有用的技巧是重写 MPointItem 和 BPathItem 的 paint 方法，在其中打印或显示它们的 Z 值，这有助于你确认它们的相对层叠顺序。
如果以上方法均无法解决问题，那么可能需要更深入地检查你的事件处理逻辑和场景的层次结构，以确定事件为何没有到达 MPointItem。
*/

    qDebug()<<"BPathItem created";
}
BPathItem::~BPathItem()
{
    qDebug()<<"Path destructed";
//    this->scene()->clear();
//    delete this;
}
void BPathItem::addNextPoint(QPointF p)
{
    BPointItem *point = new BPointItem(this, p);
    point->setParentItem(this);
    connect(point, &BPointItem::del_Point, this, &BPathItem::deletePoint );
    connect(point, &BPointItem::add_Next_Point, this, &BPathItem::insertPoint );

    pList.push_back(point);
    p_count ++ ;
    qDebug()<<"added point "<<p_count<<" : "<<p;
}
void BPathItem::addNextPoint(QPointF p, int *arr, std::map<QString, QString> self_df)
{
    BPointItem *point = new BPointItem(this, p);
    point->initAttr(arr,self_df);
    point->setParentItem(this);
    connect(point, &BPointItem::del_Point, this, &BPathItem::deletePoint );
    connect(point, &BPointItem::add_Next_Point, this, &BPathItem::insertPoint );
    pList.push_back(point);
    p_count ++ ;
    qDebug()<<"added point "<<p_count<<" : "<<p<<"attr:"<<arr[0]<<arr[1]<<arr[2]<<arr[3]<<arr[4]<<arr[5]<<arr[6]<<arr[7]<<point->attr->self_define.size();
}
void BPathItem::deletePoint(BPointItem* point)
{
    pList.erase(find(pList.begin(), pList.end(), point));
    p_count -- ;
    disconnect(point, &BPointItem::del_Point, this, &BPathItem::deletePoint );
    disconnect(point, &BPointItem::add_Next_Point, this, &BPathItem::insertPoint );
    delete point;
    this->update();
}
void BPathItem::insertPoint(BPointItem* point, qreal dense)
{
    int index = this->getSequence(point);
    qDebug()<<"cur_point: "<<index;
    if ( index == this->pointsNumber() )
    {
        QMessageBox::warning(NULL, tr("???"), tr("???"));
    }
    else if ( index == this->pointsNumber() - 1 )
    {
        QPointF p_new;
        if ( this->pointsNumber() == 1 )
        {
            p_new.setX( point->getPoint().rx() + dense );
            p_new.setY( point->getPoint().ry() );
        }
        else
        {
            const QPointF* last_one = new QPointF(this->pList[ index - 1 ]->getPoint());
            qreal yaw = atan2( last_one->y() - point->getPoint().y(), point->getPoint().x() - last_one->x() );
            p_new.setX( point->getPoint().x() + dense * cos( yaw ) );
            p_new.setY( point->getPoint().y() - dense * sin( yaw ) );
        }
        this->addNextPoint(p_new);
    }
    else
    {
        const QPointF* next_one = new QPointF( this->pList[ index + 1 ]->getPoint() );
        if ( ( point->getPoint().x() - next_one->x() ) * ( point->getPoint().x() - next_one->x() ) +
             ( point->getPoint().y() - next_one->y() ) * ( point->getPoint().y() - next_one->y() )
            <= dense * dense )
        {
            QMessageBox::warning(NULL, tr("warning"), tr("点太密了"));
            return;
        }
        QPointF p_new;
        qreal yaw = atan2( point->getPoint().y() - next_one->y(), next_one->x() - point->getPoint().x() );
        p_new.setX( point->getPoint().x() + dense * cos( yaw ) );
        p_new.setY( point->getPoint().y() - dense * sin( yaw ) );
        BPointItem *bp_new = new BPointItem(this, p_new);
        bp_new->setParentItem(this);
        connect(point, &BPointItem::del_Point, this, &BPathItem::deletePoint );
        connect(point, &BPointItem::add_Next_Point, this, &BPathItem::insertPoint );
        pList.insert( pList.begin() + index + 1, bp_new );
        p_count ++ ;
        qDebug()<<"added point "<<index+1<<" : "<<p_new;
    }
}
void BPathItem::setPoint(BPointItem* point, QPointF p)
{
    point->setPoint(p);
}
//void BPathItem::print_pList();
void BPathItem::clear()
{
    for ( auto it : pList ) it->deleteLater();
    pList.clear();
    p_count = 0;
//    this->scene()->update();
}

//void BPathItem::focusInEvent(QFocusEvent *event)
//{
//    Q_UNUSED(event);
//    this->setPen(m_pen_isSelected);
//}

//void BPathItem::focusOutEvent(QFocusEvent *event)
//{
//    Q_UNUSED(event);
//    this->setPen(m_pen_noSelected);
//}

QRectF BPathItem::boundingRect() const
{
//    return QRectF();
    return childrenBoundingRect();
}

void BPathItem::paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
//    painter->setPen(this->pen());
//    painter->setBrush(this->brush());
    painter->setBrush(this->brush());
    //反走样算法,资源消耗高！如果需要提高效率，请注释此行代码
//    painter->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);

    QPen linePen;
    linePen.setColor(QColor(240, 0, 0, 240));
    linePen.setWidth(1);
    painter->setPen(linePen);
//    qDebug()<<"paint"<<painter->pen()<<' '<<painter->brush();

    QPolygonF Ploygon;

    if(this->getStart())painter->drawText(this->getStart()->getPoint() - QPointF(0, 10), "Start");
    for ( auto iter = pList.begin(); iter != pList.end(); iter ++ )
    {
        QPointF a, b;
        a = (*iter)->getPoint();
        Ploygon << a;
        auto iter2 = iter;
        if ( ++ iter2 == pList.end() )
            break;
//            b = pList[0]->getPoint();
        else
            b = (*iter2)->getPoint();

//        if ( a != b ) painter->drawLine(a, b);
        painter->drawLine(a, b);
    }
    painter->setPen(m_pen);
//    qDebug()<<"paint"<<painter->pen()<<' '<<painter->brush();
    painter->drawPolyline(Ploygon);
//    qDebug()<<"paint finished";
}

void BPathItem::drawPath()
{
    this->scene()->update();
}

void BPathItem::pushPoint(QPointF p, QList<QPointF> list, bool isFinished)
{
    Q_UNUSED(list);
    if ( isFinished )
    {
        this->scene()->update();
    }
    else
    {
        addNextPoint(p);
        qDebug()<<"received point"<<this->pointsNumber();
        qDebug()<<"e1:"<<(void*)this<<this->scene();
        this->scene()->update();
        qDebug()<<"b4";
    }
}

void BPathItem::setPathWidth(qreal w)
{
    m_pen.setWidthF(w);
    this->update();
    qDebug()<<"m_pen seted";
}

void BPathItem::move(QPointF point)
{
    for ( int i = 0; i < p_count; ++ i )
    {
        pList[i]->rpoint() += point;
    }
}

void BPathItem::trans(qreal magnification)
{
    for ( int i = 0; i < p_count; ++ i )
    {
        pList[i]->rpoint() *= magnification;
    }
}

//##################################//
//********        3        *********//
//##################################//
BQGraphicsScene::BQGraphicsScene(QObject *parent) : QGraphicsScene(parent)
{
    is_creating_path = false;
    is_creating_virtual_wall = false;
    is_creating_mapSelect_ploygon = false;
    is_creating_erase_polygon = false;
    is_creating_pointSelect_polygon = false;
    is_creating_areaSelect_polygon = false;
    is_selecting_startPoint = false;
}

void BQGraphicsScene::startCreate()
{
    is_creating_path = true;
    m_list.clear();
}

void BQGraphicsScene::drawLine2img()
{
    is_creating_virtual_wall = true;
    m_list.clear();
}

void BQGraphicsScene::create_mapSelect_polygon()
{
    is_creating_mapSelect_ploygon = true;
    m_list.clear();
}

void BQGraphicsScene::drawWhitePolygon()
{
    is_creating_erase_polygon = true;
    m_list.clear();
}

void BQGraphicsScene::create_pointSelect_polygon()
{
    is_creating_pointSelect_polygon = true;
    m_list.clear();
}

void BQGraphicsScene::create_areaSelect_polygon()
{
    is_creating_areaSelect_polygon = true;
    m_list.clear();
}

void BQGraphicsScene::select_startPoint()
{
    is_selecting_startPoint = true;
}

QGraphicsScene* BQGraphicsScene::toQGraphicsScene()
{
    return this;
}

void BQGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (is_creating_path) {
        QPointF p(event->scenePos().x(), event->scenePos().y());

//        qDebug()<<"clicked"<<p;

        switch ( event->buttons() )
        {
        case Qt::LeftButton: {
            m_list.push_back(p);
            emit updatePoint(p, m_list, false);
        } break;
        case Qt::RightButton: {
//            if (m_list.size() >= 3) {
            emit updatePoint(p, m_list, true);
            qDebug()<<"send massage createFinished";
            emit createFinished();
            is_creating_path = false;
            m_list.clear();
//            }
        } break;
        default: break;
        }
    }
    /*
    else if (is_creating_virtual_wall)
    {
        QPointF p(event->scenePos().x(), event->scenePos().y());

        if ( event->buttons() == Qt::LeftButton )
        {
            m_list.push_back(p);
            if( m_list.size() == 2 )
            {
                emit drawLine( m_list );
                is_creating_virtual_wall = false;
                m_list.clear();

            }
        }
    }
    *///231015-腾飞
    else if (is_creating_virtual_wall)
    {
        QPointF p(event->scenePos().x(), event->scenePos().y());

        if( event->buttons() == Qt::LeftButton )
        {
            m_list.push_back(p);
            if( m_list.size() >= 2)
                emit drawLine( m_list );
            //            if( m_list.size() == 2)
            //            {
            //                emit drawLine( m_list );
            //                is_creating_virtual_wall = false;
            //                m_list.clear();

            //            }
        }
        else if(event->buttons() == Qt::RightButton){
            is_creating_virtual_wall = false;
            QPointF pmax(-1,-1);
            m_list.push_back(pmax);
            emit drawLine( m_list );
            m_list.clear();
        }
    }
    else if (is_creating_mapSelect_ploygon)
    {
        QPointF p(event->scenePos().x(), event->scenePos().y());

        switch ( event->buttons() )
        {
        case Qt::LeftButton: {
            m_list.push_back(p);
            qDebug()<<"左键点击"<<p;
            emit map_ploygon_addPoint(p, false);
        } break;
        case Qt::RightButton: {
            if (m_list.size() >= 3)
            {
                emit map_ploygon_addPoint(p, true);
                emit createFinished_mp();
                is_creating_mapSelect_ploygon = false;
                m_list.clear();
            }
            else
            {
                QMessageBox::warning(nullptr, tr("出错啦"), tr("多边形点不能少于3个"), QMessageBox::Ok);
            }
        } break;
        default: break;
        }
    }
    else if ( is_creating_erase_polygon )
    {
        QPointF p(event->scenePos().x(), event->scenePos().y());

        switch ( event->buttons() )
        {
        case Qt::LeftButton: {
            m_list.push_back(p);
            qDebug()<<"左键点击"<<p;
            emit erase_ploygon_addPoint(p);
        } break;
        case Qt::RightButton: {
            if (m_list.size() >= 3)
            {
                emit createFinished_ep(m_list);
                is_creating_erase_polygon = false;
                m_list.clear();
            }
            else
            {
                QMessageBox::warning(nullptr, tr("出错啦"), tr("多边形点不能少于3个"), QMessageBox::Ok);
            }
        } break;
        default: break;
        }
    }
    else if ( is_creating_pointSelect_polygon )
    {
        QPointF p(event->scenePos().x(), event->scenePos().y());

        switch ( event->buttons() )
        {
        case Qt::LeftButton: {
            m_list.push_back(p);
            qDebug()<<"左键点击"<<p;
            emit pointSelect_polygon_addPoint(p, false);
        } break;
        case Qt::RightButton: {
            if (m_list.size() >= 3)
            {
                emit pointSelect_polygon_addPoint(p, true);
                emit createFinished_pp();
                is_creating_pointSelect_polygon = false;
                m_list.clear();
            }
            else
            {
                QMessageBox::warning(nullptr, tr("出错啦"), tr("多边形点不能少于3个"), QMessageBox::Ok);
            }
        } break;
        default: break;
        }
    }
    else if ( is_creating_areaSelect_polygon )
    {
        QPointF p(event->scenePos().x(), event->scenePos().y());

        switch ( event->buttons() )
        {
        case Qt::LeftButton: {
            m_list.push_back(p);
            qDebug()<<"左键点击"<<p;
            emit areaSelect_polygon_addPoint(p, false);
        } break;
        case Qt::RightButton: {
            if (m_list.size() >= 3)
            {
                emit areaSelect_polygon_addPoint(p, true);
                emit createFinished_ap();
                is_creating_areaSelect_polygon = false;
                m_list.clear();
            }
            else
            {
                QMessageBox::warning(nullptr, tr("出错啦"), tr("多边形点不能少于3个"), QMessageBox::Ok);
            }
        } break;
        default: break;
        }
    }
    else if ( is_selecting_startPoint )
    {
        QPointF p(event->scenePos().x(), event->scenePos().y());
        switch ( event->buttons() )
        {
        case Qt::LeftButton: {
            emit selected_startPoint_accept(p);
            is_selecting_startPoint = false;
        } break;
        case Qt::RightButton: {
            emit selected_startPoint_reject();
            is_selecting_startPoint = false;
        } break;
        default: break;
        }
    }
    else {
        QGraphicsScene::mousePressEvent(event);
    }
}

void BQGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF p(event->scenePos().x(), event->scenePos().y());
    emit curMousePos(p);
    QGraphicsScene::mouseMoveEvent(event);
//    QAbstractGraphicsShapeItem::mouseMoveEvent(event);
}

