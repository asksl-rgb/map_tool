#include "setFunctionalAreaName.h"
#include "ui_setFunctionalAreaName.h"

SetFunctionalAreaName::SetFunctionalAreaName(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetFunctionalAreaName)
{
    ui->setupUi(this);
}

SetFunctionalAreaName::~SetFunctionalAreaName()
{
    delete ui;
}


void SetFunctionalAreaName::on_pushButton_clicked()
{
    emit sendText(ui->lineEdit->text());
    accept();
}

