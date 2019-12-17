#ifndef MQTT_DEVICE_H
#define MQTT_DEVICE_H

#include <QDialog>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QDebug>

namespace Ui {
class mqtt_Device;
}

enum Disdlog_TYPE
{
    ADD ,
    CHANGE,
};

class mqtt_Device : public QDialog
{
    Q_OBJECT

public:
    explicit mqtt_Device(QSqlDatabase db_t,QWidget *parent = nullptr);
    ~mqtt_Device();
    void showDevinfo(QString no);
    QString strconnt(QString topic,QString rutid);

signals:
    void toflashMainDev();

private slots:
    void on_pb_add_clicked();

    void on_pb_cal_clicked();

    void setDevDilog(QString No);

    void on_pb_reflash_clicked();

    void on_le_svn_editingFinished();

private:
    Ui::mqtt_Device *ui;
    QSqlDatabase db ;

};

#endif // MQTT_DEVICE_H
