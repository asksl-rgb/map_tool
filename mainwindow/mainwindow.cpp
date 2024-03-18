#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //设定关闭此窗口，则关闭所有窗口
    this->setAttribute( Qt::WA_QuitOnClose, true );

    //初始化
    pgmScene = new BQGraphicsScene;  //类 用于在场景中操作大量的2D图形元素
    pgmPixmapItem = nullptr;
//    m_path = newBPathItem();    //其实会在打开图片那里cleanImage()进行初始化;
//    m_path = new BPathItem;
    qDebug()<<"m_path:"<<(void*)m_path;
    initMPath();
    qDebug()<<"m_path:"<<(void*)m_path;
    initMapSelect();
    initPointSelect();
    initAreaSelect();

    pgmScene->setItemIndexMethod(QGraphicsScene::NoIndex);  //QGraphicsScene::NoIndex：不保存图形项索引。定位图形项具有线性复杂性，因为会搜索场景中的所有图形项。但是，添加，移动和删除图形项是在固定时间内完成的。这种方法最适合动态场景，在动态场景中，要连续添加，移动或删除许多图形项。
//    pgmScene->addItem(m_path);
//    m_path->setParentItem(pgmPixmapItem);

    //初始化
    pixel = new QLabel;
    size = new QLabel;
    mousePos = new QLabel;
    info = nullptr;
    ConstImage = "";
    flag = false;   //用于退出确认

    //初始化界面
    pgmScene->setBackgroundBrush(QColor::fromRgb(224,224,224));
    ui->pgmGraphicsView->setScene(pgmScene);
    ui->pgmGraphicsView->setMouseTracking(true);

    //初始化底部statusBar
    ui->statusbar->addPermanentWidget(size);
    ui->statusbar->addPermanentWidget(pixel);
    ui->statusbar->addPermanentWidget(mousePos);

    createToolBar();
    setActionStatus(false);
    setWindowTitle("Map Tool");

    //BPathItem : m_path
//    connect(pgmScene, SIGNAL(updatePoint(QPointF,QList<QPointF>,bool)), m_path, SLOT(pushPoint(QPointF,QList<QPointF>,bool)));
    connect(pgmScene, &BQGraphicsScene::createFinished, this, [=](){
        setActionStatus(true);
        ui->statusbar->showMessage( "选择点进行调整(左键点击拖动，左键选中后右键)，调整结束选择'轨迹绘制-保存json文件'保存路径" );
    });
    //MapSelectItem :
    connect(pgmScene, &BQGraphicsScene::createFinished_mp, this, [=](){
        setActionStatus(true);
        ui->statusbar->showMessage( "可选择点进行调整或继续绘制多边形区域，或者进行区域填充，调整结束可进行覆盖式轨迹生成" );
    });
    //MapSelectItem : pointSelecrted
    connect(pgmScene, &BQGraphicsScene::createFinished_pp, this, [=](){
        setActionStatus(true);
        ui->statusbar->showMessage( "可选择点进行调整或继续绘制多边形区域，调整结束可对点进行添加属性" );
    });
    //MapSelectItem : areaSelect
    connect(pgmScene, &BQGraphicsScene::createFinished_ap, this, [=](){
        setActionStatus(true);
        ui->statusbar->showMessage( "可选择点进行调整或继续绘制功能区，调整结束可进行下一功能区绘制或者保存区域信息" );
    });

    //CoverPathDialog : coverPathDialog
    connect(pgmScene, &BQGraphicsScene::selected_startPoint_accept,
            this, [=](const QPointF& point)
            {
        setActionStatus(true);
        ui->statusbar->showMessage( QString("覆盖式轨迹起始点选择为(%1, %2)").arg(point.x()).arg(point.y()), 5000 );
        coverPathDialog->activateWindow();
            });
    connect(pgmScene, &BQGraphicsScene::selected_startPoint_reject,
            this, [=](){
        setActionStatus(true);
        ui->statusbar->showMessage( "取消选择", 3000 );
            });

    //
    connect(pgmScene, SIGNAL(drawLine(QList<QPointF>)), this, SLOT(drawVirtualWall(QList<QPointF>)) );

    //curMousePos
    connect(pgmScene, &BQGraphicsScene::curMousePos, this, [=](QPointF p){
        mousePos->setText( "x: " + QString::number(p.x(), 'f', 2) + " y: " + QString::number(p.y(), 'f', 2) );
    });

    //erasePolygon
    connect(pgmScene, &BQGraphicsScene::createFinished_ep, this, &MainWindow::drawWhitePolygon);

    if(__cplusplus == 201402L)
    qDebug()<<"c++_version: "<<__cplusplus;

//    ui->do_rtk_full_cover_path->setEnabled(false);
//    ui->doOpenJsonExclusively->setEnabled(false);
//    ui->do_draw_full_covered_path->setEnabled(false);

}

//初始化m_path //此函数禁止直接调用
void MainWindow::initMPath()    //此函数禁止直接调用
{
    if ( m_path )
    {
        disconnect(pgmScene, SIGNAL(updatePoint(QPointF,QList<QPointF>,bool)), m_path, SLOT(pushPoint(QPointF,QList<QPointF>,bool)));
        if ( m_path->scene() == pgmScene )
            pgmScene->removeItem( m_path );
        delete m_path;
    }

    m_path = new BPathItem;
    m_path->setPathWidth(m_path_width);

    //BPathItem : m_path
    connect(pgmScene, SIGNAL(updatePoint(QPointF,QList<QPointF>,bool)), m_path, SLOT(pushPoint(QPointF,QList<QPointF>,bool)));

}

//初始化mapSelected
void MainWindow::initMapSelect()
{
    if ( mapSelected )
    {
        disconnect(pgmScene, SIGNAL(map_ploygon_addPoint(QPointF,bool)), mapSelected, SLOT(cur_ploygon_addPoint(QPointF,bool)));
        if ( mapSelected->scene() == pgmScene )
            pgmScene->removeItem(mapSelected);
        delete mapSelected;
    }

    mapSelected = new MapSelectItem;

    //MapSelectItem : mapSelected
    connect(pgmScene, SIGNAL(map_ploygon_addPoint(QPointF,bool)), mapSelected, SLOT(cur_ploygon_addPoint(QPointF,bool)));

}

//初始化pointSelected
void MainWindow::initPointSelect()
{
    if( pointSelected )
    {
        disconnect(pgmScene, SIGNAL(pointSelect_polygon_addPoint(QPointF,bool)), pointSelected, SLOT(cur_ploygon_addPoint(QPointF,bool)));
        if ( pointSelected->scene() == pgmScene )
            pgmScene->removeItem(pointSelected);
        delete pointSelected;
    }

    pointSelected = new MapSelectItem;

    //MapSelectItem : pointSelecrted
    connect(pgmScene, SIGNAL(pointSelect_polygon_addPoint(QPointF,bool)), pointSelected, SLOT(cur_ploygon_addPoint(QPointF,bool)));

}

//初始化areaSelected
void MainWindow::initAreaSelect()
{
    if( areaSelected )
    {
        disconnect(pgmScene, SIGNAL(areaSelect_polygon_addPoint(QPointF,bool)), areaSelected, SLOT(cur_ploygon_addPoint(QPointF,bool)));
        if ( areaSelected->scene() == pgmScene )
            pgmScene->removeItem(areaSelected);
        delete areaSelected;
    }

    areaSelected = new MapSelectItem;

    //MapSelectItem : areaSelecrted
    connect(pgmScene, SIGNAL(areaSelect_polygon_addPoint(QPointF,bool)), areaSelected, SLOT(cur_ploygon_addPoint(QPointF,bool)));

}

MainWindow::~MainWindow()
{
    delete ui;

    if(pgmScene)
    {
        delete pgmScene;
        pgmScene = nullptr;
    }

    if(size)
    {
        delete size;
        size = nullptr;
    }

    if(pixel)
    {
        delete pixel;
        pixel = nullptr;
    }

    if(mousePos)
    {
        delete mousePos;
        mousePos = nullptr;
    }
}

//退出事件
void MainWindow::closeEvent(QCloseEvent *event){
    //是否更改
    if(flag){
        if(QMessageBox::Yes == QMessageBox::question(this,tr("尚未保存图片"),tr("是否保存？"),QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes)){
            on_doSave_triggered();
        }
    }
    if(QMessageBox::Yes == QMessageBox::question(this,tr("退出"),tr("是否退出？"),QMessageBox::Yes | QMessageBox::No,QMessageBox::No)){
        event->accept();
    }else {
        event->ignore();
    }
}

// 更新图片
void MainWindow::updateImage(const QPixmap &pixmap)
{
    //刷新图片
    pgmPixmapItem->setPixmap(pixmap);
    pgmScene->setSceneRect(QRectF(pixmap.rect()));
}

//初始化toolBar
void MainWindow::createToolBar()
{
    ui->toolBar_File->addAction(ui->doOpen);
    ui->toolBar_File->addAction(ui->doSave);
    ui->toolBar_File->addAction(ui->doSave_As);
    ui->toolBar_File->addSeparator();
    ui->toolBar_File->addAction(ui->doClose);

    ui->toolBar_Edit->addAction(ui->doRestore);
    ui->toolBar_Edit->addAction(ui->do_adjust);
    ui->toolBar_Edit->addSeparator();
    ui->toolBar_Edit->addAction(ui->do_AutoPS_wec);
    ui->toolBar_Edit->addAction(ui->doDrawLine);
    ui->toolBar_Edit->addAction(ui->doDrawWhitePolygon);

    QLabel *label_pathWidth = new QLabel;
    label_pathWidth->setText("轨迹宽度：");

    QLineEdit *lineEdit_pathWidth = new QLineEdit;
    lineEdit_pathWidth->setEchoMode(QLineEdit::Normal);
    lineEdit_pathWidth->setPlaceholderText("140");
    //    lineEdit_pathWidth->setInputMask("0000;_");
    lineEdit_pathWidth->setFixedWidth(40);
    lineEdit_pathWidth->setMaxLength(4);
    connect(lineEdit_pathWidth, &QLineEdit::returnPressed, this, [=](){ setPathWidth(lineEdit_pathWidth->text()); });

    QLabel *label_cm = new QLabel;
    label_cm->setText("cm");

    ui->toolBar_PathEdit->addAction(ui->doOpenYaml);
    ui->toolBar_PathEdit->addSeparator();
    ui->toolBar_PathEdit->addAction(ui->doOpenJson);
    ui->toolBar_PathEdit->addAction(ui->do_draw_path);
    ui->toolBar_PathEdit->addAction(ui->doDrawPathContinue);
    ui->toolBar_PathEdit->addAction(ui->do_fill_path);
    ui->toolBar_PathEdit->addAction(ui->doCleanJson);
    ui->toolBar_PathEdit->addWidget(label_pathWidth);
    ui->toolBar_PathEdit->addWidget(lineEdit_pathWidth);
    ui->toolBar_PathEdit->addWidget(label_cm);
    ui->toolBar_PathEdit->addSeparator();
    ui->toolBar_PathEdit->addAction(ui->doSaveJson);
    ui->toolBar_PathEdit->addAction(ui->doDrawTraj_png);

    ui->toolBar_fullCover->addAction(ui->do_draw_polygons_area);
    ui->toolBar_fullCover->addAction(ui->do_load_polygons_area);
    ui->toolBar_fullCover->addAction(ui->do_clean_polygons_area);
    ui->toolBar_fullCover->addAction(ui->do_draw_ploygons);
    ui->toolBar_fullCover->addSeparator();
    ui->toolBar_fullCover->addAction(ui->do_draw_full_covered_path);


    ui->toolBar_About->addAction(ui->doAbout);
    ui->toolBar_About->addSeparator();
    ui->toolBar_About->addAction(ui->doExit);


}

//未加载图片，将图片处理按钮设为不可用
void MainWindow::setActionStatus(bool status)
{
    ui->doCool->setEnabled(status);
    ui->doWarm->setEnabled(status);
    ui->doGauss->setEnabled(status);
    ui->doDilate->setEnabled(status);
    ui->doRotate->setEnabled(status);
    ui->doSimple->setEnabled(status);
    ui->doClosing->setEnabled(status);
    ui->doLaplace->setEnabled(status);
    ui->doOpening->setEnabled(status);
    ui->doRestore->setEnabled(status);
    ui->doSave->setEnabled(status);
    ui->doSave_As->setEnabled(status);
    ui->do_adjust->setEnabled(status);
    ui->doVertical->setEnabled(status);
    ui->doExpansion->setEnabled(status);
    ui->doGrayscale->setEnabled(status);
    ui->doHorizontal->setEnabled(status);
    ui->doMovie_frame->setEnabled(status);
    ui->doBinaryzation->setEnabled(status);
    ui->doFlower_frame->setEnabled(status);
    ui->doMeida_Filter->setEnabled(status);
    ui->do_adjust_left->setEnabled(status);
    ui->doClassic_frame->setEnabled(status);
    ui->do_adjust_right->setEnabled(status);
    ui->doBrightness->setEnabled(status);
    ui->doContour_extraction->setEnabled(status);
    ui->doZoom->setEnabled(status);
    ui->doClose->setEnabled(status);
    ui->doRotate_left->setEnabled(status);
    ui->doRotate_right->setEnabled(status);

    ui->do_AutoPS_wec->setEnabled(status);
    ui->doDrawLine->setEnabled(status);
    ui->doDrawWhitePolygon->setEnabled(status);

    ui->doOpenYaml->setEnabled(status);
    ui->doOpenJson->setEnabled(status);
    ui->doOpenJson_multiMPath->setEnabled(status);
    ui->do_draw_path->setEnabled(status);
    ui->doDrawPathContinue->setEnabled(status);
    ui->do_fill_path->setEnabled(status);
    ui->do_draw_next_mpath->setEnabled(status);
    ui->doCleanJson->setEnabled(status);
    ui->doCleanAllMPath->setEnabled(status);
    ui->doSaveJson->setEnabled(status);
    ui->doDrawTraj_png->setEnabled(status);

    ui->do_draw_polygons_area->setEnabled(status);
    ui->do_draw_ploygons->setEnabled(status);
    ui->do_adjust_polygons_area->setEnabled(status);
    ui->do_load_polygons_area->setEnabled(status);
    ui->do_clean_polygons_area->setEnabled(status);
    ui->do_save_polygons_area->setEnabled(status);
    ui->do_draw_full_covered_path->setEnabled(status);

    ui->do_selectMultiPoint_polygen->setEnabled(status);
    ui->do_clean_pointSelect_area->setEnabled(status);
    ui->do_addAttribute->setEnabled(status);

    ui->do_draw_functional_area->setEnabled(status);
    ui->do_clean_current_functional_area->setEnabled(status);
    ui->do_clean_all_functional_area->setEnabled(status);
    ui->do_draw_next_functional_area->setEnabled(status);
    ui->do_save_functional_area->setEnabled(status);

}

//在绘物体的过程中，将所有按钮设为不可用
void MainWindow::setActionStatus_all(bool status)
{
    ui->doOpen->setEnabled(status);
    ui->doSave->setEnabled(status);
    ui->doSave_As->setEnabled(status);
    ui->doClose->setEnabled(status);

    ui->doRestore->setEnabled(status);
    ui->do_adjust->setEnabled(status);
    ui->do_AutoPS_wec->setEnabled(status);
    ui->doDrawLine->setEnabled(status);
    ui->doDrawWhitePolygon->setEnabled(status);

    ui->doOpenYaml->setEnabled(status);
    ui->doOpenJson->setEnabled(status);
    ui->doOpenJson_multiMPath->setEnabled(status);
    ui->do_draw_path->setEnabled(status);
    ui->doDrawPathContinue->setEnabled(status);
    ui->do_fill_path->setEnabled(status);
    ui->do_draw_next_mpath->setEnabled(status);
    ui->doCleanJson->setEnabled(status);
    ui->doCleanAllMPath->setEnabled(status);
    ui->doSaveJson->setEnabled(status);
    ui->doDrawTraj_png->setEnabled(status);

    ui->do_draw_polygons_area->setEnabled(status);
    ui->do_draw_ploygons->setEnabled(status);
    ui->do_adjust_polygons_area->setEnabled(status);
    ui->do_load_polygons_area->setEnabled(status);
    ui->do_clean_polygons_area->setEnabled(status);
    ui->do_save_polygons_area->setEnabled(status);
    ui->do_draw_full_covered_path->setEnabled(status);

    ui->doOpenJsonExclusively->setEnabled(status);
    ui->do_rtk_full_cover_path->setEnabled(status);

    ui->do_selectMultiPoint_polygen->setEnabled(status);
    ui->do_clean_pointSelect_area->setEnabled(status);
    ui->do_addAttribute->setEnabled(status);

    ui->do_draw_functional_area->setEnabled(status);
    ui->do_clean_current_functional_area->setEnabled(status);
    ui->do_clean_all_functional_area->setEnabled(status);
    ui->do_draw_next_functional_area->setEnabled(status);
    ui->do_save_functional_area->setEnabled(status);

    ui->doAbout->setEnabled(status);
}

//  清理图片
void MainWindow::cleanImage()
{
    //清理
    qDebug()<<"1";
    on_doCleanAllMPath_triggered();
    qDebug()<<"m_path:"<<(void*)m_path;
    //初始化m_path轨迹大小
    m_path_width = 1;
    m_path->setPathWidth(m_path_width);
    //清空选取区域
    initMapSelect();
    initPointSelect();
    on_do_clean_all_functional_area_triggered();
    qDebug()<<"items:"<<pgmScene->items();
    pgmScene->clear();  //Removes and deletes all items from the scene, but otherwise leaves the state of the scene unchanged.
    qDebug()<<"2";

    //重置
    ui->pgmGraphicsView->resetTransform();
    pgmScene->setSceneRect(QRectF());

    //重置信息
    info = nullptr;
    imgSrc.release();
    imgTmp.release();
    pgmPixmapItem = nullptr;
    isBlankMap = false;

    //更新statusBar
    if (size)
    {
        delete size;
        size = new QLabel;
        ui->statusbar->addPermanentWidget(size);
    }
    if(pixel){
        delete pixel;
        pixel = new QLabel;
        ui->statusbar->addPermanentWidget(pixel);
    }
    //还原标题
    this->setWindowTitle(WINDOW_TITLE);

    //还原地图信息
    resolution = 0;
    origin_x = 0;
    origin_y = 0;

    //控件设为不可用
    setActionStatus(false);
}

//    打开图片
void MainWindow::on_doOpen_triggered()
{
    if(flag){
        if(QMessageBox::Yes==QMessageBox::question(this,tr("尚未保存"),tr("是否返回操作？"),QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes)){
            return;
        }
    }
    /*
     *TODO:第一次打开桌面选择,第二次打开上次目录
     */
    if(ConstImage==""){
        ConstImage = getUserPath();
    }
    QString imagePath = QFileDialog::getOpenFileName(this, tr("Open image"), ConstImage,
                                                     tr("Image PGM (*.pgm);;"
                                                         "All Images (*.bpm *.jpg *.jpeg *.png *.tif *.pcx *.tga *.svg *.ico);;"
                                                        "All Files (*);;"
                                                        "Image BPM (*.bpm);; Image JPG (*.jpg);;"
                                                        "Image JPEG (*.jpeg);;"
                                                        "Image PNG (*.png);;"));
    ConstImage = imagePath;

    //TODO:多种图片格式
    if ( !imagePath.endsWith( ".pgm" ) ) ui->statusbar->showMessage( "非pgm图片，注意图片格式，图片处理可能出错！" );

    //如果这个目录存在
    if (!imagePath.isEmpty())//首先地址
    {
        QFile file(imagePath);
        //指定打开文件模式
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr(WINDOW_ERROR), tr("不能打开这个文件！"));
            return;
        }

        //将之前的图片清除掉
        cleanImage();

        // 更新图片信息
        info = new QFileInfo(imagePath);

        //显示图片
//        QPixmap pgmPixmap(imagePath); //之前的加载方式
        imgSrc = cv::imread(UTF8ToGBK(imagePath), CV_8U);

        file.close();

        if(!imgSrc.data)
        {
            QMessageBox::critical( this, tr(WINDOW_ERROR), tr("No image data \ncheck the <Image_Path>"));
            return;
        }

        imgTmp = imgSrc.clone();

        QPixmap pgmPixmap = CvMat8UToQPixmap(imgSrc);
        pgmPixmapItem = pgmScene->addPixmap(pgmPixmap);//设置图片
        pgmScene->setSceneRect(QRectF(pgmPixmap.rect()));//更新坐标

        //调整使得图片适应边框
        on_do_adjust_triggered();

        //更改软件标题
        this->setWindowTitle(info->fileName() + " - Map Tool");

        //设置操作可用
        setActionStatus(true);

        //statusBar更新信息
        double size_h = info->size()*1.0/1024;
        QString size_type = " KB";
        if(size_h >= 1024){
            size_h /= 1024;
            size_type = " MB";
        }
        size->setText(QString::number(size_h, 'f', 2) + size_type);
        pixel->setText(QString::number(pgmPixmapItem->pixmap().width())
                       + " x " + QString::number(pgmPixmapItem->pixmap().height()));
        flag = false;

        //如果图片不是pgm格式，则不继续加载json&yaml文件
        if ( !imagePath.endsWith(".pgm") ) return;

        QMessageBox msgBox;
        msgBox.setText("pgm图片已打开，是否加载同一路径下的同名yaml文件?   ");
        msgBox.setInformativeText("若不加载yaml文件，则不能设置轨迹宽度与保存轨迹!");
        QPushButton* yesButton = msgBox.addButton(tr("是"), QMessageBox::AcceptRole);
        QPushButton* moreButton = msgBox.addButton(tr("选择文件"), QMessageBox::ActionRole);
//        QPushButton* noButton = msgBox.addButton(tr("否"), QMessageBox::RejectRole);
        msgBox.addButton(tr("否"), QMessageBox::RejectRole);

        msgBox.exec();

        if(msgBox.clickedButton() == yesButton)
        {
            yamlPath = imagePath.left(imagePath.lastIndexOf(".")) + ".yaml";
            openYaml(yamlPath);
        }
        else if ( msgBox.clickedButton() == moreButton )
        {
            yamlPath = QFileDialog::getOpenFileName(this, tr("Open Yaml"), ConstImage.left(ConstImage.lastIndexOf(".")) + ".yaml",
                                                    tr("Yaml FIles (*.yaml);;"));
            openYaml(yamlPath);
        }
        else
        {
            return;
        }
    }
}

//打开json文件
void MainWindow::on_doOpenJson_triggered()
{
    if(ConstImage == ""){
        ConstJson = getUserPath();
    }
    else{//230828
        ConstJson = ConstImage.left(ConstImage.lastIndexOf(".")).left(ConstImage.lastIndexOf("/"));
    }
    QString jsonPath = QFileDialog::getOpenFileName(this, tr("Open Json"), ConstJson, tr("Json FIles (*.json);;"));
//    ConstJson = jsonPath;//230828
    QFile file(jsonPath);
    //指定打开文件模式
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::critical(this, tr(WINDOW_ERROR), tr("不能打开json文件！\n请检查路径！"));
        return;
    }
    //读取文件的全部内容
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    jsonContent = stream.readAll();
    //TODO_json:管理能否操作json相关的Action的变量

    file.close();

    qDebug()<<"json_file read";
    ui->statusbar->showMessage("json file is opened : " + jsonPath, 3000);

    // QJsonParseError类用于在JSON解析期间报告错误。
    QJsonParseError jsonError;
    // 将json解析为UTF-8编码的json文档，并从中创建一个QJsonDocument。
    // 如果解析成功，返回QJsonDocument对象，否则返回null
    QJsonDocument doc = QJsonDocument::fromJson(jsonContent.toUtf8(), &jsonError);
    // 判断是否解析失败
    if (jsonError.error != QJsonParseError::NoError && !doc.isNull()) {
        qDebug() << "Json格式错误！" << jsonError.error;
        QMessageBox::critical(this, tr(WINDOW_ERROR), tr("Json格式错误！\n请检查Json文件是否被破坏"));
        return;
    }

    qDebug()<<"json 格式正确";
    m_path->clear();
    pgmScene->addItem(m_path);

    std::set<QString> KEY { "pixel_x", "pixel_y", "x", "y", "yaw", "welt", "bypass", "seismic", "turnAround", "trash", "charge", "back", "clean_flag" };

    //获取根{}
    QJsonObject rootObj = doc.object();
    //解析数组(points)
    QJsonValue pointsValue = rootObj.value("points");
    //判断是否是Array类型
    if ( pointsValue.type() == QJsonValue::Array )
    {
        //转换为QJsonArray类型
        QJsonArray pointsArray = pointsValue.toArray();

        for ( int i = 0; i < pointsArray.size(); i ++ )
        {
            QJsonValue pointValue = pointsArray.at(i);
            // 判断是否是object类型
            if (pointValue.type() == QJsonValue::Object) {
                // 转换为QJsonObject类型
                QJsonObject pointObj = pointValue.toObject();

                QJsonValue point_pixel_x = pointObj.value( "pixel_x" );
                qreal pixel_x = point_pixel_x.toDouble();
                QJsonValue point_pixel_y = pointObj.value( "pixel_y" );
                qreal pixel_y = point_pixel_y.toDouble();

                bool has_attr = false;
                int arr[8] = {0,0,0,0,0,0,0,0};
                if ( pointObj.value("welt") != QJsonValue::Undefined && (has_attr = true) )
                    arr[0] = pointObj.value("welt").toInt();
                if ( pointObj.value("bypass") != QJsonValue::Undefined && (has_attr = true) )
                    arr[1] = pointObj.value("bypass").toInt();
                if ( pointObj.value("seismic") != QJsonValue::Undefined && (has_attr = true) )
                    arr[2] = pointObj.value("seismic").toInt();
                if ( pointObj.value("turnAround") != QJsonValue::Undefined && (has_attr = true) )
                    arr[3] = pointObj.value("turnAround").toInt();
                if ( pointObj.value("trash") != QJsonValue::Undefined && (has_attr = true) )
                    arr[4] = pointObj.value("trash").toInt();
                if ( pointObj.value("charge") != QJsonValue::Undefined && (has_attr = true) )
                    arr[5] = pointObj.value("charge").toInt();
                if ( pointObj.value("back") != QJsonValue::Undefined && (has_attr = true) )
                    arr[6] = pointObj.value("back").toInt();
                if ( pointObj.value("clean_flag") != QJsonValue::Undefined && (has_attr = true) )
                    arr[7] = pointObj.value("clean_flag").toInt();

                std::map<QString, QString> self_df;
                QStringList keys = pointObj.keys();
                for ( auto it = keys.begin(); it != keys.end(); it ++ )
                {
                    if ( !KEY.count(*it) && (has_attr = true) )
                        self_df[*it] = QString::fromStdString( std::to_string( pointObj.value( *it ).toInt() ) );
                }

                if ( has_attr )
                    m_path->addNextPoint( QPointF( pixel_x, pixel_y ), arr, self_df );
                else
                    m_path->addNextPoint( QPointF( pixel_x, pixel_y ) );
            }
        }
    }
//    drawPath(m_path);
    if ( resolution == 0 )
    {
        on_doOpenYaml_triggered();
    }
}

//保存json文件
void MainWindow::on_doSaveJson_triggered()
{

    qDebug()<<"on_doSaveJson_triggered";
    on_do_fill_path_triggered();

    if ( resolution == 0 )
    {
//        on_doOpenYaml_triggered();
        QMessageBox::critical(this,tr(WINDOW_ERROR),tr("未加载yaml文件或者未加载rtk-json文件"));
    }
    if ( resolution == 0 )
    {
        return;
    }

    qDebug()<<"pointsNumber : "<<m_path->pointsNumber();
    //构建points数组
    QJsonArray pointsArray;
    for ( auto& it : m_path_list )
    {
        int n = it->pointsNumber();
        qreal pre_yaw = 0;
        for ( int i = 0; i < n; i ++ )
        {
            QPointF point_pixel = it->getPoint(i);
            attribute *attr = it->getElement(i)->attr;
            QJsonObject pointObj;
            qreal pixel_x = point_pixel.rx(),
                pixel_y = point_pixel.ry();
            int height = pgmPixmapItem->pixmap().height();
            pointObj.insert( "pixel_x", pixel_x );
            pointObj.insert( "pixel_y", pixel_y );
            pointObj.insert( "x", pixel_x * resolution + origin_x );
            pointObj.insert( "y", ( height - 1 - pixel_y ) * resolution + origin_y );
            pointObj.insert( "yaw", ( i < n - 1 ) ? pre_yaw = atan2( pixel_y - it->getPoint( i + 1 ).ry() , it->getPoint( i + 1 ).rx() - pixel_x ) : pre_yaw );
            if ( attr )
            {
                pointObj.insert( "welt", attr->welt );
                pointObj.insert( "bypass", attr->bypass );
                pointObj.insert( "seismic", attr->seismic );
                pointObj.insert( "turnAround", attr->turnAround );
                pointObj.insert( "trash", attr->trash );
                pointObj.insert( "charge", attr->charge );
                pointObj.insert( "back", attr->back );
                pointObj.insert( "clean_flag", attr->clean_flag );
                for ( auto& it : attr->self_define )
                {
                    bool ok;
                    int value = it.second.toInt(&ok);
                    if (!ok) {
                        value = -1;
                    }
                    pointObj.insert(it.first, value);
//                    pointObj.insert( it.first, std::stoi( it.second.toStdString() ) );
                }
            }
            pointsArray.append( pointObj );
        }
    }
    int n = m_path->pointsNumber();
    qreal pre_yaw = 0;
    for ( int i = 0; i < n; i ++ )
    {
        QPointF point_pixel = m_path->getPoint(i);
        attribute *attr = m_path->getElement(i)->attr;
        QJsonObject pointObj;
        qreal pixel_x = point_pixel.rx(),
            pixel_y = point_pixel.ry();
        int height = pgmPixmapItem->pixmap().height();
        pointObj.insert( "pixel_x", pixel_x );
        pointObj.insert( "pixel_y", pixel_y );
        pointObj.insert( "x", pixel_x * resolution + origin_x );
        pointObj.insert( "y", ( height - 1 - pixel_y ) * resolution + origin_y );
        pointObj.insert( "yaw", ( i < n - 1 ) ? pre_yaw = atan2( pixel_y - m_path->getPoint( i + 1 ).ry() , m_path->getPoint( i + 1 ).rx() - pixel_x ) : pre_yaw );
        if ( attr )
        {
            pointObj.insert( "welt", attr->welt );
            pointObj.insert( "bypass", attr->bypass );
            pointObj.insert( "seismic", attr->seismic );
            pointObj.insert( "turnAround", attr->turnAround );
            pointObj.insert( "trash", attr->trash );
            pointObj.insert( "charge", attr->charge );
            pointObj.insert( "back", attr->back );
            pointObj.insert( "clean_flag", attr->clean_flag );
            for ( auto& it : attr->self_define )
            {
                bool ok;
                int value = it.second.toInt(&ok);
                if (!ok) {
                    value = -1;
                }
                pointObj.insert(it.first, value);
//                pointObj.insert( it.first, std::stoi( it.second.toStdString() ) );
            }
        }
       // else  //未定义功能的部分怎么写json--->再确定
       // {
       //     pointObj.insert( "welt", 0 );
       //     pointObj.insert( "bypass", 0 );
       //     pointObj.insert( "seismic", 0 );
       //     pointObj.insert( "turnAround", 0 );
       //     pointObj.insert( "trash", 0 );
       //     pointObj.insert( "charge", 0 );
       //     pointObj.insert( "back", attr->back );
       //     pointObj.insert( "clean_flag", attr->clean_flag );
       // }
        pointsArray.append( pointObj );
    }
    //构建json对象
    QJsonObject json;
    json.insert( "points", QJsonValue( pointsArray ) );
    json.insert( "post_process", 0 );
    //将json设置为文档的主要对象
    QJsonDocument doc;
    doc.setObject( json );

    // 重写打开文件，覆盖原有文件，达到删除文件全部内容的效果
//    QString test = ConstImage.left(ConstImage.lastIndexOf(".")) + "_1.json";
//    QFile writeFile( test );
    if(ConstJson==""){
        ConstJson = ConstImage;
    }
    qDebug()<<"jsonfile1";
    ConstJson = QFileDialog::getSaveFileName(this, tr("Save json"), ConstJson,
                                                   tr("file Json (*.json);;"
                                                      "All files (*);;"));
    qDebug()<<"jsonfile2";
    QFile writeFile( ConstJson );
    if ( !writeFile.open( QFile::WriteOnly | QFile::Truncate ) ) {
        qDebug() << "can't open : " << ConstJson;
        return;
    }

    // 将修改后的内容写入文件
    QTextStream wirteStream( &writeFile );
    wirteStream.setCodec( "UTF-8" );		// 设置读取编码是UTF8
    wirteStream << doc.toJson();		// 写入文件
    writeFile.close();					// 关闭文件
    //
    ui->statusbar->showMessage( "json文件已保存 : " + ConstJson, 3000 );
}

//清除当前轨迹
void MainWindow::on_doCleanJson_triggered()
{
    if (m_path && m_path->scene() == pgmScene)
    {
        pgmScene->removeItem(m_path);
        qDebug()<<"m_path removed";
    }
    m_path->clear();
}

// 关闭图片
void MainWindow::on_doClose_triggered()
{
    if(flag){
        if(QMessageBox::Yes==QMessageBox::question(this,tr("尚未保存"),tr("是否返回操作再关闭？"),QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes)){
            return;
        }
    }
    //直接清理图片W
    cleanImage();
    flag = false;
}

//保存图片
void MainWindow::on_doSave_triggered()
{
    //首先将图片与本地图片进行替换
     pgmPixmapItem->pixmap().save(ConstImage);

    //更新底部statusBar
    if (size)
    {
        delete size;
        size = new QLabel;
        ui->statusbar->addPermanentWidget(size);
    }
    if(pixel){
        delete pixel;
        pixel = new QLabel;
        ui->statusbar->addPermanentWidget(pixel);
    }

    //重新获取图片信息
    delete info;
    info = new QFileInfo(ConstImage);

    //更新底部statusBar
    double size_h=info->size()*1.0/1024;
    QString size_type=" KB";
    if(size_h>=1024){
        size_h/=1024;
        size_type=" MB";
    }
    size->setText(QString::number(size_h, 'f', 2)+size_type);
    pixel->setText(QString::number(pgmPixmapItem->pixmap().width())
                   + " x " + QString::number(pgmPixmapItem->pixmap().height()));

    //调整图片适应窗口
    on_do_adjust_triggered();
    flag = false;
}

//另存为//这个不对，还得 改改//已改逻辑为导出图片
void MainWindow::on_doSave_As_triggered()
{
    //直接用原始图片作为地址和名字
    QString newPath = QFileDialog::getSaveFileName(this, tr("Save image"), ConstImage,
                                                   tr("Image PGM (*.pgm);;"
                                                      "Image BPM (*.bpm);;"
                                                      "Image JPG (*.jpg);;"
                                                      "Image JPEG (*.jpeg);;"
                                                      "Image PNG (*.png);;"
                                                      "Image SVG (*.svg);;"
                                                      "Image ICO (*.ico);;"
                                                      "Image TIF (*.tif);;"
                                                      "Image PCX (*.pcx);;"
                                                      "Image TGA (*.tga);;"
                                                      "All files (*);;"));

    //检测是否能够保存 逻辑同打开
    if (!newPath.isEmpty()) {
        QFile file(newPath);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(this, tr(WINDOW_ERROR), tr("不能保存这个文件！"));
            return;
        }
        pgmPixmapItem->pixmap().save(newPath);
        flag = false;
        file.close();
    }
}

//退出
void MainWindow::on_doExit_triggered()
{
    //检查是否更改
    if(flag){
        if(QMessageBox::Yes==QMessageBox::question(this,tr("尚未保存"),tr("是否返回操作再退出？"),QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes)){
            return;
        }
    }
    cleanImage();
    qApp->quit();
}

//恢复图像
void MainWindow::on_doRestore_triggered()
{
    //获取初始图片
    QPixmap pgmPixmap =CvMat8UToQPixmap(imgSrc);
    imgTmp = imgSrc.clone();
    //更新图片
    updateImage(pgmPixmap);
    //调整图像自适应
//    on_do_adjust_triggered();
    flag = false;
}

//调整图片适应
void MainWindow::on_do_adjust_triggered()
{
    //恢复
    ui->pgmGraphicsView->resetTransform();
    // 计算场景矩形以包含所有项
    QRectF itemsRect = pgmScene->itemsBoundingRect();
    // 设置视图的场景矩形大小
    pgmScene->setSceneRect(QRectF(itemsRect.left()-1000,itemsRect.top()-1000,itemsRect.width()+2000,itemsRect.height()+2000));

    //图
    int height = itemsRect.height();
    int width = itemsRect.width();
    int max_height = ui->pgmGraphicsView->height();
    int max_width = ui->pgmGraphicsView->width();

    //计算
//    int pic_size = qMax(width,height);
//    int max_size = qMin(max_width,max_height) - 5;
//    double val = (max_size*1.0)/pic_size;
    double val = 0.95 * qMin( ( max_height - 5 ) * 1.0 / height , ( max_width - 5 ) * 1.0 / width );

    ui->pgmGraphicsView->scale(val,val);
    ui->pgmGraphicsView->centerOn(itemsRect.center());
}

//关于
void MainWindow::on_doAbout_triggered()
{
    AboutDialog *dialog = new AboutDialog();
    dialog->setAttribute( Qt::WA_QuitOnClose, false );
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

//自动修图
void  MainWindow::on_do_AutoPS_wec_triggered()
{
    cv::Mat imgMedian;
    cv::medianBlur(imgTmp, imgMedian, 3);
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::erode(imgMedian, imgMedian, kernel);    //膨胀
    cv::dilate(imgMedian, imgMedian, kernel);   //腐蚀
//    cv::medianBlur(imgMedian, imgMedian, 3);

    imgTmp = imgMedian;
    //获取tmp图片
    QPixmap pgmPixmap =CvMat8UToQPixmap(imgTmp);
    //更新图片
    updateImage(pgmPixmap);
    flag = true;
}

/******************************************************************************
 *                              获得当前用户的桌面目录
 *****************************************************************************/
QString MainWindow::getUserPath()
{
    QString userPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    return userPath;
}

//测试按钮
void MainWindow::on_actiontest_triggered()
{
//    qDebug()<<"PI: "<<M_PI;

//    QString yamlPath = ConstImage.left(ConstImage.lastIndexOf(".")) + ".yaml";
//    YAML::Node yamlContent = YAML::LoadFile(yamlPath.toStdString());
//    if(yamlContent["resolution"])
//    {
//        double a = yamlContent["resolution"].as<double>();
//        qDebug()<<"resolution: "<<a;
//    }else{
//        qDebug()<<"resolution not found";
//    }

//    MapSelectArea *test = new MapSelectArea(pgmScene, this);
    /*
    MapSelectItem *test = new MapSelectItem(QPoint(1000,1000));
    pgmScene->addItem(test);*/

//    test *aaa = new test(QPointF(1000, 1000), 1000, 500);
//    pgmScene->addItem(aaa);

//    AttributeAddDialog *aad = new AttributeAddDialog(this);
//    connect(aad, SIGNAL(sendData(std::string)), this, SLOT(printAttribute(std::string)));
//    aad->setAttribute(Qt::WA_QuitOnClose, false);
//    aad->setAttribute(Qt::WA_DeleteOnClose);
//    aad->show();

//    //重置
//    ui->pgmGraphicsView->resetTransform();
//    updateImage(pgmPixmapItem->pixmap());
    cleanImage();
    pgmScene->setSceneRect(QRectF());
}

void MainWindow::printAttribute(std::string msg)
{
    qDebug()<<QString::fromStdString(msg);
}
//自由绘制轨迹
void MainWindow::on_do_draw_path_triggered()
{

    qDebug()<<'a';
    m_path->clear();
    qDebug()<<'b';
    pgmScene->startCreate();
//    QPointF p(1000, 1000);
//    qDebug()<<"b1";
//    QList<QPointF> m_list;
//    qDebug()<<"b2";
//    m_list.push_back(p);
//    qDebug()<<"b3";
//    pgmScene->emit updatePoint(p, m_list, false);
    qDebug()<<'c';
    setActionStatus(false);
    qDebug()<<"d:"<<m_path->scene()<<(void*)m_path;
    pgmScene->addItem(m_path);
    qDebug()<<"e:"<<m_path->scene();
    ui->statusbar->showMessage( "左键点击加入点，右键点击结束绘制" );
    qDebug()<<'f';

}

//填充全部轨迹
void MainWindow::on_do_fill_path_triggered()
{
    qDebug()<<"11："<<m_path_list.size();
    for ( auto& it : m_path_list )
    {
//        qDebug()<<"12";
        if ( it->pointsNumber() < 2 ) continue;
        for ( int i = 0 ; i < it->pointsNumber() - 1 ; i ++ )
        {
//            qDebug()<<"13";
            QPointF cur = it->getPoint( i ),
                next = it->getPoint( i + 1 );
//            qDebug()<<"14";
            if ( ( ( cur.x() - next.x() ) * ( cur.x() - next.x() ) + ( cur.y() - next.y() ) * ( cur.y() - next.y() ) ) <= 0.0625 / resolution /resolution )
                it->deletePoint(it->getElement(i+1));
            else if ( ( ( cur.x() - next.x() ) * ( cur.x() - next.x() ) + ( cur.y() - next.y() ) * ( cur.y() - next.y() ) ) >= 1.5625 / resolution /resolution )
                it->insertPoint( it->getElement(i), 1/resolution );
//            qDebug()<<"15";
//            qDebug()<<"fill_point: "<<i;
        }
    }
//    qDebug()<<"21";
    if ( m_path->pointsNumber() < 2 ) return;
//    qDebug()<<"22";
    for ( int i = 0 ; i < m_path->pointsNumber() - 1 ; i ++ )
    {
//        qDebug()<<"23";
        QPointF cur = m_path->getPoint( i ),
            next = m_path->getPoint( i + 1 );
//        qDebug()<<"24";
        if ( ( ( cur.x() - next.x() ) * ( cur.x() - next.x() ) + ( cur.y() - next.y() ) * ( cur.y() - next.y() ) ) <= 0.0625 / resolution /resolution )
            m_path->deletePoint(m_path->getElement(i+1));
        else if ( ( ( cur.x() - next.x() ) * ( cur.x() - next.x() ) + ( cur.y() - next.y() ) * ( cur.y() - next.y() ) ) >= 1.5625 / resolution /resolution )
            m_path->insertPoint( m_path->getElement(i), 1/resolution );
//        qDebug()<<"25";
//        qDebug()<<"fill_point: "<<i;
    }
}

//func_openYAML
void MainWindow::openYaml(QString yamlPath)
{
    std::fstream fs;
    fs.open( UTF8ToGBK(yamlPath) );
    if( !fs.is_open() )
    {
        QMessageBox::warning(this, tr(WINDOW_ERROR), tr("读取yaml失败，请检查文件路径！\n"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    std::string str, yamlContent;
    while( std::getline(fs,str) ){
        yamlContent += str + ' ';
    }
    fs.close();

    std::istringstream iss;
    iss.str( yamlContent );

    while ( iss >> str )
    {
        if ( str == "resolution:" )
        {
            iss >> resolution;
        }
        else if ( str == "origin:" )
        {
            std::string tmp;
            iss >> tmp;
            origin_x = std::stod( tmp.substr( 1, tmp.size() - 2 ) );
            iss >> tmp;
            origin_y = std::stod( tmp.substr( 0, tmp.size() - 1 ) );
            iss >> tmp;
            yaw = std::stod( tmp.substr( 0, tmp.size() - 1 ) );
        }
    }
    qDebug()<<"resolution: "<<resolution<<" origin_x: "<<origin_x<<" origin_y: "<<origin_y;
    if ( resolution == 0 )
    {
        QMessageBox::warning( this, tr(WINDOW_ERROR), tr("请检查yaml文件数据，读取resolution与origin值失败" ) );
        return;
    }
    ui->statusbar->showMessage("yaml file is opened : " + yamlPath, 3000);

    startRect = new startRectItem(QPointF(-origin_x / resolution, pgmPixmapItem->pixmap().height() - 1 + origin_y / resolution),
                                  1.4 / resolution, 1.7 / resolution, - yaw * 180.0 / M_PI );
    pgmScene->addItem(startRect);
}

//打开yaml文件
void MainWindow::on_doOpenYaml_triggered()
{

    QMessageBox msgBox;
    msgBox.setText("是否加载同一路径下的同名yaml文件?");
    //TODO_ymal:管理能否操作yaml相关的Action的变量
    msgBox.setInformativeText("若不加载yaml文件，则不能进行轨迹宽度设置、轨迹保存!");
    QPushButton* yesButton = msgBox.addButton(tr("是"), QMessageBox::AcceptRole);
    QPushButton* moreButton = msgBox.addButton(tr("选择文件"), QMessageBox::ActionRole);
//        QPushButton* noButton = msgBox.addButton(tr("否"), QMessageBox::RejectRole);
    msgBox.addButton(tr("取消"), QMessageBox::RejectRole);

    msgBox.exec();

    if(msgBox.clickedButton() == yesButton)
    {
        yamlPath = ConstImage.left(ConstImage.lastIndexOf(".")) + ".yaml";
    }
    else if ( msgBox.clickedButton() == moreButton )
    {
        yamlPath = QFileDialog::getOpenFileName(this, tr("Open Yaml"), ConstImage, tr("Yaml FIles (*.yaml);;"));
    }
    else
    {
        return;
    }

    openYaml(yamlPath);
}

//func_设置轨迹宽度
void MainWindow::setPathWidth(QString str)
{
    int width = std::stoi(str.toStdString());
    if ( resolution == 0 )
    {
        QMessageBox::critical(this, tr(WINDOW_ERROR), tr("未加载yaml或未获得地图信息"));
        return;
    }
    m_path_width = width / 100.0 / resolution;

    for ( auto& it : m_path_list )
        it->setPathWidth(m_path_width);
    m_path->setPathWidth(m_path_width);
}

//func_绘制虚拟墙
/*
void MainWindow::drawVirtualWall( QList<QPointF> m_list )
{
    QPointF p_1 = m_list[0];
    QPointF p_2 = m_list[1];

    cv::line( imgTmp, cv::Point2d( p_1.x(), p_1.y() ), cv::Point2d( p_2.x(), p_2.y() ), 0, 3 );
    //获取tmp图片
    QPixmap pgmPixmap =CvMat8UToQPixmap(imgTmp);
    //更新图片
    updateImage(pgmPixmap);
    flag = true;

    setActionStatus(true);
    ui->statusbar->showMessage("");
}
*/
//231015-腾飞
void MainWindow::drawVirtualWall( QList<QPointF> m_list )
{
    //    QPointF p_1 = m_list[0];
    //    QPointF p_2 = m_list[1];

    //    cv::line( imgTmp, cv::Point2d( p_1.x(), p_1.y() ), cv::Point2d( p_2.x(), p_2.y() ), 0, 3 );
    QPointF pmin(-1,-1);
    if(m_list[m_list.size()-1]!=pmin){
        QPointF p_wei[m_list.size()];
        for(int i=0;i<m_list.size();++i){
            p_wei[i]=m_list[i];
        }
        for(int i=0;i<m_list.size()-1;++i){
            cv::line( imgTmp, cv::Point2d( p_wei[i].x(), p_wei[i].y() ), cv::Point2d( p_wei[i+1].x(), p_wei[i+1].y() ), 0, 3 );
        }

        //获取tmp图片
        QPixmap pgmPixmap =CvMat8UToQPixmap(imgTmp);
        //更新图片
        updateImage(pgmPixmap);
    }
    else{
        flag = true;
        setActionStatus(true);
        ui->statusbar->showMessage("");
    }
}

//绘制虚拟墙
void MainWindow::on_doDrawLine_triggered()
{

    pgmScene->drawLine2img();
    setActionStatus(false);
    ui->statusbar->showMessage( "左键点击加入点，右键点击结束绘制" );

}

//保存轨迹到png
void MainWindow::on_doDrawTraj_png_triggered()
{
    cv::Mat pngTraj = imgTmp;

    cv::cvtColor( pngTraj, pngTraj, cv::COLOR_GRAY2BGR );

    cv::Scalar color = cv::Scalar(255,0,0);

    for ( auto& it : m_path_list )
    {
        for ( int i = 1; i < it->pointsNumber(); i ++ )
        {
            QPointF last_p = it->getPoint( i - 1 );
            QPointF curr_p = it->getPoint( i );
            cv::circle( pngTraj, cv::Point2d( curr_p.x(), curr_p.y() ), 3, color, 4);
            cv::line( pngTraj, cv::Point2d( last_p.x(), last_p.y() ), cv::Point2d( curr_p.x(), curr_p.y() ), color, 3 );
        }
    }
    for ( int i = 1; i < m_path->pointsNumber(); i ++ )
    {
        QPointF last_p = m_path->getPoint( i - 1 );
        QPointF curr_p = m_path->getPoint( i );
        cv::circle( pngTraj, cv::Point2d( curr_p.x(), curr_p.y() ), 3, color, 4);
        cv::line( pngTraj, cv::Point2d( last_p.x(), last_p.y() ), cv::Point2d( curr_p.x(), curr_p.y() ), color, 3 );
    }

    QString pngTrajPath;
    if( ConstImage == "" )
    {
        pngTrajPath = ConstImage = getUserPath();
    }
    else
    {
        pngTrajPath = ConstImage.left(ConstImage.lastIndexOf(".")) + ".png";
    }
    pngTrajPath = QFileDialog::getSaveFileName(this, tr("Save image"), pngTrajPath,
                                                       tr("Image PNG (*.png);;"
                                                          "All files (*);;"));

    QFile file( pngTrajPath );
    //指定打开文件模式
    if ( !file.open(QIODevice::WriteOnly )) {
        QMessageBox::critical(this, tr(WINDOW_ERROR), tr("请检查路径！\n当前路径不可写"));
        return;
    }
    cv::imwrite( UTF8ToGBK(pngTrajPath), pngTraj);

    qDebug()<<"file.size()"<<file.size();
    if ( !file.exists() || file.size() == 0 )
    {
        QMessageBox::warning( this, tr(WINDOW_ERROR), tr("文件保存失败\n注意不能包含中文路径"));
    }

    file.close();
}

//绘制多边形区域
void MainWindow::on_do_draw_polygons_area_triggered()
{
    pgmScene->create_mapSelect_polygon();
    pgmScene->addItem( mapSelected );
    setActionStatus(false);
    ui->statusbar->showMessage( "左键点击绘制点，右键点击结束绘制" );
}

//填充区域
void MainWindow::on_do_draw_ploygons_triggered()
{
    if ( mapSelected->scene() == pgmScene )
        pgmScene->removeItem(mapSelected);

    std::vector<std::vector<cv::Point>> contours;
    for ( size_t i = 0; i < mapSelected->polygons.size(); i ++ )
    {
        std::vector<cv::Point> pts;
        for ( size_t j = 0; j < mapSelected->polygons[i].size(); j ++ )
        {
            pts.push_back(cv::Point((int)(mapSelected->polygons[i][j]->getPoint().x()), (int)(mapSelected->polygons[i][j]->getPoint().y())));
        }
        contours.push_back(pts);
    }
    cv::drawContours( imgTmp, contours, -1, 0, 2, 8 );
    cv::drawContours( imgTmp, contours, -1, 255, cv::FILLED, 8 );

    //获取tmp图片
    QPixmap pgmPixmap =CvMat8UToQPixmap(imgTmp);
    //更新图片
    updateImage(pgmPixmap);
    flag = true;
}

//调整多边形区域
void MainWindow::on_do_adjust_polygons_area_triggered()
{
    pgmScene->addItem(mapSelected);
}

//绘制覆盖式轨迹
void MainWindow::on_do_draw_full_covered_path_triggered()
{
    //检查yaml文件是否加载
    if ( resolution == 0 )
    {
        on_doOpenYaml_triggered();
    }
    if ( resolution == 0 )
    {
        return;
    }
    //判断是否绘制了多边形区域
    if(mapSelected->polygons.size()==0){
        QMessageBox warning;
        warning.setWindowTitle(tr("错误"));
        warning.setText(tr("不存在多边形区域！"));
        warning.exec();
        return;
    }
    //起始点
    QPointF startPoint = QPointF(-origin_x / resolution, pgmPixmapItem->pixmap().height() - 1 + origin_y / resolution);
    //输入框
    coverPathDialog = new CoverPathDialog(startPoint);
    //信号处理
    connect(coverPathDialog, &CoverPathDialog::sendData, this,
            &MainWindow::receiveCoverPathValue );
    connect(coverPathDialog, &CoverPathDialog::sendData_pre, this,
            &MainWindow::show_smooth_coverPathEdges );
    connect(coverPathDialog, &CoverPathDialog::selectStartPoint, this, &MainWindow::selectingStartPoint);
    connect(pgmScene, &BQGraphicsScene::selected_startPoint_accept, coverPathDialog, &CoverPathDialog::setStartPoint);

    //阻塞对话框
//    dialog->exec();

    //非模态对话框
    coverPathDialog->setAttribute( Qt::WA_QuitOnClose, false );
    coverPathDialog->setAttribute(Qt::WA_DeleteOnClose);
//    dialog->setWindowTitle(tr(""));
    coverPathDialog->show();
}

//slot_绘制覆盖式轨迹_选点
void MainWindow::selectingStartPoint()
{
    setActionStatus(false);
    ui->statusbar->showMessage("左键点击选点，右键点击取消");
    pgmScene->select_startPoint();
}

//func_外接圆半径的平方
double outerCircleRadius2(QPointF a, QPointF b, QPointF c)
{
    //R = ab*bc*ca/(4S)
    //R^2 = ab2*bc2*ca2/(16S2)
    //S = (1/2)*(x1y2+x2y3+x3y1-x1y3-x2y1-x3y2);
    double S16 = 4.0 * ( a.x() * b. y() + b.x() * c.y() + c.x() * a.y() - a.x() * c.y() - b.x() * a.y() - c.x() * b.y() )
                * ( a.x() * b. y() + b.x() * c.y() + c.x() * a.y() - a.x() * c.y() - b.x() * a.y() - c.x() * b.y() );
    if ( S16 == 0 ) return DBL_MAX;

    double ab,bc,ca;
    ab = DIS2(a,b);
    bc = DIS2(b,c);
    ca = DIS2(c,a);
    return ab * bc * ca / S16;
}

//func_∠abc的边向量内积
double innerProductABC(QPointF a, QPointF b, QPointF c)
{
    std::vector<double> ba,bc;
    ba.push_back( a.x() - b.x() );
    ba.push_back( a.y() - b.y() );
    bc.push_back( c.x() - b.x() );
    bc.push_back( c.y() - b.y() );

    return std::inner_product( ba.begin(), ba.end(), bc.begin(), 0 );
}

//func_从mapSelected中获得contours2d
std::vector<std::vector<QPointF>> MainWindow::mapSelectedTOcontours2d(MapSelectItem* a)
{
    std::vector<std::vector<QPointF>> contours2d;
    for ( size_t i = 0; i < a->polygons.size(); ++ i )
    {
        std::vector<QPointF> pts;
        for ( size_t j = 0; j < a->polygons[i].size(); ++ j )
        {
            pts.push_back( QPointF( a->polygons[i][j]->getPoint() ) );
        }
        contours2d.push_back(pts);
    }
    return contours2d;
}
//func_修正边缘后的多边形集
std::vector<std::vector<QPointF>> MainWindow::smooth_coverPathEdges(std::vector<std::vector<QPointF>> contours2d, int w, double mr)
{
    double minOuterRadius = ( mr + w / 100.0 / 2.0 ) / resolution;
    double minDivideDis = minOuterRadius * 15.0 / 180.0 * M_PI;

    qDebug()<< "minDivideDis: "<< minDivideDis;
    //将多边形按minDivideDis插值
    for ( size_t i = 0; i < contours2d.size(); ++ i )
    {
        for ( size_t j = 0; j < contours2d[i].size(); ++ j )
        {
            QPointF a(contours2d[i][j]), b;
            if ( j + 1 == contours2d[i].size() ) b = contours2d[i][0];
            else b = contours2d[i][j + 1];

            double dis = sqrt( DIS2(a,b) );
            int k = (int)( dis / minDivideDis );
            for ( int u = 1; u < k; ++ u )
            {
                QPointF p( ( u * b.x() + ( k - u ) * a.x() ) / k, ( u * b.y() + ( k - u ) * a.y() ) / k );
                contours2d[i].insert( contours2d[i].begin() + ( ++ j ), p );
//                qDebug()<<"insert to:"<<j<<" point: "<< p;
            }
        }
    }

    //当此次循环没有进行点的调整，才退出
    bool flag = true;

    int times = 0;
    while(flag)
    {
         ++ times;
        flag = false;
        //生成区域的path
        QPainterPath path;
        for ( size_t i = 0; i < contours2d.size(); ++ i )
        {
            QPolygonF polygon;
            for ( size_t j = 0; j < contours2d[i].size(); ++ j )
            {
                polygon << contours2d[i][j];
            }
            path.addPolygon(polygon);
        }

        for ( size_t i = 0; i < contours2d.size(); ++ i )
        {
            for ( size_t j = 0; j < contours2d[i].size(); ++ j )
            {
                QPointF a,b,c;
                if ( j == 0 ) a = contours2d[i][contours2d[i].size() - 1];
                else a = contours2d[i][j - 1];
                b = contours2d[i][j];
                if ( j == contours2d[i].size() - 1 ) c = contours2d[i][0];
                else c = contours2d[i][j + 1];

                if ( DIS2(a,c) < minDivideDis * minDivideDis )
                {
                    contours2d[i].erase( contours2d[i].begin() + ( j -- ) );
                    flag = true;
//                    qDebug()<<"dis<minDivideDis: "<<i<<","<<j;
                    continue;
                }

                QPointF t( ( a.x() + b.x() + c.x() ) / 3.0 , ( a.y() + b.y() + c.y() ) / 3.0 );
//                if ( path.contains( t ) && ( outerCircleRadius2( a, b, c ) < minOuterRadius * minOuterRadius * 0.99 || innerProductABC(a, b, c) >= 0 ) )
                if ( path.contains( t ) && outerCircleRadius2( a, b, c ) < minOuterRadius * minOuterRadius * 0.99 )
                {
//                    qDebug()<<"point: "<<a<<b<<c;
                    double theta = ( c.y() - a.y() != 0 ) ? atan( ( a.x() - c.x() ) / ( c.y() - a.y() ) ) : atan( ( a.x() - c.x() ) / 1.e-15 );
                    double ED = minOuterRadius - sqrt( minOuterRadius * minOuterRadius - 1.0 / 4.0 * ( DIS2(a,c) ) );
                    QPointF E( ( a.x() + c.x() ) / 2.0 , ( a.y() + c.y() ) / 2.0 );
                    QPointF D1( E.x() + ED * cos(theta), E.y() + ED * sin(theta) ),
                        D2( E.x() - ED * cos(theta), E.y() - ED * sin(theta) );

//                    qDebug()<<"theta: "<<theta;
//                    qDebug()<<"ac DIS: "<<sqrt(DIS2(a,c));
//                    qDebug()<<"minOuterRadius: "<<minOuterRadius;
//                    qDebug()<<"ED: "<<ED;
//                    qDebug()<<"E:"<<E;
//                    qDebug()<<"D: "<<D1<<D2;
                    if ( DIS2(b,D1) > DIS2(b,D2) ) D1 = D2;
//                    qDebug()<<"contours2d["<<i<<"]["<<j<<"]: "<<contours2d[i][j];
                    contours2d[i][j] = D1;
//                    qDebug()<<"changed to : "<<D1;
                    flag = true;
                }
            }
        }
    }
    qDebug()<<"times: "<<times;

    return contours2d;
}

//func_从QPainterPath中提取边缘并生成一个std::vector<std::vector<QPointF>>类型的边缘（contours）
std::vector<std::vector<QPointF>> MainWindow::extractContoursFromPainterPath(const QPainterPath& path)
{
    std::vector<std::vector<QPointF>> contours2d;

    QPainterPathStroker stroker;
    stroker.setWidth(1.0); // 设置边缘宽度

    QPainterPath strokePath = stroker.createStroke(path); // 提取边缘

    // 将边缘路径转换为轮廓
    QList<QPolygonF> polygons = strokePath.toSubpathPolygons();
    for (const QPolygonF& polygon : polygons) {
        std::vector<QPointF> contour2d;
        for (const QPointF& point : polygon) {
            contour2d.push_back(point);
        }
        contours2d.push_back(contour2d);
    }

    return contours2d;
}

//func_从QPainterPath中提取轮廓并生成一个std::vector<std::vector<QPointF>>类型的轮廓（contours）
std::vector<std::vector<QPointF>> contours2dFromPainterPath(const QPainterPath& path)
{
    std::vector<std::vector<QPointF>> contours2d;

    QList<QPolygonF> polygons = path.toFillPolygons();

    for (const QPolygonF& polygon : polygons) {
        std::vector<QPointF> contour2d;
        for (const QPointF& point : polygon) {
            contour2d.push_back(point);
        }
        contours2d.push_back(contour2d);
    }

    return contours2d;
}

//func_从contours2d得到QPainterPath
QPainterPath MainWindow::QPainterPathFromContours2d(std::vector<std::vector<QPointF>> contours2d)
{
    QPainterPath path;
    for ( size_t i = 0; i < contours2d.size(); ++ i )
    {
        QPolygonF polygon;
        for ( size_t j = 0; j < contours2d[i].size(); ++ j )
        {
            polygon << contours2d[i][j];
        }
        path.addPolygon(polygon);
    }
    return path;
}

//slot_绘制覆盖式轨迹
void MainWindow::receiveCoverPathValue(int w, double mr, int type, QPointF startP, qreal startDirection)
{
//    ui->statusbar->showMessage( "正在计算。。。" );
    //初始化graph
    int cols = imgTmp.cols, rows = imgTmp.rows;
    Graph toCover = Graph(rows, cols);
    Graph canAccess = Graph(rows, cols);

    //获得一个修整后的区域
    std::vector<std::vector<QPointF>> contours2d = smooth_coverPathEdges(mapSelectedTOcontours2d(mapSelected), w, mr);
    //预计覆盖区域的path
    QPainterPath path = QPainterPathFromContours2d(contours2d);
    qDebug()<<"path.elementCount:"<<path.elementCount();

    /*
//test->
//1-更快！！
    // 获取开始时间点
    auto start = std::chrono::high_resolution_clock::now();

    //预计覆盖区域的path
    QPainterPath tmp_path = QPainterPathFromContours2d(contours2d);
    // 创建一个 QImage 对象，所有点初始化为白色
    QImage image(cols, rows, QImage::Format_Grayscale8);
    image.fill(Qt::white);

    // 创建 QPainter 对象并将 path 渲染到 image 上
    QPainter painter(&image);
    painter.setPen(QPen(Qt::black, 1));  // 设置画笔颜色和宽度
    painter.fillPath(tmp_path, Qt::black);

    // 将 QImage 转换为 cv::Mat
    cv::Mat mat(image.height(), image.width(), CV_8UC1, (void*)image.bits(), image.bytesPerLine());

    // 获取结束时间点
    auto finish = std::chrono::high_resolution_clock::now();

    // 计算时间差
    std::chrono::duration<double, std::milli> elapsed = finish - start;
    qDebug() << "Elapsed time1: " << elapsed.count() << " ms";
//2
    // 获取开始时间点
    auto tmp_start = std::chrono::high_resolution_clock::now();

    cv::Mat imgTest = toCover.getContours2d(contours2d);
    // 获取结束时间点
    auto tmp_finish = std::chrono::high_resolution_clock::now();
    // 计算时间差
    std::chrono::duration<double, std::milli> tmp_elapsed = tmp_finish - tmp_start;
    qDebug() << "Elapsed time2: " << tmp_elapsed.count() << " ms";
    cv::namedWindow("path", cv::WINDOW_NORMAL);
    cv::imshow("path", imgTest);
    return;
//test-<
    */

    qDebug()<<"开始update";
    /*
    //这里手动update，少循环一边
    QRectF boundingRect = path.boundingRect();
    for ( int i = qMax(0, (int)(boundingRect.top())); i < qMin(rows,(int)(boundingRect.bottom())); ++ i )
    {
        qDebug()<<"row:"<<i;
        for ( int j = qMax(0, (int)(boundingRect.left())); j < qMin(cols, (int)(boundingRect.right())); ++ j )
        {
            //划定车辆可运行区域//划定车辆覆盖范围
            if ( path.contains( QPointF( j, i ) ) ) //这里注意坐标系(x,y)对应(j,i);
            {
                canAccess.graph[i][j] = true;
                toCover.graph[i][j] = true;
            }
        }
    }
    *///以下两行替代
    toCover.updateFromContours2d(contours2d);
    canAccess.updateFromContours2d(contours2d);

    //用于检查graph的形状
//    toCover.show();
//    return;
    //    //->以下是准备工作
    //    //获得起点信息
    //    QPointF(-origin_x / resolution, pgmPixmapItem->pixmap().height() - 1 + origin_y / resolution);
    //    //绘制json点
    //    m_path->addNextPoint( QPointF( pixel_x, pixel_y ) );
    //    //-<以上是准备工作

    QPointF startPoint = startP;
    double angle = 0.0;
    double w_ = w / 100.0 / resolution;
    double mr_ = mr / resolution;
    double step = w_;

    PathSimulate car(startPoint, angle, w_, mr_);

    qDebug()<<"sin(45):"<<sin(M_PI * 45.0 / 180.0);
    qDebug()<<"sin(90):"<<sin(M_PI * 90.0 / 180.0 );

    //起始点
    m_path->clear();
    m_path->addNextPoint( car.curPoint );
//    car.fd(20);
    qDebug()<<"test4";
    pgmScene->addItem(m_path);
    qDebug()<<"test5";

    //以下，进行轨迹绘制
    bool isNearEdge = false;
    bool isNotEnd = true;
    int n = 10000;
    int k = 0;
    int m_path_fixed = 1;//为什么m_path_fixed从1开始，因为第0个点未贴边，不要
    bool isFirstRound = true;
    bool setStartPoint = false;
    int roundTimes = 1;
    while(n-- && isNotEnd)
    {
        k ++;
        //判断是否返回起点
        if ( k > 4  && DIS2( car.curPoint, startPoint ) <= step * step )
        {
            qDebug()<<"圈数："<<roundTimes++;
            qDebug()<<"已循环至起点，更新地图1";
            QPolygonF polygon;
            //绘制m_path附近的widthPX2(w_/2)宽度的多边形，再将其从toCover.graph中去除
            //先绘制一边
            for ( int i = m_path_fixed; i < m_path->pointsNumber() - 1; ++ i )
            {
                const QPointF& p1 = m_path->getPoint( i );
                const QPointF& p2 = m_path->getPoint( i + 1 );

                QLineF line(p1, p2);
                QPointF normal = line.normalVector().unitVector().p2() - p1;
                QPointF offset = normal * w_ / 2;
                QPointF p1a = p1 + offset;
                QPointF p2a = p2 + offset;

                polygon << p1a << p2a;
//                qDebug()<<"p12a:"<<normal<<offset<<p1a<<p2a;
            }
            qDebug()<<"已循环至起点，更新地图2";
            //再绘制一边
            for ( int i = m_path->pointsNumber() - 1; i > m_path_fixed; -- i )
            {
                const QPointF& p1 = m_path->getPoint( i );
                const QPointF& p2 = m_path->getPoint( i - 1 );

                QLineF line(p1, p2);
                QPointF normal = line.normalVector().unitVector().p2() - p1;
                QPointF offset = normal * w_ / 2;
                QPointF p1b = p1 + offset;
                QPointF p2b = p2 + offset;

                polygon << p1b << p2b;
//                qDebug()<<"p12b:"<<normal<<offset<<p1b<<p2b;
            }
            m_path_fixed = m_path->pointsNumber() - 1;
            qDebug()<<"已循环至起点，更新地图3";
            //graph更新
            QPainterPath pathPolygon;
            pathPolygon.addPolygon(polygon);
            path = path.subtracted(pathPolygon);

            //这里简化一下graph
            qDebug()<<"contours2dFromPainterPath:size:"<<contours2dFromPainterPath(path).size();
            qDebug()<<"卡住测试2";
            toCover.updateFromContours2d(contours2dFromPainterPath(path));
//            toCover.openOperation(w_/2);    //这里已经toGraph(更新graph)了
//            path = toCover.pointsToPath();
//            qDebug()<<"卡住测试4"<<path.isEmpty();
//            qDebug()<<"contours2dFromPainterPath:size:"<<contours2dFromPainterPath(path).size();
//            return;
            QPainterPath tmp_painterPath = path;
            graphShow* pathShow = new graphShow(&tmp_painterPath);
            pgmScene->addItem(pathShow);
//            return;
//            std::vector<std::vector<QPointF>> contours2d_tmp = smooth_coverPathEdges(contours2dFromPainterPath(path), w, mr);
//            path = QPainterPathFromContours2d(contours2d_tmp);

            std::vector<std::vector<QPointF>> contours2d_tmp = smooth_coverPathEdges(toCover.openOperationTOcontours2d(w_/2), w, mr);
            path = QPainterPathFromContours2d(contours2d_tmp);
            qDebug()<<"卡住测试3";
            toCover.updateFromContours2d(contours2dFromPainterPath(path));
            qDebug()<<"已循环至起点，更新地图4,contours2d_tmp.size:"<<contours2d_tmp.size()<<"contours2d_tmp[0].size:"<<contours2d_tmp[0].size();
            //更新car位姿   //角度不改变了，提高鲁棒性
//            car.curPoint = QPointF( startPoint.x() + w_ * sin(angle), startPoint.y() - w_ * cos(angle) );
            car.curPoint = QPointF( startPoint.x() + w_ * sin(car.angle), startPoint.y() - w_ * cos(car.angle) );
//            car.angle = angle;
            setStartPoint = true;  //用于下一次判断
            k = 0;  //k恢复
//            m_path->addNextPoint( car.curPoint );   //测试用，请注释
            qDebug()<<"已循环至起点，更新地图5";
//            toCover.show(); //测试用，请注释
            qDebug()<<"卡住测试5";
//            break;  //测试用，请注释
            if(contours2d_tmp.size() == 1 && contours2d_tmp[0].size() == 0)
            {
                QMessageBox::information(coverPathDialog, tr("覆盖结束"), tr("覆盖完成"));
                break;
            }
        }

        //让car贴边
        int t = 4;  //防止car不贴边,进行多角度扫描
        int chances = 1;    //角度扫描控制
        while(!isNearEdge && t--)
        {
            qDebug()<<"t:"<<t;
            if( isFirstRound && ++t )
//                car.angle = angle + M_PI * 45.0 / 180.0;
                car.angle = M_PI * startDirection / 180.0;
            else if( t == 3 )
                car.angle = car.angle + M_PI * 90.0 / 180.0;
            car.fd(w_/2);

            //判断car isNear
            auto IsNear = [](PathSimulate car, Graph toCover) -> bool
            {
                std::vector<QPoint> carRound = car.round();
                /*
                qDebug()<<"carRound.size():"<<carRound.size();//利用test查看一下carRound的情况
                carRoundCircle* test = new carRoundCircle(car.curPoint,carRound);//利用test查看一下carRound的情况
                qDebug()<<"add test1";
                pgmScene->addItem(test);//利用test查看一下carRound的情况
                qDebug()<<"add test2";
                return true;
                */
                for ( auto& it : carRound )
                {
                    if ( !toCover.graph[it.y()][it.x()] )   //这里同上坐标系(x,y)对应(j,i);
                    {
                        qDebug()<<"Is Near!, the point is:"<< it.x()<<it.y();
                        return true;
                        break;
                    }
                }
                return false;
            };
            /*
            IsNear();//利用test查看一下carRound的情况
            qDebug()<<"test3";
            break;//利用test查看一下carRound的情况
            */

            if ( IsNear(car, toCover) )
            {
                t = 4;//贴边，恢复t值
                qDebug()<<"卡住测试1";
                car.bk(w_/2);
                double left = -5*w_, right = w_/2;
                for ( ; left < w_/2 && car.fd(left) && IsNear(car, toCover); car.bk(left), left += 0.1 * w_ ); //让left值是非贴边的
                car.bk(left);   //for循环中最后一步没有进行car.bk(left);
                while ( left + 0.005 * w_ < right )
                {
                    double mid = left + ( right - left ) / 2;
                    car.fd(mid);
                    if ( IsNear(car, toCover) )
                        right = mid;
                    else
                        left = mid;

                    car.bk(mid);
                }
                if( left >= w_/2 && chances >= 0 )    //left>=10表明没有能贴边的位置
                {
                    if( chances == 1 )
                        car.angle -= M_PI * 45.0 * chances / 180.0;
                    else if( chances == 0 )
                        car.angle += M_PI * 90.0 * chances / 180.0;
                    chances --;
                    qDebug()<<"卡住测试7:angle:"<<car.angle/M_PI*180;
                }
                else if ( left >= w_/2 && chances < 0 )   //chances<0表明三个角度都试过了，没成功
                {
                    isNotEnd = false;
                    qDebug()<<"卡住测试6";
                    break;
                }
                else    //left<10至少是贴边了
                {
                    chances = 1;

                    car.fd(right);  //为什么是right呢，因为需要贴边//或者，贴边时用的round参数大一些//先看效果
                    isNearEdge = true;
                    qDebug()<<"1:fd"<<right<<"left="<<left;
/*利用test查看一下carRound的情况
                    std::vector<QPoint> carRound = car.round();
                    qDebug()<<"carRound.size():"<<carRound.size();//利用test查看一下carRound的情况
                    carRoundCircle* test = new carRoundCircle(car.curPoint,carRound);//利用test查看一下carRound的情况
                    pgmScene->addItem(test);//利用test查看一下carRound的情况
                    qDebug()<<"卡住测试8";
*/

                }

            }

        }
        qDebug()<<"卡住测试9";
        /*
        break;//利用test查看一下carRound的情况
        */
        //贴边失败，结束覆盖
        if( t <= 0 )
        {
            qDebug()<<"t:"<<t;
            QMessageBox::information(coverPathDialog, tr("覆盖结束"), tr("未能贴边,当前位置太空旷"));
            break;
        }
        if(!isNotEnd)
        {
            qDebug()<<"卡住测试10";
            QMessageBox::information(coverPathDialog, tr("覆盖结束"), tr("贴边失败，当前位置太狭窄"));
            qDebug()<<"卡住测试11";
            break;
        }
        if (isFirstRound)
        {
            startPoint = car.curPoint;
            qDebug()<<"setFirstStartPoint:"<<startPoint;
            isFirstRound = false;
        }
        if ( setStartPoint )
        {
            startPoint = car.curPoint;
            qDebug()<<"setStartPoint:"<<startPoint;
            setStartPoint = false;
        }
        //isNearEdge之后，car就贴边了，开始螺旋式覆盖
        m_path->addNextPoint( car.curPoint );
        //判断边缘的方向，给出angle
        auto getRightEdgeAngle = [=]() -> double
        {
            std::vector<QPoint> carRound = car.round();
            double x_sum = 0, y_sum = 0;
            int num = 0;
            for ( auto& it : carRound )
            {
                if ( !toCover.graph[it.y()][it.x()] )   //这里同上坐标系(x,y)对应(j,i);
                {
                    ++ num;
                    x_sum += it.x();
                    y_sum += it.y();
                }
            }
            qDebug()<<"贴边点num:"<<num;
            if(num){
//                qDebug()<<"num:"<<num;
                x_sum /= num;
                y_sum /= num;
                return atan2( y_sum - car.curPoint.y(), x_sum - car.curPoint.x() ) - M_PI * 90.0 / 180.0;
            }
            else
            {
                return car.angle;   //这里怎么写待定//因为之前进行了fd(right)所以应该是贴边的
            }
        };
        car.angle = getRightEdgeAngle();
        qDebug()<<"angle:"<<car.angle;
        car.fd(step);
        isNearEdge = false;
//        m_path->addNextPoint( car.curPoint );
//        break;  //angle 测试
    }
    if ( QMessageBox::Yes == QMessageBox::information(this, tr(""),tr("是否填充全部轨迹？"),QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes))
    {
        on_do_fill_path_triggered();
    }
    return;
}

//slot_预计覆盖区域
void MainWindow::show_smooth_coverPathEdges(int w, double mr)
{
    emit cleanLastSCP();
    disconnect(this, &MainWindow::cleanLastSCP, this, nullptr);
    ui->statusbar->showMessage( "正在计算。。。" );
    std::vector<std::vector<QPointF>> contours2d = smooth_coverPathEdges(mapSelectedTOcontours2d(mapSelected), w, mr);
    smoothedCoverPath * SCP = new smoothedCoverPath(contours2d);
    pgmScene->addItem(SCP);
    ui->statusbar->showMessage( "已显示预计覆盖区域", 1500 );
    connect(this, &MainWindow::cleanLastSCP, this, [=](){ pgmScene->removeItem(SCP); delete SCP; });
}

//保存多边形区域
void MainWindow::on_do_save_polygons_area_triggered()
{
    QString csvPath = ConstImage.left(ConstImage.lastIndexOf(".")).left(ConstImage.lastIndexOf("/"));
    csvPath = QFileDialog::getSaveFileName(this, tr("Save csv"), csvPath,
                                             tr("file CSV (*.csv);;"
                                                "All files (*);;"));
    std::ofstream outFile;
    outFile.open( UTF8ToGBK(csvPath), std::ios::out );

    for ( size_t i = 0; i < mapSelected->polygons.size(); ++ i )
    {
        outFile << "#polygon " << i << std::endl;
        for ( size_t j = 0; j < mapSelected->polygons[i].size(); ++ j )
        {
            outFile << std::to_string( mapSelected->polygons[i][j]->getPoint().x() ) << ','
                    << std::to_string( mapSelected->polygons[i][j]->getPoint().y() ) << std::endl;
        }
        outFile << "#end" << std::endl;
    }

    outFile.close();
}

//加载多边形区域
void MainWindow::on_do_load_polygons_area_triggered()
{
    QString csvPath = ConstImage.left(ConstImage.lastIndexOf(".")).left(ConstImage.lastIndexOf("/"));
    csvPath = QFileDialog::getOpenFileName(this, tr("Open csv"), csvPath, tr("CSV FIles (*.csv);;"));

    std::ifstream inFile( UTF8ToGBK(csvPath), std::ios::in );

    if ( !inFile.is_open() )
    {
        QMessageBox::warning( this, tr(WINDOW_ERROR), tr("打开文件失败，请检查路径") );
        return;
    }
    else
    {
        initMapSelect();
        pgmScene->addItem( mapSelected );

        std::string line;
        while ( std::getline( inFile, line ) )
        {
            if ( line[1] == 'p' ) continue;
            else if ( line[1] == 'e' ) mapSelected->cur_ploygon_addPoint( QPointF( 0, 0 ), true );
            else
            {
                qreal x,y;
                x = std::stod( line.substr( 0, line.find(',') ) );
                y = std::stod( line.substr( line.find(',') + 1 ) );
                mapSelected->cur_ploygon_addPoint( QPointF( x, y ), false );
                qDebug()<<"add Point: "<<x<<','<<y;
            }
        }
    }
    inFile.close();
}

//清除多边形
void MainWindow::on_do_clean_polygons_area_triggered()
{
    initMapSelect();
}

//多边形橡皮擦
void MainWindow::on_doDrawWhitePolygon_triggered()
{
    if ( erase )
    {
        disconnect(pgmScene, SIGNAL(erase_ploygon_addPoint(QPointF)), erase, SLOT(pushPoint(QPointF)));
        delete erase;
    }

    erase = new erasePolygon;
    connect(pgmScene, SIGNAL(erase_ploygon_addPoint(QPointF)), erase, SLOT(pushPoint(QPointF)));

    pgmScene->drawWhitePolygon();
    pgmScene->addItem(erase);
    ui->statusbar->showMessage( "左键点击加入点，右键点击结束绘制", 5000 );
}

//func_多边形橡皮擦
void MainWindow::drawWhitePolygon(QList<QPointF> m_list)
{
    if ( erase && erase->scene() == pgmScene )
        pgmScene->removeItem(erase);

    std::vector<cv::Point> pts;
    for ( int i = 0; i < m_list.size(); ++ i )
    {
        pts.push_back( cv::Point( m_list[i].x(), m_list[i].y() ) );
    }
    cv::fillPoly( imgTmp, pts, 255, 8, 0 );

    //获取tmp图片
    QPixmap pgmPixmap =CvMat8UToQPixmap(imgTmp);
    //更新图片
    updateImage(pgmPixmap);
    flag = true;
}

//追加轨迹
void MainWindow::on_doDrawPathContinue_triggered()
{
    pgmScene->startCreate();
    setActionStatus(false);
    pgmScene->addItem(m_path);
    ui->statusbar->showMessage( "左键点击加入点，右键点击结束绘制" );
}

//func_计算两个点的叉积
double MainWindow::crossProduct(const QPointF& p1, const QPointF& p2) {
    return -(p1.x() * p2.y() - p1.y() * p2.x());    //因为y方向向下，所以顺逆时针相反
}

//func_判断一系列点是否按顺时针方向组成闭合图形
bool MainWindow::isClockwise() {
    int numPoints = m_path->pointsNumber();
    double sum = 0.0;

    for (int i = 0; i < numPoints; ++i) {
        const QPointF& current = m_path->getPoint(i);
        const QPointF& next = m_path->getPoint((i + 1) % numPoints);

        sum += crossProduct(current, next);
    }

    return sum < 0.0;
}

//func_找到给定点 startPoint 到区域 contours2d 内最近的点
QPointF MainWindow::findNearestPoint(const std::vector<std::vector<QPointF>>& contours2d, const QPointF& startPoint) {
    double minDistance = std::numeric_limits<double>::max();
    QPointF nearestPoint = startPoint;  //这里赋一个初值，不至于说最后没结果

    for (const auto& contour : contours2d) {
        for (const auto& point : contour) {
            double d = DIS2(point, startPoint);
            if (d < minDistance) {
                minDistance = d;
                nearestPoint = point;
            }
        }
    }

    return nearestPoint;
}
//slot_绘制rtk覆盖式轨迹
void MainWindow::receiveCoverPath_rtk_Value(int w, double mr, int pathType, int directionType, int mapAccuracy)
{
    qreal minx=std::numeric_limits<double>::max(),miny=std::numeric_limits<double>::max(),
        maxx=std::numeric_limits<double>::lowest(),maxy=std::numeric_limits<double>::lowest();
    for ( int i = 0; i < m_path->pointsNumber(); ++ i )
    {
        qreal x = m_path->getPoint(i).x(),
            y = m_path->getPoint(i).y();
        if ( x < minx ) minx = x;
        if ( x > maxx ) maxx = x;
        if ( y < miny ) miny = y;
        if ( y > maxy ) maxy = y;
    }
    qreal dvax = minx - 50,
        dvay = miny - 50;
    m_path->move(QPointF(-dvax,-dvay)); //将m_path移至（0，0）

    qreal magnification = mapAccuracy * resolution * 100 / w;
    int cols = (int)(maxx - minx + 100) * magnification,
        rows = (int)(maxy - miny + 100) * magnification;
    resolution /= magnification;
    m_path->trans(magnification); //将m_path放大magnification倍
    qDebug()<<"magnification:"<<dvax<<dvay<<cols<<rows<<magnification<<resolution;

    Graph toCover = Graph(rows, cols);

    //信息
    QPointF startPoint = m_path->getPoint( m_path->pointsNumber() - 1 );
    double angle;
    double w_ = w / 100.0 / resolution;
    double mr_ = mr / resolution;

    std::vector<std::vector<QPointF>> contours2d;
    //首先判断这个边界是顺时针还是逆时针
    if (isClockwise()) {
        std::vector<QPointF> contour2d;
        int numPoints = m_path->pointsNumber();
        for ( int i = 0; i < numPoints; ++ i )
        {
            const QPointF& p1 = m_path->getPoint(i);
            const QPointF& p2 = m_path->getPoint((i + 1) % numPoints);
            QLineF line(p1, p2);
            QPointF normal = line.normalVector().unitVector().p2() - p1;
            QPointF offset = normal * w_ / 2;
            QPointF p1a = p1 - offset;  //右侧
            contour2d.push_back(p1a);
        }
        contours2d.push_back(contour2d);
    } else {
        std::vector<QPointF> contour2d;
        int numPoints = m_path->pointsNumber();
        for ( int i = 0; i < numPoints; ++ i )
        {
            const QPointF& p1 = m_path->getPoint(i);
            const QPointF& p2 = m_path->getPoint((i + 1) % numPoints);
            QLineF line(p1, p2);
            QPointF normal = line.normalVector().unitVector().p2() - p1;
            QPointF offset = normal * w_ / 2;
            QPointF p1a = p1 + offset;  //左侧
            contour2d.push_back(p1a);
        }
        contours2d.push_back(contour2d);
    }
    contours2d = smooth_coverPathEdges(contours2d, w, mr);
//    qDebug()<<"闪退测试1"<<contours2d.size()<<contours2d[0].size();
//    smoothedCoverPath * SCP = new smoothedCoverPath(contours2d);
//    pgmScene->addItem(SCP);
    toCover.updateFromContours2d(contours2d);
    QPainterPath ppath = QPainterPathFromContours2d(contours2d);
//    qDebug()<<"闪退测试2"<<ppath.isEmpty();

    QPointF tmp_start = findNearestPoint(contours2d,startPoint);
    angle = atan2( tmp_start.y() - startPoint.y(), tmp_start.x() - startPoint.x() );    //atan2(0,0) == 0;

    //调整car的位置
    PathSimulate car(tmp_start, angle, w_, mr_);
    car.fd(w_/2);
    car.angle += M_PI * 90 / 180;
//    car.fd(w_);

    //以下，进行轨迹绘制
    bool isNearEdge = false;
    bool isNotEnd = true;
    int n = 10000;
    int k = 0;
    int m_path_fixed = m_path->pointsNumber();
    bool isFirstRound = true;
    bool setStartPoint = false;
    int roundTimes = 1;
    while(n-- && isNotEnd)
    {
        qDebug()<<"闪退测试3 angle:"<<car.angle;
        k ++;
        //判断是否返回起点
        if ( k > 4  && DIS2( car.curPoint, startPoint ) <= 1/resolution * 1/resolution )
        {
            qDebug()<<"圈数："<<roundTimes++;
            QPolygonF polygon;
            for ( int i = m_path_fixed; i < m_path->pointsNumber() - 1; ++ i )
            {
                const QPointF& p1 = m_path->getPoint( i );
                const QPointF& p2 = m_path->getPoint( i + 1 );
//                QLineF line(p1, p2);
//                QPointF normal = line.normalVector().unitVector().p2() - p1;
                QPointF diff = p2 - p1;
                QPointF normal(-diff.y(), diff.x());
                QPointF offset = normal / sqrt(DIS2(p1, p2)) * w_ / 2;
                QPointF p1a = p1 + offset;
                QPointF p2a = p2 + offset;
                polygon << p1a << p2a;
            }
            for ( int i = m_path->pointsNumber() - 1; i > m_path_fixed; -- i )
            {
                const QPointF& p1 = m_path->getPoint( i );
                const QPointF& p2 = m_path->getPoint( i - 1 );
                QPointF diff = p2 - p1;
                QPointF normal(-diff.y(), diff.x());
                QPointF offset = normal / sqrt(DIS2(p1, p2)) * w_ / 2;
                QPointF p1b = p1 + offset;
                QPointF p2b = p2 + offset;
                polygon << p1b << p2b;
            }
            m_path_fixed = m_path->pointsNumber();
            //graph更新
            QPainterPath pathPolygon;
            pathPolygon.addPolygon(polygon);
//            BPolygonItem* BPI = new BPolygonItem(polygon);
//            pgmScene->addItem(BPI);
            ppath = ppath.subtracted(pathPolygon);
            toCover.updateFromContours2d(contours2dFromPainterPath(ppath));
            std::vector<std::vector<QPointF>> contours2d_tmp = smooth_coverPathEdges(toCover.openOperationTOcontours2d(w_), w, mr);
            ppath = QPainterPathFromContours2d(contours2d_tmp);
            toCover.updateFromContours2d(contours2dFromPainterPath(ppath));
            car.curPoint = QPointF( startPoint.x() + w_ * sin(car.angle), startPoint.y() - w_ * cos(car.angle) );
            setStartPoint = true;  //用于下一次判断
            k = 0;  //k恢复
//            smoothedCoverPath * SCP = new smoothedCoverPath(contours2d_tmp);
//            pgmScene->addItem(SCP);
            if(contours2d_tmp.size() == 1 && contours2d_tmp[0].size() == 0)
            {
                QMessageBox::information(coverPathDialog, tr("覆盖结束"), tr("覆盖完成"));
                break;
            }
        }

//        qDebug()<<"闪退测试4";
        //让car贴边
        int t = 4;  //防止car不贴边,进行多角度扫描
        int chances = 1;    //角度扫描控制
        while(!isNearEdge && t--)
        {
//            qDebug()<<"闪退测试5";
            car.angle = car.angle + M_PI * 90.0 / 180.0;//这个是配合下面的那个chances的
            qDebug()<<"卡住测试7.1:angle:"<<car.angle/M_PI*180;
            car.fd(w_/2); //让car垂直90度前进以贴边

            auto IsNear = [](PathSimulate car, Graph toCover) -> bool
            {
//                qDebug()<<"闪退测试5.2"<<car.curPoint;
                std::vector<QPoint> carRound = car.round();
                for ( auto& it : carRound )
                {
//                    qDebug()<<"闪退测试5.3"<<it.y()<<it.x();
//                    qDebug()<<toCover.graph[it.y()][it.x()];
                    if ( !toCover.graph[it.y()][it.x()] )   //这里同上坐标系(x,y)对应(j,i);
                    {
//                        qDebug()<<"闪退测试5.4";
                        qDebug()<<"Is Near!, the point is:"<< it.x()<<it.y();
                        return true;
                        break;
                    }
                }
                return false;
            };

            if ( !IsNear(car, toCover) )
            {
                car.angle -= M_PI * 90.0 / 180.0;
//                qDebug()<<"卡住测试7.2:angle:"<<car.angle/M_PI*180;
//                qDebug()<<"闪退测试6";
//                t++;
            }
            if ( IsNear(car, toCover) )
            {
//                qDebug()<<"闪退测试5.1";
                t = 4;//贴边，恢复t值
                qDebug()<<"卡住测试1";
                car.bk(w_/2);
                double left = -w_, right = w_/2;
                for ( ; left < w_/2 && car.fd(left) && IsNear(car, toCover); car.bk(left), left += 0.1 * w_ ); //让left值是非贴边的
                car.bk(left);   //for循环中最后一步没有进行car.bk(left);
                while ( left + 0.005 * w_ < right )
                {
                    double mid = left + ( right - left ) / 2;
                    car.fd(mid);
                    if ( IsNear(car, toCover) )
                        right = mid;
                    else
                        left = mid;

                    car.bk(mid);
                }
                if( left >= w_/2 && chances >= 0 )    //left>=w_/2表明没有能贴边的位置
                {
                    car.angle -= M_PI * 135.0 * chances / 180.0;
                    chances --;
//                    qDebug()<<"卡住测试7:angle:"<<car.angle/M_PI*180;
                }
                else if ( left >= w_/2 && chances < 0 )   //chances<0表明三个角度都试过了，没成功
                {
                    isNotEnd = false;
                    qDebug()<<"卡住测试6";
                    break;
                }
                else    //left<w_/2至少是贴边了
                {
                    chances = 1;

                    car.fd(right);  //为什么是right呢，因为需要贴边//或者，贴边时用的round参数大一些//先看效果
                    isNearEdge = true;
                    qDebug()<<"1:fd"<<right<<"left="<<left;
                }
            }
        }
//        qDebug()<<"闪退测试7";
        //贴边失败，结束覆盖
        if( t <= 0 )
        {
            qDebug()<<"t:"<<t;
            QMessageBox::information(coverPathDialog, tr("覆盖结束"), tr("未能贴边,当前位置太空旷"));
            break;
        }
        if(!isNotEnd)
        {
            qDebug()<<"卡住测试10";
            QMessageBox::information(coverPathDialog, tr("覆盖结束"), tr("贴边失败，当前位置太狭窄"));
            qDebug()<<"卡住测试11";
            break;
        }
        if (isFirstRound)
        {
            startPoint = car.curPoint;
            qDebug()<<"setFirstStartPoint:"<<startPoint;
            isFirstRound = false;
        }
        if ( setStartPoint )
        {
            startPoint = car.curPoint;
            qDebug()<<"setStartPoint:"<<startPoint;
            setStartPoint = false;
        }
        //isNearEdge之后，car就贴边了，开始螺旋式覆盖
        m_path->addNextPoint( car.curPoint );

        //判断边缘的方向，给出angle
        auto getRightEdgeAngle = [=]() -> double
        {
            std::vector<QPoint> carRound = car.round();
            double x_sum = 0, y_sum = 0;
            int num = 0;
            for ( auto& it : carRound )
            {
                if ( !toCover.graph[it.y()][it.x()] )   //这里同上坐标系(x,y)对应(j,i);
                {
                    ++ num;
                    x_sum += it.x();
                    y_sum += it.y();
                }
            }
            qDebug()<<"贴边点num:"<<num;
                if(num){
                //                qDebug()<<"num:"<<num;
                x_sum /= num;
                y_sum /= num;
                return atan2( y_sum - car.curPoint.y(), x_sum - car.curPoint.x() ) - M_PI * 90.0 / 180.0;
            }
            else
            {
                return car.angle;   //这里怎么写待定//因为之前进行了fd(right)所以应该是贴边的
            }
        };
        car.angle = getRightEdgeAngle();
        qDebug()<<"angle:"<<car.angle;
        car.fd(1/resolution);
        isNearEdge = false;
    }

    resolution *= magnification;
    m_path->trans(1/magnification);
    m_path->move(QPointF(dvax,dvay));   //将m_path移到偏移位置
    on_do_adjust_triggered();

    if ( QMessageBox::Yes == QMessageBox::information(this, tr("rtk-覆盖"),tr("是否填充全部轨迹？"),QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes))
    {
        on_do_fill_path_triggered();
    }
    on_do_adjust_triggered();
    return;
}

//rtk覆盖式轨迹
void MainWindow::on_do_rtk_full_cover_path_triggered()
{
    //检查resolution属性是否提取
    if ( resolution == 0 )
    {
        QMessageBox::critical(this,tr("rtk"),tr("请检查是否加载了json文件"));
        return;
    }
    //判断是否加载了轨迹
    if ( m_path->pointsNumber() < 3 )
    {
        QMessageBox::warning( this, tr(WINDOW_ERROR), tr("轨迹点太少或未加载轨迹") );
        return;
    }
    //输入框
    CoverPath_Rtk_Dialog* dialog = new CoverPath_Rtk_Dialog();

    connect(dialog, &CoverPath_Rtk_Dialog::sendData, this, &MainWindow::receiveCoverPath_rtk_Value);

    dialog->setAttribute( Qt::WA_QuitOnClose, false );
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    //    dialog->setWindowTitle(tr(""));
    dialog->show();

}

//func_打开一张1000*1000的空白地图
void MainWindow::openBlankPixmap()  //注释类似on_doOpen_triggered()一律省略
{
    if(flag){
        if(QMessageBox::Yes==QMessageBox::question(this,tr("尚未保存"),tr("是否返回操作？"),QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes)){
            return;
        }
    }
    cleanImage();
    QPixmap blankPixmap(1000, 1000);
    blankPixmap.fill(Qt::white);
    imgSrc = QPixmapToCvMat8U(blankPixmap);
    imgTmp = imgSrc.clone();
    pgmPixmapItem = pgmScene->addPixmap(blankPixmap);
    pgmScene->setSceneRect(QRectF(blankPixmap.rect().left()-1000,blankPixmap.rect().top()-1000,3000,3000));
    on_do_adjust_triggered();
    this->setWindowTitle("rtk地图 - Map Tool");
    setActionStatus(true);
    size->setText(QString::number(976.5625, 'f', 2) + "KB");
    pixel->setText(QString::number(pgmPixmapItem->pixmap().width())
                   + " x " + QString::number(pgmPixmapItem->pixmap().height()));
    flag = false;
    isBlankMap = true;
    //rtk地图没有yaml文件
}

//仅打开Json文件
void MainWindow::on_doOpenJsonExclusively_triggered()   //注释类似on_doOpenJson_triggered()一律省略
{
    if ( !isBlankMap ){
        if ( pgmPixmapItem )
        {
            QMessageBox msgBox;
            msgBox.setText("是否采用空白地图?");
            msgBox.setInformativeText("rtk场景一般采用空白地图!");
            QPushButton* yesButton = msgBox.addButton(tr("是"), QMessageBox::AcceptRole);
            QPushButton* noButton = msgBox.addButton(tr("否"), QMessageBox::RejectRole);
            msgBox.addButton(tr("取消"), QMessageBox::RejectRole);
            msgBox.exec();
            if ( msgBox.clickedButton() == yesButton )
            {
                openBlankPixmap();
            }
            else if ( msgBox.clickedButton() == noButton )
            {
                ;
            }
            else
            {
                return;
            }
        }
        else
        {
            openBlankPixmap();
        }
    }

    if(ConstJson == "")
        ConstJson = getUserPath();
    else
        ConstJson = ConstImage.left(ConstImage.lastIndexOf("/"));
    QString jsonPath = QFileDialog::getOpenFileName(this, tr("Open Json"), ConstJson, tr("Json FIles (*.json);;"));
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::critical(this, tr(WINDOW_ERROR), tr("不能打开json文件！\n请检查路径！"));
        return;
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    jsonContent = stream.readAll();
    file.close();
    ui->statusbar->showMessage("json file is opened : " + jsonPath, 3000);
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonContent.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError && !doc.isNull()) {
        QMessageBox::critical(this, tr("json-rtk"), tr("Json格式错误！")+jsonError.error);
        return;
    }

    qDebug()<<"json 格式正确";
    m_path->clear();
    pgmScene->addItem(m_path);
    std::set<QString> KEY { "pixel_x", "pixel_y", "x", "y", "yaw", "welt", "bypass", "seismic", "turnAround", "trash", "charge", "back", "clean_flag" };
    QJsonObject rootObj = doc.object();
    QJsonValue pointsValue = rootObj.value("points");
    if ( pointsValue.type() == QJsonValue::Array )
    {
        QJsonArray pointsArray = pointsValue.toArray();
        if ( pointsArray.size() < 2 )
        {
            QMessageBox::warning(this, tr("json-rtk"), tr("json内的点数量太少！"));
            return;
        }
        int jsoni = 0;
        qreal x1,x2,pix1,pix2,y1,piy1;
        for ( int i = 0; i < pointsArray.size(); i ++ )
        {
            QJsonValue pointValue = pointsArray.at(i);
            // 判断是否是object类型
            if (pointValue.type() == QJsonValue::Object) {
                // 转换为QJsonObject类型
                QJsonObject pointObj = pointValue.toObject();
                if ( jsoni == 0 )
                {
                    x1 = pointObj.value( "x" ).toDouble();
                    pix1 = pointObj.value( "pixel_x" ).toDouble();
                    y1 = pointObj.value( "y" ).toDouble();
                    piy1 = pointObj.value( "pixel_y" ).toDouble();
                    jsoni ++;
                }
                else if ( jsoni == 1 )
                {
                    x2 = pointObj.value( "x" ).toDouble();
                    pix2 = pointObj.value( "pixel_x" ).toDouble();
                    resolution = ( x1 - x2 ) / ( pix1 - pix2 );
                    origin_x = x1 - pix1 * resolution;
                    origin_y = y1 - ( 999 - piy1 ) * resolution;
                    qDebug()<<"resolution:"<<resolution<<"origin_x:"<<origin_x<<"origin_y:"<<origin_y;
                    jsoni ++;
                }
                QJsonValue point_pixel_x = pointObj.value( "pixel_x" );
                qreal pixel_x = point_pixel_x.toDouble();
                QJsonValue point_pixel_y = pointObj.value( "pixel_y" );
                qreal pixel_y = point_pixel_y.toDouble();

                bool has_attr = false;
                int arr[8] = {0,0,0,0,0,0,0,0};
                if ( pointObj.value("welt") != QJsonValue::Undefined && (has_attr = true) )
                    arr[0] = pointObj.value("welt").toInt();
                if ( pointObj.value("bypass") != QJsonValue::Undefined && (has_attr = true) )
                    arr[1] = pointObj.value("bypass").toInt();
                if ( pointObj.value("seismic") != QJsonValue::Undefined && (has_attr = true) )
                    arr[2] = pointObj.value("seismic").toInt();
                if ( pointObj.value("turnAround") != QJsonValue::Undefined && (has_attr = true) )
                    arr[3] = pointObj.value("turnAround").toInt();
                if ( pointObj.value("trash") != QJsonValue::Undefined && (has_attr = true) )
                    arr[4] = pointObj.value("trash").toInt();
                if ( pointObj.value("charge") != QJsonValue::Undefined && (has_attr = true) )
                    arr[5] = pointObj.value("charge").toInt();
                if ( pointObj.value("back") != QJsonValue::Undefined && (has_attr = true) )
                    arr[6] = pointObj.value("back").toInt();
                if ( pointObj.value("clean_flag") != QJsonValue::Undefined && (has_attr = true) )
                    arr[7] = pointObj.value("clean_flag").toInt();

                std::map<QString, QString> self_df;
                QStringList keys = pointObj.keys();
                for ( auto it = keys.begin(); it != keys.end(); it ++ )
                {
                    if ( !KEY.count(*it) && (has_attr = true) )
                        self_df[*it] = QString::fromStdString( std::to_string( pointObj.value( *it ).toInt() ) );
                }

                if ( has_attr )
                    m_path->addNextPoint( QPointF( pixel_x, pixel_y ), arr, self_df );
                else
                    m_path->addNextPoint( QPointF( pixel_x, pixel_y ) );
            }
        }
    }
    on_do_adjust_triggered();
}

//轨迹标注-框选点
void MainWindow::on_do_selectMultiPoint_polygen_triggered()
{
    pgmScene->create_pointSelect_polygon();
    pgmScene->addItem( pointSelected );
    setActionStatus(false);
    ui->statusbar->showMessage( "左键点击绘制点，右键点击结束绘制" );
}

//轨迹标注-清除框选
void MainWindow::on_do_clean_pointSelect_area_triggered()
{
    initPointSelect();
}

//slot_轨迹标注_进行赋值
void MainWindow::getAttributeData(std::string str)
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

    for ( int i = 0; i < point_idx.size() - 1; ++ i )
    {
        BPathItem* tmp = m_path_list[i];
        for ( int j = 0; j < point_idx[i].size(); ++ j )
        {
            tmp->getElement(point_idx[i][j])->initAttr(arr,self_df);
        }
    }
    for ( int i = 0; i < point_idx[m_path_list.size()].size(); ++ i )
    {
        m_path->getElement(point_idx[m_path_list.size()][i])->initAttr(arr,self_df);
    }
}
//轨迹标注-添加属性
void MainWindow::on_do_addAttribute_triggered()
{
    //判断选框中有哪些点
    QPainterPath path = QPainterPathFromContours2d(mapSelectedTOcontours2d(pointSelected));
    point_idx.clear();
    int pointContainNum = 0;
    QList<int> idx;
    for ( auto& it : m_path_list )
    {
        for ( int i = 0; i < it->pointsNumber(); ++ i )
            if ( path.contains(it->getPoint(i)) )
                idx.push_back(i);
        point_idx.push_back(idx);
        pointContainNum += idx.size();
        idx.clear();
    }
    for ( int i = 0; i < m_path->pointsNumber(); ++ i )
        if ( path.contains(m_path->getPoint(i)) )
            idx.push_back(i);
    point_idx.push_back(idx);
    pointContainNum += idx.size();
    idx.clear();
    //如果没有点选中，则直接退出
    if ( pointContainNum == 0 )
    {
        QMessageBox::warning(this, tr(WINDOW_ERROR), tr("区域内没有点"));
        return;
    }
//    point_idx = idx;    //将idx传给MainWindow

    //多点编辑-判断是否属性统一，不统一进行提示
    attribute *attrTest = nullptr;
    bool attrIsSame = true;
    for ( int i = 0; i < point_idx.size() - 1; ++ i )
    {
        BPathItem* tmp = m_path_list[i];
        for ( int j = 0; j < point_idx[i].size(); ++ j )
        {
            if(nullptr == attrTest)
                attrTest = tmp->getElement(point_idx[i][j])->attr;
            auto b = tmp->getElement(point_idx[i][j])->attr;
            if(attrIsSame){
                if( (bool)b ^ (bool)attrTest || ( (bool)b && (bool)attrTest && *b != *attrTest ) )
                    attrIsSame = false;
            }
        }
    }
    for ( int i = 0; i < point_idx[m_path_list.size()].size(); ++ i )
    {
        if(nullptr == attrTest)
            attrTest = m_path->getElement(point_idx[m_path_list.size()][i])->attr;
        auto b = m_path->getElement(point_idx[m_path_list.size()][i])->attr;
        if(attrIsSame){
            if( (bool)b ^ (bool)attrTest || ( (bool)b && (bool)attrTest && *b != *attrTest ) )
                attrIsSame = false;
        }
    }
    if(!attrIsSame)
    {
        QMessageBox msgBox;
        msgBox.setText("选取点属性不统一！");
        msgBox.setInformativeText("是否继续进行属性编辑");
        QPushButton* yesButton = msgBox.addButton(tr("是"), QMessageBox::AcceptRole);
        QPushButton* noButton = msgBox.addButton(tr("否"), QMessageBox::RejectRole);

        msgBox.exec();

        if ( msgBox.clickedButton() == yesButton )
        {
            attrTest = nullptr;
        }
        else if ( msgBox.clickedButton() == noButton )
        {
            return;
        }
    }

    //开始更改属性
    initPointSelect();  //将选框重置了
    for ( int i = 0; i < point_idx.size() - 1; ++ i )
    {
        BPathItem* tmp = m_path_list[i];
        for ( int j = 0; j < point_idx[i].size(); ++ j )  //改一下绿色
        {
            tmp->getElement(point_idx[i][j])->color_seed = 3840;
            tmp->getElement(point_idx[i][j])->colorChange();
        }
    }
    for ( int i = 0; i < point_idx[m_path_list.size()].size(); ++ i )  //改一下绿色
    {
        m_path->getElement(point_idx[m_path_list.size()][i])->color_seed = 3840;
        m_path->getElement(point_idx[m_path_list.size()][i])->colorChange();
    }
    //打开AttributeAddDialog
    AttributeAddDialog *aad = new AttributeAddDialog(this,attrTest);
    connect(aad, SIGNAL(sendData(std::string)), this, SLOT(getAttributeData(std::string))); //这里直接用getAttributeData解决赋值问题
    aad->setAttribute(Qt::WA_QuitOnClose, false);
    aad->setAttribute(Qt::WA_DeleteOnClose);
    aad->show();
}

//清除所有轨迹
void MainWindow::on_doCleanAllMPath_triggered()
{
    for ( auto& it : m_path_list )
    {
        pgmScene->removeItem(it);
        delete it;
    }
    m_path_list.clear();
    if ( m_path->scene() == pgmScene )
        pgmScene->removeItem(m_path);
    m_path->clear();
}

//保存并进行下一条轨迹绘制
void MainWindow::on_do_draw_next_mpath_triggered()
{
    m_path_list.push_back(m_path);
    disconnect(pgmScene, SIGNAL(updatePoint(QPointF,QList<QPointF>,bool)), m_path, SLOT(pushPoint(QPointF,QList<QPointF>,bool)));

    m_path = new BPathItem;
    m_path->setPathWidth(m_path_width);

    //BPathItem : m_path
    connect(pgmScene, SIGNAL(updatePoint(QPointF,QList<QPointF>,bool)), m_path, SLOT(pushPoint(QPointF,QList<QPointF>,bool)));

    pgmScene->addItem(m_path);
}

//加载Json文件(多条轨迹)
void MainWindow::on_doOpenJson_multiMPath_triggered()
{
    if(ConstImage == "")
        ConstJson = getUserPath();
    else
        ConstJson = ConstImage.left(ConstImage.lastIndexOf("/"));

    QString jsonPath = QFileDialog::getOpenFileName(this, tr("Open Json"), ConstJson, tr("Json FIles (*.json);;"));
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::critical(this, tr(WINDOW_ERROR), tr("不能打开json文件！\n请检查路径！"));
        return;
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    jsonContent = stream.readAll();
    file.close();
    ui->statusbar->showMessage("Json文件已被打开，路径为 : " + jsonPath, 3000);
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonContent.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError && !doc.isNull()) {
        QMessageBox::critical(this, tr(WINDOW_ERROR), tr("Json格式错误！\n请检查Json文件是否被破坏"));
        return;
    }
    m_path->clear();
    pgmScene->addItem(m_path);
    std::set<QString> KEY { "pixel_x", "pixel_y", "x", "y", "yaw", "welt", "bypass", "seismic", "turnAround", "trash", "charge", "back", "clean_flag" };
    int road_id  = INT_MIN;
    bool all_has_road_id = true;

    QJsonObject rootObj = doc.object();
    QJsonValue pointsValue = rootObj.value("points");
    if ( pointsValue.type() == QJsonValue::Array )
    {
        QJsonArray pointsArray = pointsValue.toArray();
        qDebug()<<"pointsArray.size();"<<pointsArray.size();
        for ( int i = 0; i < pointsArray.size(); i ++ )
        {
            qDebug()<<"i:"<<i;
            QJsonValue pointValue = pointsArray.at(i);
            if (pointValue.type() == QJsonValue::Object) {
                QJsonObject pointObj = pointValue.toObject();
                QJsonValue point_pixel_x = pointObj.value( "pixel_x" );
                qreal pixel_x = point_pixel_x.toDouble();
                QJsonValue point_pixel_y = pointObj.value( "pixel_y" );
                qreal pixel_y = point_pixel_y.toDouble();
                bool has_attr = false;
                int arr[8] = {0,0,0,0,0,0,0,0};
                if ( pointObj.value("welt") != QJsonValue::Undefined && (has_attr = true) )
                    arr[0] = pointObj.value("welt").toInt();
                if ( pointObj.value("bypass") != QJsonValue::Undefined && (has_attr = true) )
                    arr[1] = pointObj.value("bypass").toInt();
                if ( pointObj.value("seismic") != QJsonValue::Undefined && (has_attr = true) )
                    arr[2] = pointObj.value("seismic").toInt();
                if ( pointObj.value("turnAround") != QJsonValue::Undefined && (has_attr = true) )
                    arr[3] = pointObj.value("turnAround").toInt();
                if ( pointObj.value("trash") != QJsonValue::Undefined && (has_attr = true) )
                    arr[4] = pointObj.value("trash").toInt();
                if ( pointObj.value("charge") != QJsonValue::Undefined && (has_attr = true) )
                    arr[5] = pointObj.value("charge").toInt();
                if ( pointObj.value("back") != QJsonValue::Undefined && (has_attr = true) )
                    arr[6] = pointObj.value("back").toInt();
                if ( pointObj.value("clean_flag") != QJsonValue::Undefined && (has_attr = true) )
                    arr[7] = pointObj.value("clean_flag").toInt();
                std::map<QString, QString> self_df;
                bool is_new_road = false;
                bool has_road_id = false;
                QStringList keys = pointObj.keys();
                for ( auto it = keys.begin(); it != keys.end(); it ++ )
                {
                    if ( !KEY.count(*it) && (has_attr = true) ){
                        self_df[*it] = QString::fromStdString( std::to_string( pointObj.value( *it ).toInt() ) );
                        if ( (*it) == "road_id" )
                        {
                            has_road_id = true;
                            if ( road_id == INT_MIN )
                                road_id = pointObj.value( *it ).toInt();
                            else if ( road_id != pointObj.value( *it ).toInt() )
                            {
                                is_new_road = true;
                                road_id = pointObj.value( *it ).toInt();
                            }
                        }
                    }
                }
                if ( is_new_road )
                    on_do_draw_next_mpath_triggered();

                if ( has_attr )
                    m_path->addNextPoint( QPointF( pixel_x, pixel_y ), arr, self_df );
                else
                    m_path->addNextPoint( QPointF( pixel_x, pixel_y ) );

                if ( !has_road_id )
                    all_has_road_id = false;
            }
        }
    }
    if ( !all_has_road_id )
        QMessageBox::critical(this, tr(WINDOW_ERROR), tr("此Json文件含有未定义road_id的节点，不符合规则"));
    if ( resolution == 0 )
        on_doOpenYaml_triggered();
}

//绘制功能区
void MainWindow::on_do_draw_functional_area_triggered()
{
    pgmScene->create_areaSelect_polygon();
    pgmScene->addItem(areaSelected);
    setActionStatus(false);
    ui->statusbar->showMessage( "左键点击绘制点，右键点击结束绘制" );
}

//清除当前功能区
void MainWindow::on_do_clean_current_functional_area_triggered()
{
    initAreaSelect();
}

//清除所有功能区
void MainWindow::on_do_clean_all_functional_area_triggered()
{
    for ( auto& it : areaSelected_list )
    {
        pgmScene->removeItem(it);
        delete it;
    }
    areaSelected_list.clear();
    initAreaSelect();
}

//保存并进行下一个功能区绘制
void MainWindow::on_do_draw_next_functional_area_triggered()
{
    areaSelected_list.push_back(areaSelected);
    disconnect(pgmScene, SIGNAL(areaSelect_polygon_addPoint(QPointF,bool)),
               areaSelected, SLOT(cur_ploygon_addPoint(QPointF,bool)));

    SetFunctionalAreaName* dialog = new SetFunctionalAreaName;
    connect(dialog, &SetFunctionalAreaName::sendText, areaSelected, &MapSelectItem::setText);
    dialog->exec();
    disconnect(dialog, &SetFunctionalAreaName::sendText, areaSelected, &MapSelectItem::setText);

    areaSelected = new MapSelectItem;

    //MapSelectedItem : areaSelected
    connect(pgmScene, SIGNAL(areaSelect_polygon_addPoint(QPointF,bool)),
            areaSelected, SLOT(cur_ploygon_addPoint(QPointF,bool)));

}

//记录功能区
void MainWindow::on_do_save_functional_area_triggered()
{
    QString csvPath = ConstImage.left(ConstImage.lastIndexOf("/"));
    csvPath = QFileDialog::getSaveFileName(this, tr("Save csv"), csvPath,
                                           tr("file CSV (*.csv);;"
                                              "All files (*);;"));
    std::ofstream outFile;
    outFile.open( UTF8ToGBK(csvPath), std::ios::out );
    //开始保存
    int i = 0;
    for ( auto& it : areaSelected_list )
    {
        for ( auto& it2 : it->polygons )
        {
            outFile << "#polygon " << i ++ << ':' << UTF8ToGBK( it->getText() ) << std::endl;
            for ( auto& it3 : it2 )
            {
                outFile << std::to_string( it3->getPoint().x() ) << ','
                        << std::to_string( it3->getPoint().y() ) << std::endl;
            }
            outFile << "#end" << std::endl;
        }
    }
    outFile.close();
}

