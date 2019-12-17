#include "mqtt_thread.h"
#include <QDebug>

mqtt_thread::mqtt_thread(serInfo_t *info,QObject *parent) : QThread(parent)
{
    info_thread = info;
}

void mqtt_thread::run()
{
     qDebug()<<"in mqtt_thread:run";
      mop =  new MqttOperate(info_thread);
      exec();

}
