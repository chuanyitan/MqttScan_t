#include "json_op.h"
#include <QDebug>

json_op::json_op(QSqlDatabase db,QObject *parent) : QObject(parent)
{
    db2 = db;
}


QString json_op::parese_data_type(QString data)
{
    QJsonParseError error;
    QJsonDocument doc;
    QString function;
    doc = QJsonDocument::fromJson(data.toLocal8Bit().data(), &error);
    if(error.error == QJsonParseError::NoError)
    {
              QJsonObject obj = doc.object();
              if(obj.contains("params")){
                QJsonValue value = obj.value("params");
                if(value.isObject()){
                    QJsonObject obj_2 = value.toObject();
                    if(obj_2.contains("function")){
                        QJsonValue value = obj_2.value("function");
                        if(value.isString()){
                         function  = value.toString();
                         qDebug() << function;
                        }
                      }
                }
             }
    }
    if(function == "30")
    {
        parese_upload_data(data);
    }
    else {
        parse_upload_perhour_data(data);
    }
}

QString json_op::parese_upload_data(QString data)
{
    QString rtuid,function,time;
    int water,voltage,alarm[3];
    double x_axis,y_axis;
    QJsonParseError error;
    QJsonDocument doc;
    QSqlTableModel  model ;
    QSqlQuery query(db2);
    doc = QJsonDocument::fromJson(data.toLocal8Bit().data(), &error);
    if(error.error == QJsonParseError::NoError)
    {
        {
            QJsonObject obj = doc.object();
            if(obj.contains("rtuid")){//字符串的解析
                 QJsonValue value = obj.value("rtuid");
                 if(value.isString()){
                  rtuid  = value.toString();
                  qDebug() << rtuid;
                 }
            }
            if(obj.contains("time")){
                QJsonValue value = obj.value("time");
                if(value.isString()){
                    time = value.toString();
                    qDebug() << time;
                }
            }
            if(obj.contains("params")){
              QJsonValue value = obj.value("params");
              if(value.isObject()){
                  QJsonObject obj_2 = value.toObject();
                  if(obj_2.contains("function")){
                      QJsonValue value = obj_2.value("function");
                      if(value.isString()){
                       function  = value.toString();
                       qDebug() << function;
                      }
                  }
                  if(obj_2.contains("water")){
                      QJsonValue value = obj_2.value("water");
                      if(value.isDouble()){
                       water  = value.toVariant().toInt();
                       qDebug() << water;
                      }
                  }
                  if(obj_2.contains("x_axis")){
                      QJsonValue value = obj_2.value("x_axis");
                      if(value.isDouble()){
                       x_axis  = value.toDouble();
                       qDebug() << x_axis;
                      }
                  }
                  if(obj_2.contains("y_axis")){
                      QJsonValue value = obj_2.value("y_axis");
                      if(value.isDouble()){
                       y_axis  = value.toDouble();
                       qDebug() << y_axis;
                      }
                  }
                  if(obj_2.contains("voltage")){
                      QJsonValue value = obj_2.value("voltage");
                      if(value.isDouble()){
                       voltage  = value.toVariant().toInt();
                       qDebug() << voltage;
                      }
                  }

              }
            }
            if(obj.contains("status"))
            {
                QJsonValue value = obj.value("status");
                if(value.isObject()){
                     QJsonObject obj_2 = value.toObject();
                     if(obj_2.contains("w_alarm")){
                         QJsonValue value = obj_2.value("w_alarm");
                         if(value.isDouble()){
                             alarm[WATER] = value.toVariant().toInt();
                             qDebug() << alarm[0];
                         }
                     }
                     if(obj_2.contains("c_alarm")){
                         QJsonValue value = obj_2.value("c_alarm");
                         if(value.isDouble()){
                             alarm[ANGLE] = value.toVariant().toInt();
                             qDebug() << alarm[1];
                         }
                     }
                     if(obj_2.contains("v_alarm")){
                         QJsonValue value = obj_2.value("v_alarm");
                         if(value.isDouble()){
                             alarm[VOLTOGE] = value.toVariant().toInt();
                             qDebug() << alarm[2];
                         }
                     }
                }
            }
        }
    }
    //upload内容插入数据库
    model.database().transaction() ;
    if(!query.exec(QString("insert into upload_addreport_table(client_id, data_type, time,v_alarm,c_alarm,w_alarm,water_height,x_axis,y_axis,voltage) \
                          values('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10')")\
                          .arg(rtuid).arg(function).arg(time).arg(alarm[VOLTOGE]).arg(alarm[ANGLE]) \
                          .arg(alarm[WATER]).arg(water).arg(x_axis).arg(y_axis).arg(voltage) ))
    {
        // QMessageBox::critical(this,"错误","文件中有相同的书目条码",QMessageBox::Cancel) ;
       qDebug()<<"query.exec failed";
    }
    model.database().commit() ;
    return NULL;
}

QString json_op::parse_upload_perhour_data(QString data)
{
    qDebug() << "parse_upload_perhour_data";

    QString rtuid,function,time;
    int water[12]={0},water_h=0,water_l=0,n=0,m=0,voltage;
    double clinometer_x[12],clinometer_y[12],clinometer_x_h=0,clinometer_x_l=0,clinometer_y_h=0,clinometer_y_l=0;
    QJsonParseError error;
    QJsonDocument doc;
    QSqlTableModel  model;
    doc = QJsonDocument::fromJson(data.toLocal8Bit().data(), &error);
             qDebug() << "11123333333332221";
    if(error.error == QJsonParseError::NoError)
    {
            qDebug() << "111111111111111";

        if(doc.isObject())
        {
            QJsonObject obj = doc.object();
            if(obj.contains("rtuid")){//字符串的解析
                 QJsonValue value = obj.value("rtuid");
                 if(value.isString()){
                  rtuid  = value.toString();
                  qDebug() << rtuid;
                 }
            }
            if(obj.contains("time")){
                QJsonValue value = obj.value("time");
                if(value.isString()){
                    time = value.toString();
                    qDebug() << time;
                }
            }
            if(obj.contains("params")){
              QJsonValue value = obj.value("params");
              if(value.isObject()){
                  QJsonObject obj_2 = value.toObject();
                  if(obj_2.contains("function")){
                      QJsonValue value = obj.value("function");
                      if(value.isString()){
                       function  = value.toString();
                       qDebug() << function;
                      }
                  }
                 if(obj_2.contains("voltage")){
                  QJsonValue value = obj.value("voltage");
                  if(value.isDouble()){
                   voltage  = value.toVariant().toInt();
                   qDebug() << voltage;
                  }
                 }
                  if(obj_2.contains("water")){
                      QJsonValue value = obj_2.value("water");
                      if(value.isArray()){//为12组数据
                      QJsonArray array = value.toArray();
                      int nSize = array.size();
                      qDebug()<< "nsize"<< nSize;
                      for (int i = 0; i < nSize; ++i) {
                         value = array.at(i);
                         if (value.isDouble()) {
                         water[i] = value.toVariant().toInt();
                         if(water[i]>water_h)
                                water_h=water[i];
                         if(water[i]<water_l)
                                water_l=water[i];
                         qDebug() << "water" +QString::number(i) +": " << water[i];
                         }
                       }
                     }
                  }
                  if(obj_2.contains("clinometer")){
                      QJsonValue value = obj_2.value("clinometer");
                      if(value.isArray()){//为12组数据
                      QJsonArray array = value.toArray();
                      int nSize = array.size();
                      for (int i = 0; i < nSize; ++i) {
                         value = array.at(i);
                         if (value.isDouble()) {
                            if((i+1)%2==1){
                                 clinometer_x[n] = value.toVariant().toInt();
                                 if(clinometer_x[i]>clinometer_x_h)
                                        clinometer_x_h=clinometer_x[i];
                                 if(clinometer_x[i]<clinometer_x_l)
                                        clinometer_x_l=clinometer_x[i];
                                 n++;
                                 qDebug() << "clinometer" +QString::number(n) +": " << clinometer_x[n];
                             }
                             else {
                                 clinometer_y[m] = value.toVariant().toInt();
                                 if(clinometer_y[i]>clinometer_y_h)
                                        clinometer_y_h=clinometer_y[i];
                                 if(clinometer_x[i]<clinometer_y_l)
                                        clinometer_y_l=clinometer_x[i];
                                 m++;
                                 qDebug() << "clinometer" +QString::number(m) +": " << clinometer_y[m];
                             }
                         }
                       }
                     }
                  }

             }

          }
      }
    }

    //把记录的小时数据全部插入数据库
    QSqlQuery query(db2);
            /*
    model.database().transaction();
    if(!query.exec(QString("insert into upload_perhour_table(client_id,data_type,time,water1,water2,water3,water4,water5,water6,water7,water8,water9,water10,water11,water12, \
                           clinometer_x1,clinometer_x2,clinometer_x3,clinometer_x4,clinometer_x5,clinometer_x6,clinometer_x7,clinometer_x8,clinometer_x9,clinometer_x10,clinometer_x11,clinometer_x12,\
                           clinometer_y1,clinometer_y2,clinometer_y3,clinometer_y4,clinometer_y5,clinometer_y6,clinometer_y7,clinometer_y8,clinometer_y9,clinometer_y10,clinometer_y11,clinometer_y12,\
                           voltage,water_h,water_l,clinometer_x_h,clinometer_x_l,clinometer_y_h,clinometer_y_l) values('%1','%2','%3','%4','%5','%6','%7',\"%8\",\
                           '%9','%10','%11','%12','%13','%14','%15','%16','%17','%18','%19','%20','%21','%22','%23','%24','%25','%26''%27','%28','%29','%30','%31','%32','%33','%34','%35'\
                           '%35','%36','%37','%38','%39','%40','%41','%42','%43''%44','%44','%45','%46';)")\
                          .arg(rtuid).arg(function).arg(time)\
                          .arg(water[0]).arg(water[1]).arg(water[2]).arg(water[3]).arg(water[4]).arg(water[5]).arg(water[6]).arg(water[7]).arg(water[8]).arg(water[9]).arg(water[10]).arg(water[11])\
                          .arg(clinometer_x[0]).arg(clinometer_x[1]).arg(clinometer_x[2]).arg(clinometer_x[3]).arg(clinometer_x[4]).arg(clinometer_x[5]).arg(clinometer_x[6]).arg(clinometer_x[7]).arg(clinometer_x[8]).arg(clinometer_x[9]).arg(clinometer_x[10]).arg(clinometer_x[11])\
                          .arg(clinometer_y[0]).arg(clinometer_y[1]).arg(clinometer_y[2]).arg(clinometer_y[3]).arg(clinometer_y[4]).arg(clinometer_y[5]).arg(clinometer_y[6]).arg(clinometer_y[7]).arg(clinometer_y[8]).arg(clinometer_y[9]).arg(clinometer_y[10]).arg(clinometer_y[11])\
                          .arg(voltage).arg(water_h).arg(water_l).arg(clinometer_x_h).arg(clinometer_x_l).arg(clinometer_y_h).arg(clinometer_y_l)))
    {
       qDebug()<<"query failed";
    }
    model.database().commit();
            */
    return NULL;
}




/*
 * if(!query.exec(QString("insert into upload_perhour_table(client_id,data_type,water1,water2,water3,water4,water5,water6,water7,water8,water9,water10,water11,water12, \
                                   clinometer_x1,clinometer_x2,clinometer_x3,clinometer_x4,clinometer_x5,clinometer_x6,clinometer_x7,clinometer_x8,clinometer_x9,clinometer_x10,clinometer_x11,clinometer_x12,\
                                   clinometer_y1,clinometer_y2,clinometer_y3,clinometer_y4,clinometer_y5,clinometer_y6,clinometer_y7,clinometer_y8,clinometer_y9,clinometer_y10,clinometer_y11,clinometer_y12,\
                                   voltage1,voltage2,voltage3,voltage4,voltage5,voltage6,voltage7,voltage8,voltage9,voltage10,voltage11,voltage12) values('%1','%2','%3','%4','%5','%6','%7',\"%8\",\
                                   '%9','%10','%11','%12','%13','%14','%15','%16','%17','%18','%19','%20','%21','%22','%23','%24','%25','%26''%27','%28','%29','%30','%31','%32','%33','%34','%35'\
                                   '%35','%36','%37','%38','%39','%40','%41','%42','%43''%44','%44','%45','%46','%47','%48','%49','%50')")\
                                  .arg(rtuid).arg(function)\
                                  .arg(water[1]).arg(water[2]).arg(water[3]).arg(water[4]).arg(water[5]).arg(water[6]).arg(water[7]).arg(water[8]).arg(water[9]).arg(water[10]).arg(water[11]).arg(water[12])\
                                  .arg(clinometer_x[1]).arg(clinometer_x[2]).arg(clinometer_x[3]).arg(clinometer_x[4]).arg(clinometer_x[5]).arg(clinometer_x[6]).arg(clinometer_x[7]).arg(clinometer_x[8]).arg(clinometer_x[9]).arg(clinometer_x[10]).arg(clinometer_x[11]).arg(clinometer_x[12])\
                                  .arg(clinometer_y[1]).arg(clinometer_y[2]).arg(clinometer_y[3]).arg(clinometer_y[4]).arg(clinometer_y[5]).arg(clinometer_y[6]).arg(clinometer_y[7]).arg(clinometer_y[8]).arg(clinometer_y[9]).arg(clinometer_y[10]).arg(clinometer_y[11]).arg(clinometer_y[12])\
                                  .arg(voltage).arg(time) ) )
            {
 */



QString json_op::parse_param_data(QString data)
{
    QString rtuid,ip,time,username,password,clientid,pubtopic1,pubtopic2,subtopic1,subtopic2;
    int port,heartbeat,water_sample,water_alarm,clinometer_sample,clinometer_alarm,voltage_sample,voltage_alarm;
    QJsonParseError error;
    QSqlQuery query(db2);
    QSqlTableModel  model;
    QJsonDocument doc;
    doc = QJsonDocument::fromJson(data.toLocal8Bit().data(), &error);
    if(error.error == QJsonParseError::NoError)
    {
        if(doc.isObject())
        {
            QJsonObject obj = doc.object();
            if(obj.contains("rtuid")){//字符串的解析
                 QJsonValue value = obj.value("rtuid");
                 if(value.isString()){
                  rtuid  = value.toString();
                  qDebug() << rtuid;
                 }
            }
            if(obj.contains("params")){
              QJsonValue value = obj.value("params");
              if(value.isObject()){
                  QJsonObject obj_2 = value.toObject();
                  if(obj_2.contains("ip")){
                      QJsonValue value = obj_2.value("ip");
                      if(value.isString()){
                       ip  = value.toString();
                       qDebug() << ip;
                      }
                  }
                  if(obj_2.contains("port")){
                      QJsonValue value = obj_2.value("port");
                      if(value.isDouble()){
                       port  = value.toVariant().toInt();
                       qDebug() << port;
                      }
                  }
                  if(obj_2.contains("heartbeat")){
                      QJsonValue value = obj_2.value("heartbeat");
                      if(value.isDouble()){
                       heartbeat  = value.toVariant().toInt();
                       qDebug() << heartbeat;
                      }
                  }
                  if(obj_2.contains("water")){
                      QJsonValue value = obj_2.value("water");
                      if(value.isObject()){
                          QJsonObject obj_3 = value.toObject();
                          if(obj_3.contains("water_sample")){
                              QJsonValue value_3 = obj_3.value("water_sample");
                              if(value_3.isDouble()){
                                  water_sample = value_3.toVariant().toInt();
                                  qDebug() << water_sample;
                              }
                          }
                          if(obj_3.contains("water_alarm")){
                              QJsonValue value_3 = obj_3.value("water_alarm");
                              if(value_3.isDouble()){
                                  water_alarm = value_3.toVariant().toInt();
                                  qDebug() << water_alarm;
                              }
                          }
                      }
                  }
                  if(obj_2.contains("voltage")){
                      QJsonValue value = obj_2.value("voltage");
                      if(value.isObject()){
                          QJsonObject obj_3 = value.toObject();
                          if(obj_3.contains("voltage_sample")){
                              QJsonValue value_3 = obj_3.value("voltage_sample");
                              if(value_3.isDouble()){
                                  voltage_sample = value_3.toVariant().toInt();
                                  qDebug() << voltage_sample;
                              }
                          }
                          if(obj_3.contains("voltage_alarm")){
                              QJsonValue value_3 = obj_3.value("voltage_alarm");
                              if(value_3.isDouble()){
                                  voltage_alarm = value_3.toVariant().toInt();
                                  qDebug() << voltage_alarm;
                              }
                          }
                      }
                  }
                  if(obj_2.contains("clinometer")){
                      QJsonValue value = obj_2.value("clinometer");
                      if(value.isObject()){
                          QJsonObject obj_3 = value.toObject();
                          if(obj_3.contains("clinometer_sample")){
                              QJsonValue value_3 = obj_3.value("clinometer_sample");
                              if(value_3.isDouble()){
                                  clinometer_sample = value_3.toVariant().toInt();
                                  qDebug() << clinometer_sample;
                              }
                          }
                          if(obj_3.contains("clinometer_alarm")){
                              QJsonValue value_3 = obj_3.value("clinometer_alarm");
                              if(value_3.isDouble()){
                                  clinometer_alarm = value_3.toVariant().toInt();
                                  qDebug() << clinometer_alarm;
                              }
                          }
                      }
                  }
                  if(obj_2.contains("mqttAuth")){
                      QJsonValue value = obj_2.value("mqttAuth");
                      if(value.isObject()){
                          QJsonObject obj_3 = value.toObject();
                          if(obj_3.contains("username")){
                              QJsonValue value_3 = obj_3.value("username");
                              if(value_3.isString()){
                                  username = value_3.toString();
                                  qDebug() << username;
                              }
                          }
                          if(obj_3.contains("password")){
                              QJsonValue value_3 = obj_3.value("password");
                              if(value_3.isString()){
                                  password = value_3.toString();
                                  qDebug() << password;
                              }
                          }
                          if(obj_3.contains("clientid")){
                              QJsonValue value_3 = obj_3.value("clientid");
                              if(value_3.isString()){
                                  clientid = value_3.toString();
                                  qDebug() << clientid;
                              }
                          }
                          if(obj_3.contains("pubtopic1")){
                              QJsonValue value_3 = obj_3.value("pubtopic1");
                              if(value_3.isString()){
                                  pubtopic1 = value_3.toString();
                                  qDebug() << pubtopic1;
                              }
                          }
                          if(obj_3.contains("pubtopic2")){
                              QJsonValue value_3 = obj_3.value("pubtopic2");
                              if(value_3.isString()){
                                  pubtopic2 = value_3.toString();
                                  qDebug() << pubtopic2;
                              }
                          }
                          if(obj_3.contains("subtopic1")){
                              QJsonValue value_3 = obj_3.value("subtopic1");
                              if(value_3.isString()){
                                  subtopic1 = value_3.toString();
                                  qDebug() << subtopic1;
                              }
                          }
                          if(obj_3.contains("subtopic2")){
                              QJsonValue value_3 = obj_3.value("subtopic2");
                              if(value_3.isString()){
                                  subtopic2 = value_3.toString();
                                  qDebug() << subtopic2;
                              }

                          }
                      }
                  }

                }
            }
        }
    }
    //param参数更改内容插入数据库
    model.database().transaction();
    if(!query.exec(QString("insert into rtu_param_table(client_id,server_ip,server_port, \
                           heartbeat,username,password,clientid,pubtopic_1,pubtopic_2,subtopic_1,subtopic_2,water_sample,water_alarm,\
                           clinometer_sample,clinometer_alarm,voltage_sample,voltage_alarm) values('%1','%2','%3','%4','%5','%6','%7',\"%8\",\
                           \"%9\",\"%10\",\"%11\",'%12','%13','%14','%15','%16','%17')")\
                          .arg(rtuid).arg(ip).arg(port).arg(heartbeat)\
                          .arg(username).arg(password).arg(clientid).arg(pubtopic1).arg(pubtopic2)  \
                          .arg(subtopic1).arg(subtopic2).arg(water_sample).arg(water_alarm).arg(clinometer_sample) \
                          .arg(clinometer_alarm).arg(voltage_sample).arg(voltage_alarm) ) )
    {
       qDebug()<<"query.exec failed";
    }
    model.database().commit();
    return NULL;
}

QString json_op::compose_paramset_cmd_json(QString rtuid)
{
    QJsonObject json;
    json.insert("rtuid", rtuid);
    json.insert("params", "all");
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);

    QString strJson(byteArray);
    qDebug() << strJson;

    return strJson;
}

QString json_op::compose_paramget_cmd_json(QString rtuid)
{
    QJsonObject json;
    json.insert("rtuid", rtuid);
    json.insert("params", "all");
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);

    QString strJson(byteArray);
    qDebug() << strJson;

    return strJson;
}

QString json_op::compose_ctrl_cmd_json(QString rtuid,QString cmd)
{
    QJsonObject json,json1,json2;
    QSqlQuery query(db2);
    QString water_sample,water_alarm,clinometer_sample,clinometer_alarm;
    if(cmd == "allset")
    {    QSqlQuery query(db2);
         QString water_sample,water_alarm,clinometer_sample,clinometer_alarm;
         if(query.exec(QString("select water_sample,water_alarm,clinometer_sample,clinometer_alarm from rtu_param_table where client_id = '%1' ").arg(rtuid) ) )//获取即刷新
         {
              while(query.next())
                 {
                           water_sample=query.value("water_sample").toString();
                           water_alarm=query.value("water_alarm").toString();
                           clinometer_sample = query.value("clinometer_sample").toString();
                           clinometer_alarm = query.value("clinometer_alarm").toString();
                 }
          }

/*
        json.insert("rtuid", rtuid);
        json.insert("params", QJsonValue(json1));//把这里的参数全部组成包，下发
        json1.insert("mqttAuth",QJsonValue(json2));
        json2.insert("water_sample", water_sample);
        json2.insert("water_alarm", water_alarm);
        json2.insert("clinometer_sample", clinometer_sample);
        json2.insert("clinometer_alarm", clinometer_alarm);
*/
        QString data = QString("{\"rtuid\":\"%1\",\"params\":{\"water\":{\"water_sample\":\"%2\",\"water_alarm\":\"%3\"},\"clinometer\":{\"clinometer_sample\":\"%4\",\"clinometer_alarm\":\"%5\"}}}").arg(rtuid).arg(water_sample).arg(water_alarm).arg(clinometer_sample).arg(clinometer_alarm);
        qDebug() << data;
        return data;
    }
    else {
        json.insert("rtuid", rtuid);
        json.insert("params", cmd);//把这里的参数全部组成包，下发
    }
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);

    QString strJson(byteArray);
    qDebug() << strJson;
    return strJson;
}
