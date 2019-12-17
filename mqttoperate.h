#ifndef MQTTOPERATE_H
#define MQTTOPERATE_H

#include <QObject>
#include <QtMqtt/qmqttclient.h>
#include <QDebug>
#include <QSqlQuery>
#include <QTimer>
#include <QMessageBox>
#include <QMutex>
#include <QThread>
#include "database.h"
#include "json_op.h"


typedef  struct  serverInfo{
   QString port;
   QString ser_ip;
   QString ser_Client;
   QString user_Name;
   QString pass_Word;
}serInfo_t;;


class MqttOperate : public QObject
{
    Q_OBJECT
public:
    explicit MqttOperate(serInfo_t *info,QObject *parent = nullptr);
    int  Mqtt_oConnect(QString ip,QString port,QString name,QString password,QString client_id);
    int  Mqtt_oConnect();
    int  Mqtt_Publish(QString topic,QString message);
    int  Mqtt_Subscribe(QString topic);
    bool connct_db();
    ~MqttOperate();

signals:

public slots:

private:
    QMqttClient *m_client;
    QString m_ip, m_port, m_name, m_password, m_client_id;
    database *db;
    QSqlQuery query;
    QTimer *timer;
    QTimer *pubtimer;
    QSqlDatabase db2;
    QMutex mutex1;
    json_op *jsop;
    serInfo_t *info_ser;

private slots:
    void ReceivedDel(QByteArray message,QMqttTopicName topic);
    void deal_timeout();
    void deal_publish_timeout();
};

#endif // MQTTOPERATE_H
