#include "attributeAddDialog.h"
#include "ui_attributeAddDialog.h"

AttributeAddDialog::AttributeAddDialog(QWidget *parent, attribute *attr) :
    QDialog(parent),
    ui(new Ui::AttributeAddDialog)
{
    ui->setupUi(this);
    if (attr)
    {
        switch (attr->welt) {
        case 1:
            ui->welt_left->setChecked(true);
            break;
        case 2:
            ui->welt_right->setChecked(true);
            break;
        default:
            ui->welt_NO->setChecked(true);
            break;
        }
        switch (attr->bypass) {
        case 1:
            ui->bypass_off->setChecked(true);
            break;
        default:
            ui->bypass_on->setChecked(true);
            break;
        }
        if(attr->seismic == 1)
            ui->seismic->setChecked(true);
        if(attr->turnAround == 1)
            ui->turnAround->setChecked(true);
        if(attr->trash == 1)
            ui->trash->setChecked(true);
        if(attr->charge == 1)
            ui->charge->setChecked(true);
        if(attr->back == 1)
            ui->back->setChecked(true);
        if(attr->clean_flag == 1)
            ui->clean_flag->setChecked(true);

        int i = 0;
        for ( auto& it : attr->self_define )
        {
            if(i == 0)
            {
                ui->att_1->setText(it.first);
                ui->val_1->setText(it.second);
            }
            else if(i == 1)
            {
                ui->att_2->setText(it.first);
                ui->val_2->setText(it.second);
            }
            else if(i == 3)
            {
                ui->att_3->setText(it.first);
                ui->val_3->setText(it.second);
            }
            i++;
        }
        //如果att_1没有设置值，设定为road_id
        if ( ui->att_1->text().isEmpty() )
            ui->att_1->setText("road_id");
    }
}

AttributeAddDialog::~AttributeAddDialog()
{
    delete ui;
}

void AttributeAddDialog::done(int r)
{
    // 这里是你的条件
    if (r == QDialog::Accepted &&
        self_buttonBox_accepted()/* 你的判断条件 */) {    //在条件不满足时，不关闭窗口
        // 如果满足你的条件，接受对话框并关闭它
        QDialog::done(r);
    }
    else if (r == QDialog::Rejected) {
        // 如果点击的是Cancel按钮，接受对话框并关闭它
        QDialog::done(r);
    }
    else {
        // 如果不满足你的条件，更新窗口的一些数值并返回，对话框不会关闭
    }
}

bool AttributeAddDialog::self_buttonBox_accepted()
{
    std::string ans;

    if(ui->welt_left->isChecked())
        ans +='1';
    else if(ui->welt_right->isChecked())
        ans +='2';
    else if(ui->welt_NO->isChecked())
        ans +='0';
    else
        ans +='0';

    if(ui->bypass_on->isChecked())
        ans +='0';
    else if(ui->bypass_off->isChecked())
        ans +='1';
    else
        ans +='0';

    if(ui->seismic->isChecked())
        ans +='1';
    else
        ans +='0';

    if(ui->turnAround->isChecked())
        ans +='1';
    else
        ans +='0';

    if(ui->trash->isChecked())
        ans +='1';
    else
        ans +='0';

    if(ui->charge->isChecked())
        ans +='1';
    else
        ans +='0';

    if(ui->back->isChecked())
        ans +='1';
    else
        ans +='0';

    if(ui->clean_flag->isChecked())
        ans +='1';
    else
        ans +='0';

    if(!ui->att_1->text().isEmpty()&&!ui->val_1->text().isEmpty())
    {
        bool ok;
        ui->val_1->text().toInt(&ok);
        if (!ok) {
            QMessageBox::critical(this, tr("属性值错误"), ui->att_1->text() + tr("的属性值有误\n各属性值必须为数字") );
            return false;
        }
        ans = ans + ui->att_1->text().toStdString() + ',' + ui->val_1->text().toStdString() + ',';
    }

    if(!ui->att_2->text().isEmpty()&&!ui->val_2->text().isEmpty())
    {
        bool ok;
        ui->val_1->text().toInt(&ok);
        if (!ok) {
            QMessageBox::critical(this, tr("属性值错误"), ui->att_1->text() + tr("的属性值有误\n各属性值必须为数字") );
            return false;
        }
        ans = ans + ui->att_2->text().toStdString() + ',' + ui->val_2->text().toStdString() + ',';
    }

    if(!ui->att_3->text().isEmpty()&&!ui->val_3->text().isEmpty())
    {
        bool ok;
        ui->val_1->text().toInt(&ok);
        if (!ok) {
            QMessageBox::critical(this, tr("属性值错误"), ui->att_1->text() + tr("的属性值有误\n各属性值必须为数字") );
            return false;
        }
        ans = ans + ui->att_3->text().toStdString() + ',' + ui->val_3->text().toStdString() + ',';
    }

    emit sendData(ans);
    return true;
}
