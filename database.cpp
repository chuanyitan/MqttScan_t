#include "database.h"
#include<QSqlError>
database::database(QObject *parent) : QObject(parent)
{
   db = new QSqlDatabase;
}

database::~database()
{
    delete  db;
}

bool database::connection()
{
    QSqlDatabase db ;
    db = QSqlDatabase::addDatabase("QMYSQL");//设置使用的数据库驱动
    db.setHostName("47.93.62.22"); //数据库服务器IP
    db.setPort(3306);
    db.setUserName("root");//用户名
    db.setPassword("Admin123!");//密码
    db.setDatabaseName("mqtt_data");//使用的数据库
    qDebug()<<"DatabaseConnect";
    //打开数据库
    if(!db.open())//数据库打开失败
    {
         //QMessageBox::critical(0,"Cannot open database","Uable to establisha database connection.",QMessageBox::Cancel);
          QMessageBox::critical(0,"Cannot open database",db.lastError().text(),QMessageBox::Cancel);
         return false ;
    }
    return true;
}
