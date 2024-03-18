#ifndef ATTRIBUTEADDDIALOG_H
#define ATTRIBUTEADDDIALOG_H

#include <QDialog>
#include <string>
#include <QMessageBox>

struct attribute{
    int welt;
    int bypass;
    int seismic;
    int turnAround;
    int trash;
    int charge;
    int back;
    int clean_flag;
    std::map<QString, QString> self_define;
    attribute(int *arr) :
        welt(arr[0]), bypass(arr[1]), seismic(arr[2]), turnAround(arr[3]), trash(arr[4]), charge(arr[5]), back(arr[6]), clean_flag(arr[7]) {}
    bool operator==(const attribute& b) const {
        if (welt == b.welt &&
            bypass == b.bypass &&
            seismic == b.seismic &&
            turnAround == b.turnAround &&
            trash == b.trash &&
            charge == b.charge &&
            back == b.back &&
            clean_flag == b.clean_flag &&
            self_define == b.self_define )
            return true;
        else
            return false;
    }
    bool operator!=(const attribute& b) const {
        if (*this == b) return false;
        return true;
    }
};

namespace Ui {
class AttributeAddDialog;
}

class AttributeAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AttributeAddDialog(QWidget *parent = nullptr, attribute *attr = nullptr );
    ~AttributeAddDialog();
    void done(int r) override;
    bool self_buttonBox_accepted();

private slots:

signals:
    void sendData(std::string);

private:
    Ui::AttributeAddDialog *ui;
};

#endif // ATTRIBUTEADDDIALOG_H
