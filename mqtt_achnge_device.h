#ifndef MQTT_ACHNGE_DEVICE_H
#define MQTT_ACHNGE_DEVICE_H

#include <QDialog>

namespace Ui {
class mqtt_achnge_Device;
}

class mqtt_achnge_Device : public QDialog
{
    Q_OBJECT

public:
    explicit mqtt_achnge_Device(int number,QWidget *parent = nullptr);
    ~mqtt_achnge_Device();

private:
    Ui::mqtt_achnge_Device *ui;
};

#endif // MQTT_ACHNGE_DEVICE_H
