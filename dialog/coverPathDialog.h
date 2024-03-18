#ifndef COVERPATHDIALOG_H
#define COVERPATHDIALOG_H

#include <QDialog>
#include <string>

namespace Ui {
class CoverPathDialog;
}

class CoverPathDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CoverPathDialog(QPointF startPoint, QWidget *parent = nullptr);
    ~CoverPathDialog();

private:
    Ui::CoverPathDialog *ui;
    QPointF startPoint;

    //信号
signals:
    void sendData(int, double, int, QPointF, qreal);
    void sendData_pre(int, double);
    void selectStartPoint();

public slots:
    void setStartPoint(QPointF);
    //槽
private slots:
    void on_buttonOK_accepted();
    void on_buttonOK_rejected();
    void on_pushButton_coverPath_pre_clicked();

    void on_pushButton_selectPoint_clicked();
};

#endif // COVERPATHDIALOG_H
