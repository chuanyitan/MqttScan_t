#include "mqtt_device.h"
#include "ui_mqtt_device.h"

mqtt_Device::mqtt_Device(QSqlDatabase db_t,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mqtt_Device)
{
    ui->setupUi(this);
    db=db_t;
    this->setWindowTitle("设备属性编辑");

}

mqtt_Device::~mqtt_Device()
{
    delete ui;

}

QString mqtt_Device::strconnt(QString topic,QString rutid)
{
    if(topic.contains("${sn}",Qt::CaseInsensitive))
          topic.replace("${sn}",rutid);
          return topic;

}


void mqtt_Device::showDevinfo(QString no)
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
             qDebug()<< "333";
        }
    }
          //刷新显示
          ui->le_ip->setText(ip);
          ui->le_svn->setText(rtuid);
          ui->le_username->setText(username);
          ui->le_password->setText(password);
          ui->le_client_id->setText(clientid);
          ui->te_pub1->setText(pubtopic1);
          ui->te_pub2->setText(pubtopic2);
          ui->te_sc1->setText(subtopic1);
          ui->te_sc2->setText(subtopic2);
          ui->le_port->setText(port);
          ui->le_heartnate->setText(heartbeat);
          ui->le_wsample->setText(water_sample);
          ui->le_walarm->setText(water_alarm);
          ui->le_cslample->setText(clinometer_sample);
          ui->le_calarm->setText(clinometer_alarm);
          ui->le_vsample->setText(voltage_sample);
          ui->le_valarm->setText(voltage_alarm);

          this->show();
}


void mqtt_Device::setDevDilog(QString no)
{
    qDebug()<< no;
    if( no.toInt() == ADD )
    {
        ui->le_svn->clear();
        ui->le_client_id->clear();
        ui->pb_add->setText("增加");
    }
    else {   //则剩下是更改设备内容
       ui->pb_add->setText("修改");
       showDevinfo(no);

    }
}

void mqtt_Device::on_pb_add_clicked()
{
    int state = 0,wstate =0;
    if(ui->le_svn->text().isEmpty())
    {
        QMessageBox box;
        QFont font;
        font.setPointSize(20);
        box.setFont(font);
        box.setText("终端号svn不能为空，请填写！");
        box.exec();
        return;
    }
    if(ui->le_client_id->text().isEmpty())
    {

        QMessageBox box;
        QFont font;
        font.setPointSize(20);
        box.setFont(font);
        box.setText("客户端号不能为空，请填写！");
        box.exec();
        return;
    }
    else if(ui->pb_add->text() == "增加")
    {
        QSqlQuery query(db);
        QMessageBox box;
        QFont font;
        QString ca="select * from devices where client_id='"+ui->le_client_id->text()+"'or client_id='"+ui->le_svn->text()+"' ;";
        if(query.exec(ca))
        {
            if(query.next())
            {
                font.setPointSize(20);
                box.setFont(font);
                box.setText("设备号有重复，请重新填写！");
                box.exec();
                return;
            }
            else
            {
            }
        }
    }
    QString rtuid,ip,username,password,clientid,pubtopic1,pubtopic2,subtopic1,subtopic2;
    int port,heartbeat,water_sample,water_alarm,clinometer_sample,clinometer_alarm,voltage_sample,voltage_alarm;
    ip=ui->le_ip->text();
    rtuid =ui->le_svn->text();
    username = ui->le_username->text();
    password = ui->le_password->text();
    clientid = ui->le_client_id->text();
    pubtopic1 = ui->te_pub1->toPlainText();
    pubtopic2 = ui->te_pub2->toPlainText();
    subtopic1 = ui->te_sc1->toPlainText();
    subtopic2 = ui->te_sc2->toPlainText();
    port = ui->le_port->text().toInt();
    heartbeat = ui->le_heartnate->text().toInt();
    water_sample = ui->le_wsample->text().toInt();
    water_alarm = ui->le_walarm->text().toInt();
    clinometer_sample = ui->le_cslample->text().toInt();
    clinometer_alarm = ui->le_calarm->text().toInt();
    voltage_sample = ui->le_vsample->text().toInt();
    voltage_alarm = ui->le_valarm->text().toInt();
    QSqlQuery query(db);
    QSqlTableModel  model;
    QString sql,sql2; int flage = 0;

    if(ui->pb_add->text() == "增加")//增加需要置参数，主题订阅的标志位
    {
        qDebug()<<"sql122222";
       wstate =1;
       sql= "insert into rtu_param_table(client_id,server_ip,server_port, \
                                   heartbeat,username,password,clientid,pubtopic_1,pubtopic_2,subtopic_1,subtopic_2,water_sample,water_alarm,\
                                   clinometer_sample,clinometer_alarm,voltage_sample,voltage_alarm,get_state,set_state,topic1_state,topic2_state) values('"+rtuid+"','"+ip+"','"+QString::number(port)+"',\
                                   '"+ QString::number(heartbeat)+"','"+username+"','"+password+"','"+clientid+"',\""+pubtopic1+"\",\
                                   \""+pubtopic2+"\",\""+subtopic1+"\",\""+subtopic2+"\",'"+QString::number(water_sample)+"','"+QString::number(water_alarm)+"',\
                                   '"+QString::number(clinometer_sample)+"','"+QString::number(clinometer_alarm)+"','"+QString::number(voltage_sample)+"','"+QString::number(voltage_alarm)+"'\
                                   ,'1','1','1','1')";

        sql2 = "insert into devices(client_id,state) value('"+rtuid+"',0);";
    }
    else if(ui->pb_add->text() == "修改")//修改不允许修改主题，只是下发参数
    {
        wstate =2;
        qDebug()<<"sql111111";
        sql="update rtu_param_table set client_id ='"+rtuid+"', server_ip='"+ip+"', server_port= '"+QString::number(port)+"', heartbeat= '"+ QString::number(heartbeat)+"',\
             username = '"+username+"',password='"+password+"',clientid='"+clientid+"',pubtopic_1=\""+pubtopic1+"\",pubtopic_2=\""+pubtopic2+"\",\
             subtopic_1=\""+subtopic1+"\",subtopic_2=\""+subtopic2+"\",water_sample='"+QString::number(water_sample)+"',water_alarm='"+QString::number(water_alarm)+"',\
             clinometer_sample= '"+QString::number(clinometer_sample)+"', clinometer_alarm= '"+QString::number(clinometer_alarm)+"',  voltage_sample=  '"+QString::number(voltage_sample)+"',\
             voltage_alarm='"+QString::number(voltage_alarm)+"',set_state = '1' where client_id = '"+rtuid+"'";
    }
    else
    {
       ;
    }
qDebug()<<"sql:"<<sql;
qDebug()<<"sql2:"<<sql2;

    model.database().transaction();
    /*
    if(!query.exec( QString("insert into rtu_param_table(client_id,server_ip,server_port, \
                           heartbeat,username,password,clientid,pubtopic_1,pubtopic_2,subtopic_1,subtopic_2,water_sample,water_alarm,\
                           clinometer_sample,clinometer_alarm,voltage_sample,voltage_alarm) values('%1','%2','%3','%4','%5','%6','%7',\"%8\",\
                           \"%9\",\"%10\",\"%11\",'%12','%13','%14','%15','%16','%17')")\
                          .arg(rtuid).arg(ip).arg(port).arg(heartbeat)\
                          .arg(username).arg(password).arg(clientid).arg(pubtopic1).arg(pubtopic2)  \
                          .arg(subtopic1).arg(subtopic2).arg(water_sample).arg(water_alarm).arg(clinometer_sample) \
                          .arg(clinometer_alarm).arg(voltage_sample).arg(voltage_alarm) ) )
    */
    if(!query.exec(sql))
    {
       qDebug()<<"query.exec failed1";
       state = 1;
    }


    if(wstate == 1 )
    {
        /*
         if(!query.exec( QString("insert into rtu_param_table(client_id,state,connected_at) values( \
                                 '%1','%2','%3')").arg(clientid).arg(0).arg( NULL)))
                                 */
        if(!query.exec(sql2))
         {
               qDebug()<<"query.exec failed2";
               state = 1;
         }
        flage = 0;
    }
    model.database().commit();


    QMessageBox box;
    QFont font;
    font.setPointSize(20);
    box.setFont(font);
    if(!state)
    {
       if(wstate == 1)
         box.setText("添加成功！");
       else
         box.setText("修改成功！");
      this->hide();
    }
    else {
         if(wstate == 1)
          box.setText("添加失败！");
        else
          box.setText("修改失败！");
    }
    box.exec();

    emit toflashMainDev();
}

void mqtt_Device::on_pb_cal_clicked()
{
     this->close();
}

void mqtt_Device::on_pb_reflash_clicked()//刷新，从设备端获得参数填入
{
   QSqlQuery query(db);
   QString sql;
   QString rtuid ;
   if(ui->le_svn->text().isEmpty())
   {
       QMessageBox::critical(0,"Cannot reflesh","Uable to get dev clinet_id.",QMessageBox::Cancel);
       return ;
   }
   rtuid = ui->le_svn->text();
   sql="update rtu_param_table set  get_state = '1' where client_id ='"+rtuid+"'  ";


   if(!query.exec(sql))
   {
     qDebug()<<"query.exec failed2";
   }

}



void mqtt_Device::on_le_svn_editingFinished()
{
    ui->te_sc1->setText(strconnt(ui->te_sc1->toPlainText(),ui->le_svn->text()));
    ui->te_sc2->setText(strconnt(ui->te_sc2->toPlainText(),ui->le_svn->text()));
    ui->te_pub1->setText(strconnt(ui->te_pub1->toPlainText(),ui->le_svn->text()));
    ui->te_pub2->setText(strconnt(ui->te_pub2->toPlainText(),ui->le_svn->text()));

}

