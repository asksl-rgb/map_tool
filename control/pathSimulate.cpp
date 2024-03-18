#include "pathSimulate.h"

PathSimulate::PathSimulate(QPointF startPoint, double angle, double w, double mr) :
    curPoint(startPoint), angle(angle)
{
    widthPX2 = w / 2.0;
    minRadius = mr;
    rightPoint = QPointF( startPoint.x() - widthPX2 * sin(angle), startPoint.y() + widthPX2 * cos(angle) );
    leftPoint = QPointF( startPoint.x() + widthPX2 * sin(angle), startPoint.y() - widthPX2 * cos(angle) );

}

PathSimulate::~PathSimulate(){}

void PathSimulate::updatePoint()
{
    rightPoint = QPointF( curPoint.x() - widthPX2 * sin(angle), curPoint.y() + widthPX2 * cos(angle) );
    leftPoint = QPointF( curPoint.x() + widthPX2 * sin(angle), curPoint.y() - widthPX2 * cos(angle) );

}

bool PathSimulate::fd( double dis )
{
    curPoint = QPointF( curPoint.x() + dis * cos(angle), curPoint.y() + dis * sin(angle) );
    updatePoint();
    return true;
}

bool PathSimulate::bk( double dis )
{
    curPoint = QPointF( curPoint.x() - dis * cos(angle), curPoint.y() - dis * sin(angle) );
    updatePoint();
    return true;
}

std::vector<QPoint> PathSimulate::round() const
{
    std::set<QPoint, QPointCompare> tmp;
    for ( int i = 0; i < 360; ++ i )
    {
        tmp.insert( QPoint( curPoint.x() + widthPX2 * cos( M_PI * i / 180 ), curPoint.y() + widthPX2 * sin( M_PI * i / 180 ) ) );
    }
    std::vector<QPoint> res;
    for ( auto& it : tmp )
    {
        res.push_back(it);
    }
    return res;
}

