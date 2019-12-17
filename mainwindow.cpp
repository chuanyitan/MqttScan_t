#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connct_db();

    this->setWindowTitle("智能井盖管理平台");
    this->setWindowIcon(QPixmap(":/picture/title.png"));


    ui->loginbtn->setIcon(QPixmap(":/picture/login_sure.png"));
    ui->loginbtn->setIconSize(QSize(60,40));
    ui->stackedWidget->setCurrentIndex(0);


    //设置功能按键图标及显示方法
    ui->tbn_device->setIcon(QPixmap(":/picture/device_view.png"));
    ui->tbn_view->setIcon(QPixmap(":/picture/log.png"));
    ui->tbn_setting->setIcon(QPixmap(":/picture/setting.png"));
    ui->tbn_dev_contrl->setIcon(QPixmap(":/picture/contrl.png"));
    ui->tbn_device->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->tbn_view->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->tbn_setting->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->tbn_dev_contrl->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->tbn_device->setIconSize(QSize(150,50));
    ui->tbn_view->setIconSize(QSize(150,50));
    ui->tbn_setting->setIconSize(QSize(150,50));
    ui->tbn_dev_contrl->setIconSize(QSize(150,50));
    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setCalendarPopup(true);

    info.port = ui->le_port->text();
    info.ser_ip = ui->le_ip->text();
    info.ser_Client = ui->le_client_id->text();
    info.user_Name = ui->le_username->text();
    info.pass_Word = ui->le_password->text();

    //状态栏显示
    currentRole.setText("管理员");
    this->statusBar()->addWidget(&currentNo,1);
    this->statusBar()->addWidget(&currentName,1);
    this->statusBar()->addWidget(&currentRole,1);
    this->statusBar()->addWidget(&currentTime,3);
    this->statusBar()->addWidget(&currentOpt,6);
    QFont font;
    font.setPointSize(12);
    this->statusBar()->setFont(font);
    model_dev = new QStandardItemModel;
    model_dev_2 = new QStandardItemModel;
    model_log = new QStandardItemModel;
    model_log_perhour = new QStandardItemModel;
    init_Devices();
    init_Dev_Date_Log("init");
    init_Dev_Date_Log_perhour("init");

    userlogin = new UserLogin(db);
    connect(&time,SIGNAL(timeout()),SLOT(timesecond()));
    connect(&table_log_flash_timer,SIGNAL(timeout()),this,SLOT(table_log_timeout()));
    connect(userlogin,SIGNAL(sendloginno(QString,int)),this,SLOT(GetLogIn(QString,int)));

    newdev = new mqtt_Device(db);
    connect(this,SIGNAL(sendTabNo(QString)),newdev,SLOT(setDevDilog(QString)));
    connect(newdev,SIGNAL(toflashMainDev()),this,SLOT(tofalshDevShow()));

    time.start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete userlogin;
}



bool  MainWindow::connct_db()
{
    mutex.lock();
    db = QSqlDatabase::addDatabase("QMYSQL",QString("1"));//设置使用的数据库驱动
    db.setHostName("47.93.62.22"); //数据库服务器IP
    db.setPort(3306);
    db.setUserName("root");//用户名
    db.setPassword("Admin123!");//密码
    db.setDatabaseName("mqtt_data");//使用的数据库
    if(!db.open())//数据库打开失败
    {
        QMessageBox::critical(0,"Cannot open database",db.lastError().text(),QMessageBox::Cancel);
         return false ;
    }
    mutex.unlock();
    return true;
}

void MainWindow::tofalshDevShow()
{
    on_pb_dev_reflash_clicked();
}


void MainWindow::on_loginbtn_clicked()
{
    userlogin->show();
}

void MainWindow::timesecond()
{
    int ss=QDateTime::currentDateTime().toString("ss").toInt();
    if((ss%4)==0)
    {

    }
    currentTime.setText(QDateTime::currentDateTime().toString("MM-dd hh:mm:ss"));
}

void MainWindow::Device_reflash()
{
    QString sql;
    QSqlQuery query(db);
    QString connected_at,id, client_id,state,status;
    QStringList list;
    list<<"客户端ID"<<"在线状态"<<"最近连接时间";
    sql="select * from devices";
    qDebug()<<sql;
    model_dev_2->setColumnCount(3);
    model_dev_2->setHorizontalHeaderLabels(list);
    ui->tableView_dev->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_dev->horizontalHeader()->setStretchLastSection(true);
    if(query.exec(sql))
    {
        int number_index=0;
            while(query.next())
            {
             id=query.value("id").toString();
             client_id=query.value("client_id").toString();
             state=query.value("state").toString();
             connected_at=query.value("connected_at").toString();
             qDebug()<< id <<"  " << client_id <<"  "<< state <<" " << connected_at;

             int statusflag=0;
             if(state == "1")
             {
                 statusflag=1;
                 status="在线";
             }
             else
             {
                statusflag=0;
                status="离线";
             }
             QStandardItem *item_client_id=new QStandardItem(client_id);
             QStandardItem *item_state=new QStandardItem(status);
             QStandardItem *item_connected_at=new QStandardItem(connected_at);


             model_dev_2->setItem(number_index,0,item_client_id);
             model_dev_2->setItem(number_index,1,item_state);
             model_dev_2->setItem(number_index,2,item_connected_at);

             if(statusflag==1)
             {
                model_dev_2->item(number_index,1)->setIcon(QPixmap(":/picture/online.png"));
             }
             else
             {
                 model_dev_2->item(number_index,1)->setIcon(QPixmap(":/picture/outline.png"));

             }
             number_index++;
         }
    }
    ui->tableView_dev->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->tableView_dev->setModel(model_dev_2);
    ui->tableView_dev->show();
}


void MainWindow::init_Devices()
{
   QString sql;
   QSqlQuery query(db);
   QString connected_at,id, client_id,state,status;
   QStringList list;
   model_dev->clear();
   model_dev->setColumnCount(3);
   list<<"客户端ID"<<"在线状态"<<"交互时间";
   sql=QString("select * from devices");
   qDebug()<<sql;
   model_dev->setHorizontalHeaderLabels(list);
   ui->tableView_dev->setSelectionBehavior(QAbstractItemView::SelectRows);
   ui->tableView_dev->horizontalHeader()->setStretchLastSection(true);
   if(query.exec(sql))
   {
       int number_index=0;
           while(query.next())
           {
            id=query.value("id").toString();
            client_id=query.value("client_id").toString();
            state=query.value("state").toString();
            connected_at=query.value("connected_at").toString();
            qDebug()<< id <<"  " << client_id <<"  "<< state <<" " << connected_at;

            int statusflag=0;
            if(state == "1")
            {
                statusflag=1;
                status="在线";
            }
            else
            {
               statusflag=0;
               status="离线";
            }
            QStandardItem *item_client_id=new QStandardItem(client_id);
            QStandardItem *item_state=new QStandardItem(status);
            QStandardItem *item_connected_at=new QStandardItem(connected_at);


            model_dev->setItem(number_index,0,item_client_id);
            model_dev->setItem(number_index,1,item_state);
            model_dev->setItem(number_index,2,item_connected_at);

            if(statusflag==1)
            {
               model_dev->item(number_index,1)->setIcon(QPixmap(":/picture/online.png"));
            }
            else
            {
                model_dev->item(number_index,1)->setIcon(QPixmap(":/picture/outline.png"));

            }
            number_index++;
        }
   }
   else {
       qDebug()<<"slect devices error";
   }
   ui->tableView_dev->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

   ui->tableView_dev->setModel(model_dev);
   ui->tableView_dev->show();
}

void MainWindow::init_Dev_Date_Log_perhour(QString str)
{
    QString sql;
    QString dev_str;
    model_log_perhour->clear();
    if((str == "init") || (str.isNull()))
    {
        sql="select time,client_id,data_type,water_h,water_l,clinometer_x_h,clinometer_x_l,clinometer_y_h,clinometer_y_l,voltage from upload_perhour_table ORDER BY id DESC";
    }
    else {
        dev_str = str;
        sql= QString("select time,client_id,data_type,water_h,water_l,clinometer_x_h,clinometer_x_l,clinometer_y_h,clinometer_y_l,voltage from upload_perhour_table where client_id = '%1' ORDER BY id DESC;").arg(dev_str);
        qDebug()<<sql;
    }
    QSqlQuery query(db);
    QString  client_id,data_type,water_l,water_h,x_axis_l,x_axis_h,y_axis_l,y_axis_h,voltage,time,status;
    QStringList list;

    list<<"上报时间"<<"终端ID"<<"数据类型"<<"液位高度(高)"<<"液位高度(低)"<<"x轴值(高)"<<"y轴值(高)"<<"x轴值(低)"<<"y轴值(低)"<<"电池电压";
    model_log_perhour->setColumnCount(10);
    model_log_perhour->setHorizontalHeaderLabels(list);
    ui->tableView_log_perhour->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_log_perhour->horizontalHeader()->setStretchLastSection(true);
    if(query.exec(sql))
    {
        int number_index=0;
           while(query.next())
           {
             time = query.value("time").toDateTime().toString("yyyy-MM-dd hh:mm:ss");
              qDebug()<<"init_Dev_Date_Log_perhour" <<time;
             client_id=query.value("client_id").toString();
             data_type=query.value("data_type").toString();
             water_l = query.value("water_l").toString();
             water_h = query.value("water_h").toString();
             x_axis_h = query.value("clinometer_x_h").toString();
             x_axis_l = query.value("clinometer_x_l").toString();
             y_axis_h = query.value("clinometer_y_h").toString();
             y_axis_l = query.value("clinometer_y_l").toString();
             voltage =query.value("voltage").toString();

             if(data_type == "31")
             {
                status = "小时报";
             }

             QStandardItem *item_client_id=new QStandardItem(client_id);
             QStandardItem *item_time=new QStandardItem(time);
             QStandardItem *item_data_type=new QStandardItem(status);
             QStandardItem *item_water_h=new QStandardItem(water_h);
             QStandardItem *item_water_l=new QStandardItem(water_l);
             QStandardItem *item_x_axis_h=new QStandardItem(x_axis_h);
             QStandardItem *item_x_axis_l=new QStandardItem(x_axis_l);
             QStandardItem *item_y_axis_h=new QStandardItem(y_axis_h);
             QStandardItem *item_y_axis_l=new QStandardItem(y_axis_l);
             QStandardItem *item_voltage=new QStandardItem(voltage);

             model_log_perhour->setItem(number_index,0,item_time);
             model_log_perhour->setItem(number_index,1,item_client_id);
             model_log_perhour->setItem(number_index,2,item_data_type);
             model_log_perhour->setItem(number_index,3,item_water_h);
             model_log_perhour->setItem(number_index,4,item_water_l);
             model_log_perhour->setItem(number_index,5,item_x_axis_h);
             model_log_perhour->setItem(number_index,6,item_x_axis_l);
             model_log_perhour->setItem(number_index,7,item_y_axis_h);
             model_log_perhour->setItem(number_index,8,item_y_axis_l);
             model_log_perhour->setItem(number_index,9,item_voltage);
            number_index++;
         }
           qDebug()<<number_index;
    }
    else {
        qDebug()<<"slect log_perhour error";
    }

    ui->tableView_log_perhour->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView_log_perhour->setModel(model_log_perhour);
    ui->tableView_log_perhour->show();
}


void MainWindow::init_Dev_Date_Log(QString str)
{
    QString sql;
    QString dev_str;
    if( (str == "init") || (str.isNull() ))
    {
        sql="select * from upload_addreport_table ORDER BY id DESC;";
    }
    else {
        dev_str = str;
        sql= QString("select * from upload_addreport_table where client_id = '%1' ORDER BY id DESC;").arg(dev_str);
    }
    qDebug()<<sql;
    QSqlQuery query(db);
    QString id, client_id,data_type,water_height,x_axis,y_axis,voltage,time,alarm[3],status[3],up_type;
    QStringList list;
    int i=0,flage[3]={0};
    model_log->clear();
    list<<"终端ID"<<"数据类型"<<"液位状态"<<"倾角状态"<<"电压状态"<<"液位高度"<<"x轴值"<<"y轴值"<<"电池电压"<<"上报时间";
    model_log->setColumnCount(10);
    model_log->setHorizontalHeaderLabels(list);
    ui->tableView_log->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_log->horizontalHeader()->setStretchLastSection(true);
    if(query.exec(sql))
    {
        int number_index=0;
           while(query.next())
           {
             id=query.value("id").toString();
             client_id=query.value("client_id").toString();
             data_type=query.value("data_type").toString();
             alarm[WATER] = query.value("w_alarm").toString();
             alarm[ANGLE] = query.value("c_alarm").toString();
             alarm[VOLTOGE] = query.value("v_alarm").toString();
             water_height=query.value("water_height").toString();
             x_axis=query.value("x_axis").toString();
             y_axis=query.value("y_axis").toString();
             voltage=query.value("voltage").toString();
             time=query.value("time").toDateTime().toString("yyyy-MM-dd hh:mm:ss");;
             qDebug()<<"init_Dev_Date_Log_perhour" <<time;

             for( i=0 ;i<3; i++)
             {
                 if(alarm[i] == "400")
                 {
                     status[i] = "警告";
                     flage[i] = 0;
                 }
                 if(alarm[i] == "200")
                 {
                     flage[i] = 1;
                     status[i] = "正常";
                 }
             }

             if(data_type == "30")
             {
                up_type = "分钟报";
             }

             QStandardItem *item_client_id=new QStandardItem(client_id);
             QStandardItem *item_data_type=new QStandardItem(up_type);
             QStandardItem *item_w_alarm=new QStandardItem(status[WATER]);
             QStandardItem *item_c_alarm=new QStandardItem(status[ANGLE]);
             QStandardItem *item_v_alarm=new QStandardItem(status[VOLTOGE]);
             QStandardItem *item_water_height=new QStandardItem(water_height);
             QStandardItem *item_x_axis=new QStandardItem(x_axis);
             QStandardItem *item_y_axis=new QStandardItem(y_axis);
             QStandardItem *item_voltage=new QStandardItem(voltage);
             QStandardItem *item_time=new QStandardItem(time);


             model_log->setItem(number_index,0,item_client_id);
             model_log->setItem(number_index,1,item_data_type);
             model_log->setItem(number_index,2,item_w_alarm);
             model_log->setItem(number_index,3,item_c_alarm);
             model_log->setItem(number_index,4,item_v_alarm);
             model_log->setItem(number_index,5,item_water_height);
             model_log->setItem(number_index,6,item_x_axis);
             model_log->setItem(number_index,7,item_y_axis);
             model_log->setItem(number_index,8,item_voltage);
             model_log->setItem(number_index,9,item_time);

             for(i=0 ;i<3; i++ )
             {
                 if(flage[i]==1)
                 {
                    model_log->item(number_index,i+2)->setIcon(QPixmap(":/picture/online.png"));
                 }
                 else
                 {
                     model_log->item(number_index,i+2)->setIcon(QPixmap(":/picture/outline.png"));

                 }
             }
             number_index++;
         }
    }
    else {
        qDebug()<<"slect log error";
    }
    ui->tableView_log->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView_log->setModel(model_log);
    ui->tableView_log->show();
}


void  MainWindow::Dev_Date_Log(QString str)
{
     QSqlQuery query(db);
    // dev_str = str;
    // sql= QString("select * from upload_addreport_table where client_id = '%1';").arg(dev_str);
}

void MainWindow::GetLogIn(QString username, int no)
{
     ui->tbn_device->click();
     ui->statusBar->setVisible(true);
     ui->tbn_view->setVisible(true);
     ui->tbn_device->setVisible(true);
     ui->tbn_setting->setVisible(true);
     ui->tbn_dev_contrl->setVisible(true);

     loginuserNo = username;
     currentNo.setText(username);
     qthread = new mqtt_thread(&info);
     qthread->start();
     table_log_flash_timer.start(1000*60*2);

     qDebug()<< no << " +++  "<< username ;
}



void MainWindow::closeEvent(QCloseEvent *e)
{
   // e->ignore();
    ui->statusBar->setVisible(false);
    ui->tbn_view->setVisible(false);
    ui->tbn_device->setVisible(false);
    ui->tbn_setting->setVisible(false);
    ui->tbn_dev_contrl->setVisible(false);
    ui->stackedWidget->setCurrentIndex(0);

}

void MainWindow::showEvent(QShowEvent *e)
{
    if(!loginuserNo.isEmpty())
    {
    ui->tbn_device->click();
    ui->statusBar->setVisible(true);
    ui->tbn_view->setVisible(true);
    ui->tbn_device->setVisible(true);
    ui->tbn_setting->setVisible(true);
    ui->tbn_dev_contrl->setVisible(true);

    }
    else
    {
        ui->statusBar->setVisible(false);
        ui->tbn_view->setVisible(false);
        ui->tbn_device->setVisible(false);
        ui->tbn_setting->setVisible(false);
        ui->tbn_dev_contrl->setVisible(false);
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void MainWindow::on_tbn_device_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    currentOpt.setText("设备管理");
}

void MainWindow::on_tbn_dev_contrl_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);///////原先界面为2
    currentOpt.setText("设备操作");
}

void MainWindow::on_tbn_setting_clicked()
{
     ui->stackedWidget->setCurrentIndex(5);
     currentOpt.setText("设置");
}

QStringList MainWindow::checkOnLineDev()
{
     QString sql = "select client_id from devices where state = 1;";
     QStringList qlist;
     QSqlQuery query(db);
     if(query.exec(sql))
     {
         while(query.next())
         {
             qlist.append(query.value("client_id").toString());
         }
     }
     return qlist;
}

void MainWindow::on_tbn_view_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    currentOpt.setText("日志查看");
    QStringList qlist = checkOnLineDev();
    ui->comboBox_3->clear();
    ui->comboBox_3->insertItems(qlist.size(),qlist);
}



void MainWindow::on_pb_mqttparam_save_clicked()
{
     qthread->terminate();
     info.port = ui->le_port->text();
     info.ser_ip = ui->le_ip->text();
     info.ser_Client = ui->le_client_id->text();
     info.user_Name = ui->le_username->text();
     info.pass_Word = ui->le_password->text();
     qthread->start();
}

void MainWindow::on_pb_dev_add_clicked()
{
    newdev ->show();
    emit sendTabNo("0");
}

void MainWindow::on_tableView_dev_clicked(const QModelIndex &index)//获取设备列表被选中的设备
{

    dev_chooseindex = model_dev->data(model_dev->index(index.row(),0)).toString();
     qDebug()<<index.row()<< " " << dev_chooseindex;
    upDateShowParams(dev_chooseindex);
}


void MainWindow::on_pb_dev_reflash_clicked()
{
    init_Devices();
}

void MainWindow::on_pb_dev_change_clicked()
{
    if(dev_chooseindex.isEmpty())
    {
        QMessageBox::critical(0,"can not open device info","Please select a dev to change.",QMessageBox::Cancel);
        return;
    }
    emit sendTabNo(dev_chooseindex);
}

void MainWindow::on_pb_dev_dele_clicked()
{
    if(dev_chooseindex.isEmpty())
    {
        QMessageBox::critical(0,"can not open device info","Please select a dev to delete.",QMessageBox::Cancel);
        return;
    }
    qDebug()<< dev_chooseindex;
    QString sql_del1="delete from rtu_param_table where client_id='"+dev_chooseindex+"'";
    QString sql_del2="delete from devices where client_id='"+dev_chooseindex+"'";
    QSqlQuery query(db),query2(db);
    QMessageBox box;
    QFont font;
    font.setPointSize(20);
    box.setFont(font);
    if(query.exec(sql_del1))
    {
        if(query2.exec(sql_del2))
        {
             box.setText("成功删除");
             box.exec();
             on_pb_dev_reflash_clicked();
             return ;
        }
    }
    box.setText("删除失败");
    box.exec();
    on_pb_dev_reflash_clicked();
}

void MainWindow::upDateShowParams(QString no)
{
        QString rtuid,ip,username,password,clientid,pubtopic1,pubtopic2,subtopic1,subtopic2;
        QString port,heartbeat,water_sample,water_alarm,clinometer_sample,clinometer_alarm,voltage_sample,voltage_alarm;
        QSqlQuery query(db);
        QString sql= QString("select client_id,server_ip,server_port, \
              heartbeat,username,password,clientid,pubtopic_1,pubtopic_2,subtopic_1,subtopic_2,water_sample,water_alarm,\
              clinometer_sample,clinometer_alarm,voltage_sample,voltage_alarm  from rtu_param_table where client_id ='%1';").arg(no);

        if(query.exec(sql))
        {
            if(query.next())
            {
                ip=query.value("server_ip").toString();
                rtuid=query.value("client_id").toString();
                username=query.value("username").toString();
                password=query.value("password").toString();
                clientid=query.value("clientid").toString();
                pubtopic1=query.value("pubtopic_1").toString();
                pubtopic2=query.value("pubtopic_2").toString();
                subtopic1=query.value("subtopic_1").toString();
                subtopic2=query.value("subtopic_2").toString();
                voltage_alarm=query.value("voltage_alarm").toString();
                voltage_sample=query.value("voltage_sample").toString();
                clinometer_alarm=query.value("clinometer_alarm").toString();
                clinometer_sample=query.value("clinometer_sample").toString();
                water_alarm=query.value("water_alarm").toString();
                water_sample=query.value("water_sample").toString();
                heartbeat=query.value("heartbeat").toString();
                port=query.value("server_port").toString();
            }
            else
            {
                // qDebug()<< "查询失败";
            }
        }
              //刷新显示
              QString info = QString(" 服务器IP:%1 \n 服务器端口:%2 \n 终端ID:%3 \n 用户名:%4 \n 客户端ID:%5 \n\
 发布主题1:%6\n 发布主题2:%7 \n 订阅主题1:%8\n 订阅主题2；%9\n 心跳间隔：%10秒\n 液位上报频率(m)：%11分\n 液位报警阈值(cm):%12cm\n\
 角度上报频率(m):%13分\n 角度报警阈值:%14度\n 电压上报频率:%15分\n 电压报警阈值:%16mV\n")\
                                     .arg(ip).arg(port).arg(rtuid).arg(username)\
                                     .arg(clientid).arg(pubtopic1).arg(pubtopic2).arg(subtopic1).arg(subtopic2).arg(heartbeat)\
                                     .arg(water_sample).arg(water_alarm).arg(clinometer_sample).arg(clinometer_alarm).arg(voltage_sample).arg(voltage_alarm);

              ui->te_dev_params->setText(info);

}


void MainWindow::on_comboBox_3_currentIndexChanged(const QString &arg1)
{
    qDebug()<< arg1;
    dev_No = arg1;
    init_Dev_Date_Log(dev_No);
    init_Dev_Date_Log_perhour(dev_No);

}

void MainWindow::table_log_timeout()
{
    table_log_flash_timer.stop();
    init_Dev_Date_Log(dev_No);
    init_Dev_Date_Log_perhour(dev_No);
    init_Devices();
    table_log_flash_timer.start(1000*60*2);
}

void MainWindow::on_pushButton_2_clicked()
{
    init_Dev_Date_Log("init");
    init_Dev_Date_Log_perhour("init");
}


void MainWindow::on_pb_reflash_clicked()
{
    init_Dev_Date_Log(dev_No);
    init_Dev_Date_Log_perhour(dev_No);
}



void MainWindow::on_dateEdit_userDateChanged(const QDate &date)
{
    qDebug()<<date.toString("yyyy-MM-dd");
}
