#ifndef PATHSIMULATE_H
#define PATHSIMULATE_H

#include <QPointF>
#include <cmath>
#include <set>
#include <vector>
#include <QDebug>

struct QPointCompare
{
    bool operator()(const QPoint& p1, const QPoint& p2) const
    {
        if ( p1.x() < p2.x() )
            return true;
        else
            return p1.y() < p2.y();
    }
};

class PathSimulate
{
public:
    PathSimulate(QPointF startPoint,
                 double angle,
                 double w,
                 double mr);
    ~PathSimulate();
    void updatePoint();
    bool fd(double dis);
    bool bk(double dis);
    std::vector<QPoint> round() const;

    QPointF curPoint;
    double angle;
    double widthPX2;
    double minRadius;
    QPointF rightPoint;
    QPointF leftPoint;

};

#endif // PATHSIMULATE_H
