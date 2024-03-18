#ifndef SETFUNCTIONALAREANAME_H
#define SETFUNCTIONALAREANAME_H

#include <QDialog>

namespace Ui {
class SetFunctionalAreaName;
}

class SetFunctionalAreaName : public QDialog
{
    Q_OBJECT

public:
    SetFunctionalAreaName(QWidget *parent = nullptr);
    ~SetFunctionalAreaName();

private slots:
    void on_pushButton_clicked();

signals:
    void sendText(QString);

private:
    Ui::SetFunctionalAreaName *ui;

};

#endif // SETFUNCTIONALAREANAME_H
