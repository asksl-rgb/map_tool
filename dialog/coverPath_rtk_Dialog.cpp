#include "coverPath_rtk_Dialog.h"
#include "ui_coverPath_rtk_Dialog.h"

CoverPath_Rtk_Dialog::CoverPath_Rtk_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CoverPath_Rtk_Dialog)
{
    ui->setupUi(this);
    QButtonGroup *block = new QButtonGroup(this);
    block->addButton(ui->radioButton_round,0);
    block->addButton(ui->radioButton_forthAndBack,1);
    ui->radioButton_round->setChecked(true);
    ui->radioButton_forthAndBack->setCheckable(false);
    ui->radioButton_forthAndBack->setVisible(false);
}

CoverPath_Rtk_Dialog::~CoverPath_Rtk_Dialog()
{
    qDebug("cpr_dialog deleted");
    delete ui;
}

void CoverPath_Rtk_Dialog::on_buttonOK_accepted()
{
    int pathType = (ui->comboBoxPathType->currentText() == "靠右") ? 1 : 0;
    int directionType = (ui->radioButton_round->isChecked()) ? 0 : 1;
    emit sendData(ui->pathWidthSpinBox->value(), ui->minTurnRaiusSpinBox->value(),
                  pathType,directionType, ui->SpinBox_mapAccuracy->value());
}

void CoverPath_Rtk_Dialog::on_buttonOK_rejected()
{
    this->close();
}
