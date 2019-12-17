#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "userlogin.h"
#include <QLabel>
#include <QCloseEvent>
#include <QStandardItemModel>
#include <QTimer>
#include <QAction>
#include <QSqlQuery>
#include <QStandardItem>
#include <QtMqtt/qmqttclient.h>
#include <QMessageBox>
#include <QSqlTableModel>
#include "mqtt_thread.h"
#include "mqtt_device.h"
#include<QSqlError>


enum ALARM_FLAGE
{
    NORMAL = 200,
    ALARM  = 400,
};



namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void  init_Devices();
    void  Devices_show();
    void  init_Dev_Date_Log(QString str);
    void  init_Dev_Date_Log_perhour(QString str);
    void  Dev_Date_Log(QString str);
    bool  connct_db();
    void Device_reflash();
    void upDateShowParams(QString no);
    QStringList checkOnLineDev();
    QString dev_chooseindex;

signals:
   void sendTabNo(QString);

private slots:
    void on_loginbtn_clicked();

    void GetLogIn(QString , int );

    void on_tbn_device_clicked();

    void closeEvent(QCloseEvent *e);

    void showEvent(QShowEvent *e);

    void on_tbn_dev_contrl_clicked();

    void on_tbn_setting_clicked();

    void on_tbn_view_clicked();

    void timesecond();

    void on_pb_mqttparam_save_clicked();

    void on_pb_dev_add_clicked();

    void on_tableView_dev_clicked(const QModelIndex &index);

    void on_pb_dev_reflash_clicked();

    void on_pb_dev_change_clicked();

    void on_pb_dev_dele_clicked();

    void tofalshDevShow();

    void on_comboBox_3_currentIndexChanged(const QString &arg1);

    void table_log_timeout();

    void on_pushButton_2_clicked();

    void on_pb_reflash_clicked();


    void on_dateEdit_userDateChanged(const QDate &date);

private:
    Ui::MainWindow *ui;
    UserLogin *userlogin;

    //状态栏显示
    QLabel currentTime;
    QLabel currentOpt;
    QLabel currentNo;
    QLabel currentName;
    QLabel currentRole;
    QString loginuserNo;
    QString dev_No;
    QTimer time;//状态栏时间显示
    QTimer table_log_flash_timer;//状态栏时间显示

    QStandardItemModel * model_dev;
    QStandardItemModel * model_dev_2;
    QStandardItemModel * model_log;
    QStandardItemModel * model_log_perhour;
    QMqttClient *m_client;
    mqtt_thread *qthread;
    QSqlDatabase db ;
    QMutex mutex;
    mqtt_Device *newdev;
    serInfo_t info;

};

#endif // MAINWINDOW_H
