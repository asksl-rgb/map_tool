#include "graphicsview.h"

//重写QGraphicsView实现图像的缩放

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent)
{
}

GraphicsView::~GraphicsView()
{
}

//使得鼠标能够实现放大缩小
/*
void GraphicsView::wheelEvent(QWheelEvent *e)
{
    int distance = e->delta()/66;
    double val;

    if (distance != 0)
    {
        val = pow(1.2, distance);
        this->scale(val, val);
    }
}
*/
//231015-腾飞
void GraphicsView::wheelEvent(QWheelEvent *e)
{
    QPointF cursorPoint = e->position();
    QPointF scenePos = this->mapToScene(QPoint(cursorPoint.x(), cursorPoint.y()));
    QPointF centerscenePos = mapToScene(this->width() / 2, this->height() / 2);
    //    qreal viewWidth = this->viewport()->width();
    //    qreal viewHeight = this->viewport()->height();
    //    qreal hScale = cursorPoint.x() / viewWidth;
    //    qreal vScale = cursorPoint.y() / viewHeight;
    int distance = e->angleDelta().y()/66;
    double val;

    if (distance != 0)
    {
        val = pow(1.2, distance);
        this->scale(val, val);
        centerOn((centerscenePos.x()-scenePos.x())/val+scenePos.x(),(centerscenePos.y()-scenePos.y())/val+scenePos.y());
    }


}
