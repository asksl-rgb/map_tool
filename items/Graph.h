#ifndef GRAPH_H
#define GRAPH_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <QPainterPath>
#include <QDebug>

class Graph
{
public:
    Graph( int rows, int cols );
    ~Graph();
    cv::Mat toMat();
    void toGraph(cv::Mat imgGraph);
//    void update(QPainterPath path);   //废弃
    void openOperation(int k);
    std::vector<std::vector<QPointF>> openOperationTOcontours2d(int k);
    void show();
    cv::Mat getContours2d(std::vector<std::vector<QPointF>> contours2d);
    bool updateFromContours2d(std::vector<std::vector<QPointF>> contours2d);

    QPainterPath pointsToPath();

    int rows;
    int cols;
    std::vector<std::vector<bool>> graph;
//    cv::Mat imgGraph;

};

#endif // GRAPH_H
