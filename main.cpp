#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    /*
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
*/
    QApplication a( argc, argv );

//    QFont font(QStringLiteral("Tahoma"));   //设置一种字体
//    font.setPixelSize(12);   //字体大小，等同于PointSize的9点大小
//    qApp->setFont(font);
    /*这个只能设置主页面的字体，不能设置所有的字体*/

    MainWindow *w = new MainWindow();
    w->show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
