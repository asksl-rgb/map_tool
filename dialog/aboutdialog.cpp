#include "aboutdialog.h"
#include "ui_aboutdialog.h"
//#include <QProcess>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    //设置图片
    QPixmap *pixmap = new QPixmap(":/SimpleImage.svg");
    pixmap->scaled(ui->label_Pic->size(), Qt::KeepAspectRatio);
    ui->label_Pic->setScaledContents(true);
    ui->label_Pic->setPixmap(*pixmap);
    //设置字
    QFont font ("Microsoft YaHei", 10, 75);
    ui->label_Text->setText("Map Tool - pgm图片处理，自动驾驶轨迹绘制工具");
    ui->label_Text->setFont(font);
    ui->label_Text->setAlignment(Qt::AlignHCenter);
    ui->label_Text->setStyleSheet("color:#12BCFB;");
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

//三个button
void AboutDialog::on_pushButton_clicked()
{
//    QProcess::startDetached("explorer https://");
    QMessageBox msgBox;
    msgBox.setWindowTitle("关于更新");
    msgBox.setText("此工具处于测试阶段");
    msgBox.setInformativeText("详细请联系~吴恩聪~");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

void AboutDialog::on_pushButton_2_clicked()
{
//    QProcess::startDetached("explorer https://");
    QMessageBox msgBox;
    msgBox.setWindowTitle("技术支持");
    msgBox.setText("通过cmake编译qt+opencv环境");
    msgBox.setInformativeText("cmake：3.24.2\nqt：5.15.15\nopencv：4.8.0");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}


void AboutDialog::on_pushButton_3_clicked()
{
//    QProcess::startDetached("explorer https://");
}
