#ifndef MQTT_THREAD_H
#define MQTT_THREAD_H

#include <QObject>
#include <QThread>
#include "mqttoperate.h"



class mqtt_thread : public QThread
{
    Q_OBJECT
public:
    explicit mqtt_thread(serInfo_t *info,QObject *parent = nullptr);
    MqttOperate *mop;

signals:
    void over();

protected:
    void run();

public slots:
private:
     serInfo_t *info_thread;
};

#endif // MQTT_THREAD_H
