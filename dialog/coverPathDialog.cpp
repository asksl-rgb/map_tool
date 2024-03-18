#include "coverPathDialog.h"
#include "ui_coverPathDialog.h"

CoverPathDialog::CoverPathDialog(QPointF startPoint, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CoverPathDialog),
    startPoint(startPoint)
{
    ui->setupUi(this);
    std::string startPoint_str = "(" + std::to_string(startPoint.x()) + "," + std::to_string(startPoint.y()) + ")";
    ui->labelStartPointF_data->setText(QString::fromStdString(startPoint_str));
    //设定窗口保持最上层
//    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    qDebug("cp_dialog created");
}

CoverPathDialog::~CoverPathDialog()
{
    qDebug("cp_dialog deleted");
    delete ui;
}

//信号处理
void CoverPathDialog::on_buttonOK_accepted()
{
    int type = (ui->comboBoxPathType->currentText() == "靠右") ? 1 : 0;
    emit sendData(ui->pathWidthSpinBox->value(), ui->minTurnRaiusSpinBox->value(), type,
                  startPoint, ui->SpinBox_initDirection->value());
}

void CoverPathDialog::on_buttonOK_rejected()
{
    this->close();
}


void CoverPathDialog::on_pushButton_coverPath_pre_clicked()
{
    emit sendData_pre(ui->pathWidthSpinBox->value(), ui->minTurnRaiusSpinBox->value());
}


void CoverPathDialog::on_pushButton_selectPoint_clicked()
{
    emit selectStartPoint();
}


void CoverPathDialog::setStartPoint(QPointF startP)
{
    startPoint = startP;
    std::string startPoint_str = "(" + std::to_string(startPoint.x()) + "," + std::to_string(startPoint.y()) + ")";
    ui->labelStartPointF_data->setText(QString::fromStdString(startPoint_str));
}

