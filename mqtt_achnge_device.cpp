#include "mqtt_achnge_device.h"
#include "ui_mqtt_achnge_device.h"

mqtt_achnge_Device::mqtt_achnge_Device(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mqtt_achnge_Device)
{
    ui->setupUi(this);
}

mqtt_achnge_Device::~mqtt_achnge_Device()
{
    delete ui;
}
