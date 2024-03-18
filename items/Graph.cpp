#include "Graph.h"

Graph::Graph( int rows, int cols ):
    rows(rows), cols(cols)
{
    /*
    graph = new bool*[rows];
    for ( int i = 0; i < rows; ++ i )
        graph[i] = new bool[cols];
    for ( int i = 0; i < rows; ++ i )
        for ( int j = 0; j < cols; ++ j )
            graph[i][j] = false;
    *///这感觉没问题，但好像有问题//换了之后还真没问题了？？？
    graph = std::vector<std::vector<bool>>(rows, std::vector<bool>(cols, false));
}

Graph::~Graph()
{
    /*
    for ( int i = 0 ; i < rows ; i ++ )
        delete[] graph[i];
    delete[] graph;
    */
}

cv::Mat Graph::toMat()
{
    cv::Mat imgGraph = cv::Mat::zeros(rows, cols, CV_8U);
    for ( int i = 0; i < rows; ++ i )
    {
        for ( int j = 0; j < cols; ++ j )
        {
            if ( graph[i][j] )
            {
                imgGraph.at<uint8_t>(i, j) = 255;
            }
        }
    }
    return imgGraph.clone();
}

void Graph::toGraph(cv::Mat imgGraph)
{
    for ( int i = 0; i < rows; ++ i )
    {
        for ( int j = 0; j < cols; ++ j )
        {
            if ( imgGraph.ptr<uchar>(i)[j] == 0 )
            {
                graph[i][j] = false;
            }
            else
            {
                graph[i][j] = true;
            }
        }
    }
}

//废弃
//void Graph::update(QPainterPath path)
//{
//    for(auto& row : graph) {
//        row.assign(cols, false);
//    }
//    QRectF boundingRect = path.boundingRect();
//    for ( int i = qMax(0, (int)(boundingRect.top())); i < qMin(rows,(int)(boundingRect.bottom())); ++ i )
//    {
//        qDebug()<<"row:"<<i;
//        for ( int j = qMax(0, (int)(boundingRect.left())); j < qMin(cols, (int)(boundingRect.right())); ++ j )
//        {
////            graph[i][j] = false;  //之前置零了，不需要设false了
//            //if ( polygon.containsPoint( QPoint( j, i ), Qt::OddEvenFill ) ) //这里注意坐标系(x,y)对应(j,i);
//            if ( path.contains( QPointF( j, i ) ) ) //这里注意坐标系(x,y)对应(j,i);//这个快很多
//            {
//                graph[i][j] = true;
//            }
//        }
//    }
//}

QPainterPath Graph::pointsToPath()
{
    /*1*/
//    QPainterPath path;
//    for ( size_t i = 0; i < graph.size(); ++i) {
//        for ( size_t j = 0; j < graph[i].size(); ++j) {
//            if (graph[i][j]) {
////                path.addRect(QRect(j,i,1,1));
//                QPointF point( j, i );
//                if (path.isEmpty()) {
//                    path.moveTo(point);
//                } else {
//                    path.lineTo(point);
//                }
//            }
//        }
//    }

////    path.closeSubpath();
//    return path;
    /*2*/
//    QPainterPath path;

//    // 从左到右扫描数组
//    for (size_t i = 0; i < graph.size(); ++i) {
//        for (size_t j = 0; j < graph[i].size(); ++j) {
//            if (graph[i][j]) {
//                if (path.isEmpty()) {
//                    path.moveTo(j, i);
//                } else {
//                    path.lineTo(j, i);
//                }
//                break;
//            }
//        }
//    }

//    // 从右到左扫描数组
//    for (int i = graph.size() - 1; i >= 0; --i) {
//        for (int j = graph[i].size() - 1; j >= 0; --j) {
//            if (graph[i][j]) {
//                path.lineTo(j, i);
//                break;
//            }
//        }
//    }

//    path.closeSubpath();
//    return path;
    /*3*/
}

void Graph::openOperation(int k)
{
    cv::Mat imgGraph = toMat();
    k = k / 2;
    k = k * 2 + 1;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(k, k));
    cv::erode(imgGraph, imgGraph, kernel);    //膨胀
    cv::dilate(imgGraph, imgGraph, kernel);   //腐蚀
    toGraph(imgGraph);
//    cv::imshow("graph", imgGraph);
}

std::vector<std::vector<QPointF>> Graph::openOperationTOcontours2d(int k)
{
    cv::Mat imgGraph = toMat();
    k = k / 2;
    k = k * 2 + 1;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(k, k));
    cv::erode(imgGraph, imgGraph, kernel);    //膨胀
    cv::dilate(imgGraph, imgGraph, kernel);   //腐蚀
//    toGraph(imgGraph);
    //    cv::imshow("graph", imgGraph);
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours( imgGraph, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE );
    std::vector<std::vector<QPointF>> contours2d;
    for ( size_t i = 0; i < contours.size(); ++ i )
    {
        std::vector<QPointF> contour2d;
        for ( size_t j = 0; j < contours[i].size(); ++ j )
        {
            contour2d.push_back(QPointF(contours[i][j].x, contours[i][j].y));
        }
        contours2d.push_back(contour2d);
    }
    return contours2d;
}

void Graph::show()
{
    cv::Mat imgGraph = toMat();
    cv::namedWindow("graph", cv::WINDOW_NORMAL);
    cv::imshow("graph", imgGraph);
}

cv::Mat Graph::getContours2d(std::vector<std::vector<QPointF>> contours2d)
{
    cv::Mat imgGraph = cv::Mat::zeros(rows, cols, CV_8U);
    std::vector<std::vector<cv::Point>> contours;
    for ( size_t i = 0; i < contours2d.size(); i ++ )
    {
        std::vector<cv::Point> pts;
        for ( size_t j = 0; j < contours2d[i].size(); j ++ )
        {
            pts.push_back(cv::Point((int)(contours2d[i][j].x()), (int)(contours2d[i][j].y())));
        }
        contours.push_back(pts);
    }
    cv::drawContours( imgGraph, contours, -1, 255, cv::FILLED, 8 );

    return imgGraph.clone();
}

bool Graph::updateFromContours2d(std::vector<std::vector<QPointF>> contours2d)
{
    cv::Mat imgGraph = cv::Mat::zeros(rows, cols, CV_8U);
    std::vector<std::vector<cv::Point>> contours;
    for ( size_t i = 0; i < contours2d.size(); i ++ )
    {
        std::vector<cv::Point> pts;
        for ( size_t j = 0; j < contours2d[i].size(); j ++ )
        {
            pts.push_back(cv::Point((int)(contours2d[i][j].x()), (int)(contours2d[i][j].y())));
        }
        contours.push_back(pts);
    }
//    cv::drawContours(imgGraph, contours, -1, 255, cv::FILLED, 8);
    try {
        cv::drawContours(imgGraph, contours, -1, 255, cv::FILLED, 8);
    } catch (const cv::Exception& e) {
        // 捕获 OpenCV 异常并进行处理
        qDebug() << "OpenCV Exception: " << e.what();
        // 进行适当的错误处理
        return false;
    } catch (const std::exception& e) {
        // 捕获其他异常并进行处理
        qDebug() << "Exception: " << e.what();
        // 进行适当的错误处理
        return false;
    } catch (...) {
        // 捕获任何其他类型的异常并进行处理
        qDebug() << "Unknown Exception";
        // 进行适当的错误处理
        return false;
    }

    toGraph( imgGraph );
    return true;
}
