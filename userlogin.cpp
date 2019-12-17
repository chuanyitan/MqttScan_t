#include "userlogin.h"
#include "ui_userlogin.h"
#include<QDebug>
#include<QSqlQuery>
#include<QMessageBox>

UserLogin::UserLogin(QSqlDatabase db_t,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserLogin)
{
    ui->setupUi(this);
    db = db_t;
    ui->user->setClearButtonEnabled(true);
    ui->user->setPlaceholderText("账号");
    ui->password->setPlaceholderText("密码");
    ui->password->setClearButtonEnabled(true);
    ui->password->setEchoMode(QLineEdit::Password);
    this->setWindowTitle("用户登录");
    ui->user->setFocus();
    ui->userpicture->setPixmap(QPixmap(":/picture/userlogin.png"));
    ui->userpicture->setScaledContents(true);
    this->setWindowIcon(QPixmap(":/picture/userlogin.png"));
    this->setFixedSize(this->width(),this->height());
}

UserLogin::~UserLogin()
{
    delete ui;
}

void UserLogin::on_login_clicked()
{
    QString userid=ui->user->text();
    QString passwd=ui->password->text();
    QString username;
//    QByteArray bytePwd = passwd.toLatin1();//MD5进行加密
//    QByteArray bytePwdMd5 = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
//    QString passwdMd5 = bytePwdMd5.toHex();

    if(userid.isEmpty()||passwd.isEmpty())
    {
        QMessageBox::information(this,tr("提示"),tr("请输入完整信息"),QMessageBox::Ok);
        return;
    }
    QSqlQuery query(db),query1(db);
    query.prepare("select *from user_log where userID=:uid ");
    query.bindValue(":uid",userid);
    query.exec();
    if(query.next())
    {
       QString  passwdindatabase = query.value(2).toString();
       username= query.value(1).toString();
       qDebug()<<username;
       if(passwdindatabase == passwd)
       {
            query1.exec(QString("update user_log set state = '1' where userID ='%1' and username='%2';").arg(userid).arg(username));
            emit sendloginno(username,userid.toInt());
            this->hide();

       }
       else
       {
           QMessageBox::information(this,tr("提示"),tr("请核对用户名和密码！"),QMessageBox::Ok);
           ui->password->clear();
           ui->user->clear();
           ui->user->setFocus();
       }
    }
}
