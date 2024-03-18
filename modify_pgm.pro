QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MapTool
TEMPLATE = app
CONFIG += C++11

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    control/pathSimulate.cpp \
    dialog/attributeAddDialog.cpp \
    dialog/coverPathDialog.cpp \
    dialog/coverPath_rtk_Dialog.cpp \
    dialog/setFunctionalAreaName.cpp \
    items/Graph.cpp \
    items/GraphicsType.cpp \
    dialog/aboutdialog.cpp \
    items/erasePolygon.cpp \
    items/test.cpp \
    mainwindow/graphicsview.cpp \
    main.cpp \
    mainwindow/mainwindow.cpp \
    items/mapSelectItem.cpp \
    items/startRect.cpp

HEADERS += \
    control/pathSimulate.h \
    dialog/attributeAddDialog.h \
    dialog/coverPathDialog.h \
    dialog/coverPath_rtk_Dialog.h \
    dialog/setFunctionalAreaName.h \
    items/Graph.h \
    items/GraphicsType.h \
    dialog/aboutdialog.h \
    items/erasePolygon.h \
    items/test.h \
    mainwindow/graphicsview.h \
    mainwindow/mainwindow.h \
    items/mapSelectItem.h \
    items/startRect.h \
    tools/UTF8ToGBEx.h

FORMS += \
    dialog/aboutdialog.ui \
    dialog/attributeAddDialog.ui \
    dialog/coverPathDialog.ui \
    dialog/coverPath_rtk_Dialog.ui \
    dialog/setFunctionalAreaName.ui \
    mainwindow/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH +=  D:/dev/opencv/opencv-build/install/include \
                D:/dev/opencv/opencv-build/install/include/opencv2 \
                ./ \
            dialog/ \
            items/ \
            tools/ \
            control/ \
            mainwindow/

LIBS += -L D:/dev/opencv/opencv-build/lib/libopencv_*.a

RESOURCES += \
    res/ImageDir.qrc

RC_FILE = res/ico.rc
