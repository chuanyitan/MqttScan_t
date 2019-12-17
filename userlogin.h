#ifndef USERLOGIN_H
#define USERLOGIN_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class UserLogin;
}

class UserLogin : public QDialog
{
    Q_OBJECT

public:
    explicit UserLogin( QSqlDatabase db_t,QWidget *parent = nullptr);
    ~UserLogin();

private:
    Ui::UserLogin *ui;
    QSqlDatabase db;

private slots:
    void on_login_clicked();

signals:
    void sendloginno(QString,int);
};

#endif // USERLOGIN_H
