#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <string>
#include <QFileDialog>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include <QPointF>
#include <QLineEdit>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <fstream>
//#include "yaml-cpp/yaml.h"
#include <sstream>

#include <cmath>
#include <chrono>   //测试用

#include <QTextCodec>

#include "graphicsview.h"
#include "../dialog/aboutdialog.h"
#include "../dialog/coverPathDialog.h"
#include "../items/GraphicsType.h"
#include "../items/mapSelectItem.h"
#include "../items/startRect.h"
#include "../items/erasePolygon.h"
#include "../tools/UTF8ToGBEx.h"
#include "../items/Graph.h"
#include "../control/pathSimulate.h"
#include "../items/test.h"
#include "../dialog/attributeAddDialog.h"
#include "../dialog/setFunctionalAreaName.h"
#include "../dialog/coverPath_rtk_Dialog.h"

#define WINDOW_TITLE "Map Tool"
#define WINDOW_ERROR "Error - Map Tool"
#define DIS2(a,b) ((a.x()-b.x())*(a.x()-b.x())+(a.y()-b.y())*(a.y()-b.y())) //without sqrt!

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    cv::Mat imgSrc, imgTmp;
    void closeEvent(QCloseEvent *);
    void updateImage(const QPixmap &pixmap);
    void createToolBar();
    void setActionStatus(bool);
    void setActionStatus_all(bool);
    void cleanImage();
    void openBlankPixmap();

    //将QPixmap转化为Mat
    inline cv::Mat QPixmapToCvMat( const QPixmap &inPixmap )
    {
        QImage qimg = inPixmap.toImage().convertToFormat(QImage::Format_RGB888).rgbSwapped();
        return cv::Mat(qimg.height(), qimg.width(), CV_8UC3, const_cast<uchar*>(qimg.bits()), static_cast<size_t>(qimg.bytesPerLine())).clone();
    }
    inline cv::Mat QPixmapToCvMat8U( const QPixmap &inPixmap )
    {
        QImage qimg = inPixmap.toImage().convertToFormat(QImage::Format_Grayscale8).rgbSwapped();
        return cv::Mat(qimg.height(), qimg.width(), CV_8U, const_cast<uchar*>(qimg.bits()), static_cast<size_t>(qimg.bytesPerLine())).clone();
    }
    //将Mat转化为QPixmap
    inline QPixmap CvMatToQPixmap( const cv::Mat &inCvMat )
    {
        cv::Mat qimg;
        cv::cvtColor(inCvMat, qimg, cv::COLOR_BGR2RGB);
        return QPixmap::fromImage(QImage((const unsigned char*)(qimg.data), qimg.cols, qimg.rows, static_cast<int>(qimg.step), QImage::Format_RGB888));
    }
    inline QPixmap CvMat8UToQPixmap( const cv::Mat &inCvMat )
    {
        return QPixmap::fromImage(QImage((const unsigned char*)(inCvMat.data), inCvMat.cols, inCvMat.rows, static_cast<int>(inCvMat.step), QImage::Format_Grayscale8));
    }

    void openYaml(QString);
    void setPathWidth(QString);
    std::vector<std::vector<QPointF>> mapSelectedTOcontours2d(MapSelectItem*);
    std::vector<std::vector<QPointF>> smooth_coverPathEdges(std::vector<std::vector<QPointF>> contours2d, int w, double mr);
    std::vector<std::vector<QPointF>> extractContoursFromPainterPath(const QPainterPath& path);
    QPainterPath QPainterPathFromContours2d(std::vector<std::vector<QPointF>> contours2d);
    void initMapSelect();
    void initPointSelect();
    void initMPath();
    void initAreaSelect();
    double crossProduct(const QPointF& p1, const QPointF& p2);
    bool isClockwise();
    QPointF findNearestPoint(const std::vector<std::vector<QPointF>>& contours2d, const QPointF& startPoint);

signals:
    void cleanLastSCP();

public slots:
    void drawVirtualWall( QList<QPointF> );
    void receiveCoverPathValue(int, double, int, QPointF, qreal);
    void show_smooth_coverPathEdges(int, double);
    void drawWhitePolygon(QList<QPointF>);
    void printAttribute(std::string);
    void getAttributeData(std::string);
    void selectingStartPoint();
    void receiveCoverPath_rtk_Value(int, double, int, int, int);

private slots:
    void on_doOpen_triggered(); //打开文件
    void on_doOpenJson_triggered(); //打开json文件
    void on_doClose_triggered();    //关闭文件
    void on_doSave_triggered(); //保存
    void on_doSaveJson_triggered(); //保存json文件
    void on_doSave_As_triggered();  //另存为
//TODO：on_doSaveJson_As_triggered();
    void on_doExit_triggered(); //退出
    void on_doRestore_triggered();  //恢复图像
    void on_do_adjust_triggered();  //调整图片适应窗口
    void on_doAbout_triggered();    //关于
    void on_do_AutoPS_wec_triggered();  //自动修图
    void on_doCleanJson_triggered();    //清除轨迹
    void on_actiontest_triggered(); //用于测试想要测试的
    void on_do_draw_path_triggered();   //自由绘制轨迹
    void on_do_fill_path_triggered();   //填充轨迹
    void on_doOpenYaml_triggered(); //打开YAML文件
    void on_doDrawLine_triggered(); //绘制虚拟墙
    void on_doDrawTraj_png_triggered(); //保存截图到png
    void on_do_draw_polygons_area_triggered();  //绘制多边形区域
    void on_do_draw_ploygons_triggered();   //填充区域
    void on_do_adjust_polygons_area_triggered();    //调整多边形区域
    void on_do_draw_full_covered_path_triggered();  //绘制覆盖式轨迹
    void on_do_save_polygons_area_triggered();  //保存区域
    void on_do_load_polygons_area_triggered();  //加载区域
    void on_do_clean_polygons_area_triggered(); //清除区域
    void on_doDrawWhitePolygon_triggered(); //多边形橡皮擦
    void on_doDrawPathContinue_triggered(); //追加轨迹
    void on_do_rtk_full_cover_path_triggered(); //rtk覆盖式轨迹
    void on_doOpenJsonExclusively_triggered();//仅打开Json文件
    void on_do_selectMultiPoint_polygen_triggered();//轨迹标注-框选点
    void on_do_clean_pointSelect_area_triggered();//轨迹标注-清除框选
    void on_do_addAttribute_triggered();//轨迹标注-添加属性
    void on_doCleanAllMPath_triggered();//清除所有轨迹
    void on_do_draw_next_mpath_triggered();//保存并进行下一条轨迹绘制
    void on_doOpenJson_multiMPath_triggered();//加载Json文件(多条轨迹)
    void on_do_draw_functional_area_triggered();//绘制功能区
    void on_do_clean_current_functional_area_triggered();//清除当前功能区
    void on_do_clean_all_functional_area_triggered();//清除所有功能区
    void on_do_draw_next_functional_area_triggered();//保存并进行下一个功能区绘制
    void on_do_save_functional_area_triggered();//记录功能区

private:
    Ui::MainWindow *ui;
    //图片
    BQGraphicsScene *pgmScene;
    QGraphicsPixmapItem *pgmPixmapItem;
    bool isBlankMap = false;

    //图片大小label
    QLabel *size;
    //图片像素
    QLabel *pixel;
    //文件信息读取
    QFileInfo *info;
    //用于存放 图片地址（下次打开，保存，另存）
    QString ConstImage;
    QString ConstJson;
    //是否修改图片
    bool flag;
    //用于获取桌面位置
    QString getUserPath();

    //BPathItem轨迹
    QList<BPathItem*> m_path_list;
    BPathItem *m_path = nullptr;
    //BPathItem轨迹宽度
    qreal m_path_width = 1;
//    BPathItem* m_path = new BPathItem;
    bool connect_flag = false;
    //json文件内容
    QString jsonContent;
    //yaml地图信息
    qreal resolution = 0, origin_x = 0, origin_y = 0, yaw = 0;
    //保存轨迹需要地图信息（yaml文件）
    QString yamlPath;
    //起始位置Rect
    startRectItem *startRect;
    //路径区域
    MapSelectItem *mapSelected = nullptr;
    //橡皮擦
    erasePolygon *erase = nullptr;
    //轨迹标注-框选区域
    MapSelectItem *pointSelected = nullptr;
    QList<QList<int> > point_idx; //暂存选取的point
    std::string attributeData;  //暂存attributeData
    //功能区
    MapSelectItem *areaSelected = nullptr;
    QList<MapSelectItem*> areaSelected_list;

    //覆盖式轨迹输入框
    CoverPathDialog *coverPathDialog = nullptr;

    //curMousePos
    QLabel *mousePos;

};

#endif // MAINWINDOW_H
