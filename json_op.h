#ifndef JSON_OP_H
#define JSON_OP_H

#include <QObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QSqlQuery>
#include <QSqlTableModel>

enum ALARM_TYPE
{
    WATER,
    ANGLE,
    VOLTOGE,
};

class json_op : public QObject
{
    Q_OBJECT
public:
    explicit json_op(QSqlDatabase db,QObject *parent = nullptr);
    QString parese_upload_data(QString data);
    QString parse_upload_perhour_data(QString data);
    QString parse_param_data(QString data);
    QString compose_paramset_cmd_json(QString rtuid);
    QString compose_paramget_cmd_json(QString rtuid);
    QString compose_ctrl_cmd_json(QString rtuid,QString cmd);
    QString parese_data_type(QString data);
    QSqlDatabase db2;

signals:

public slots:
};

#endif // JSON_OP_H
