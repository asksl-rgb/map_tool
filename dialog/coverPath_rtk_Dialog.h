#ifndef COVERPATH_RTK_DIALOG_H
#define COVERPATH_RTK_DIALOG_H

#include <QDialog>
#include <QButtonGroup>

namespace Ui {
class CoverPath_Rtk_Dialog;
}

class CoverPath_Rtk_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit CoverPath_Rtk_Dialog(QWidget *parent = nullptr);
    ~CoverPath_Rtk_Dialog();

signals:
    void sendData(int,double,int,int,int);

private slots:
    void on_buttonOK_accepted();
    void on_buttonOK_rejected();

private:
    Ui::CoverPath_Rtk_Dialog *ui;
};

#endif // COVERPATH_RTK_DIALOG_H
