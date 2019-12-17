#include "mqttoperate.h"


MqttOperate::MqttOperate(serInfo_t *info,QObject *parent) : QObject(parent)
{
    qDebug()<<"in MqttOperate::MqttOperate";
      info_ser = info;
      if(!info_ser)
      {
         m_ip ="47.93.62.22";
         m_port = "1883";
         m_name = "admin";
         m_password = "public";
         m_client_id = "viewer_client_user";
      }
      else {
          m_ip = info->ser_ip;
          m_port = info->port;
          m_name = info->user_Name;
          m_password = info->pass_Word;
          m_client_id = info->ser_Client;
      }
      qDebug()<<m_ip <<"   "<<m_name;
      m_client = new QMqttClient;
      connct_db();
      Mqtt_oConnect();
     // timer = new QTimer;
      pubtimer = new QTimer;
      jsop = new json_op(db2);
     // connect(m_client, &QMqttClient::messageReceived, this, &MqttOperate::ReceivedDel);
     // connect(timer,&QTimer::timeout,this,&MqttOperate::deal_timeout);
      connect(pubtimer,&QTimer::timeout,this,&MqttOperate::deal_publish_timeout);
     // timer->start(1000*15);
      pubtimer->start(1000*60);
}

MqttOperate::~MqttOperate()
{
    delete  m_client;
}


bool  MqttOperate::connct_db()
{
    mutex1.lock();
    db2 = QSqlDatabase::addDatabase("QMYSQL",QString("2"));
    db2.setHostName("47.93.62.22");
    db2.setPort(3306);
    db2.setUserName("root");
    db2.setPassword("Admin123!");
    db2.setDatabaseName("mqtt_data");
    if(!db2.open())//数据库打开失败
    {
         return false ;
    }
    mutex1.unlock();
    return true;
}


/*
 * 发送数据的主题标志位
 * 0 不用发
 * 1 要发
 * p1 发主题1数据   /DL/234567/service/property/set    设置参数 下发指令内容
 * p2 发主题2数据   /DL/234567/service/property/get    询问参数 请求上传数据
 */
void MqttOperate::deal_publish_timeout()
{
     pubtimer->stop();
     QSqlQuery query(db2), query2(db2),query1(db2);
     QString rtuid,clientid,pubtopic1,pubtopic2,subtopic1,subtopic2,id;
     int ret;
    
     // subtopic2
     ///DL/100001/service/property/set
     if(query.exec(QString("select  id ,subtopic_2,client_id from rtu_param_table where set_state = 1 ") ) )
     {
          while(query.next())
          {
                    subtopic2=query.value("subtopic_2").toString();
                    qDebug()<<"fabu1dao1"<<subtopic2;
                    rtuid = query.value("client_id").toString();
                    id = query.value("id").toString();
                    ret = Mqtt_Publish(subtopic2 ,jsop->compose_ctrl_cmd_json(rtuid,"allset"));///获取即刷新
                    if(ret == 0)
                    {
                        if(query2.exec(QString("update rtu_param_table set set_state = 0  where client_id ='%1' and id ='%2'").arg(rtuid).arg(id) ) )
                        {
                            qDebug()<<"发送完成p1";
                             ret = Mqtt_Publish(subtopic2 ,jsop->compose_ctrl_cmd_json(rtuid,"save"));//发指令的主题
                        }               
                    }

          }
     }
    // subtopic1
     //发送参数配置 /DL/100001/service/property/get
     if(query.exec(QString("select id, subtopic_1,subtopic_2,client_id from rtu_param_table where get_state = 1 ") ) )//获取即刷新
     {
          while(query.next())
          {
                    subtopic2=query.value("subtopic_2").toString();
                    subtopic1=query.value("subtopic_1").toString();
                    rtuid = query.value("client_id").toString();
                    id = query.value("id").toString();
                    ret = Mqtt_Publish(subtopic1 ,jsop->compose_ctrl_cmd_json(rtuid,"all"));//获取所有参数

                    if(ret == 0)
                    {
                        if(query2.exec(QString("update rtu_param_table set get_state = 0  where client_id ='%1' and id ='%2' ").arg(rtuid).arg(id) ) )
                        {
                            qDebug()<<"发送完成p2";
                       }
                    }
          }
     }

     pubtimer->start(1000*60);


}


void MqttOperate::ReceivedDel(const QByteArray message, const QMqttTopicName topic)
{
    /*
     * /UL为上报的数据
     * /UL/100001/event/property/post         加报 数据的主题
     * /UL/100001/service/property/set_reply  参数的上报主题
    */
    /*
    qDebug()<< topic.name()<< endl;
    qDebug()<< message<<endl;


    if(topic.name().startsWith("/UL/") && topic.name().endsWith("/event/property/post"))
         jsop->parese_data_type(message,db2);
    else if( topic.name().startsWith("/UL/") && topic.name().endsWith("/service/property/set_reply")  )
         jsop->parse_param_data(message,db2);
    else
         qDebug()<<"其他类型数据："<<message<<endl;
*/
}



void MqttOperate::deal_timeout()
{
    /*
    //在定时器中进行自动去刷数据库，自动订阅主题
    timer->stop();
    QSqlQuery query(db2);
    QString rtuid,ip,time,username,password,clientid,pubtopic1,pubtopic2,subtopic1,subtopic2;
    if(query.exec(QString("select pubtopic_1,pubtopic_2 from rtu_param_table  ") ) )
    {
         while(query.next())
         {
                   pubtopic1=query.value("pubtopic_1").toString();
                   pubtopic2=query.value("pubtopic_2").toString();                 
                   qDebug()<<"pubtopic_2"<<pubtopic1;
                   qDebug()<<"pubtopic_1"<<pubtopic2;
                   Mqtt_Subscribe(pubtopic1);
                   Mqtt_Subscribe(pubtopic2);
         }
    }
    */
}




int MqttOperate::Mqtt_oConnect(QString ip,QString port,QString name,QString password,QString client_id)
{
    m_ip = ip; m_port = port ; m_name = name; m_password = password;m_client_id = client_id;
    if (m_client->state() == QMqttClient::Disconnected) {
        m_client->setHostname(m_ip);
        m_client->setPort(m_port.toInt());
        m_client->setUsername(m_name);
        m_client->setPassword(m_password);
        m_client->setClientId(m_client_id);
        m_client->connectToHost();
        return 0;
    } else {

        m_client->disconnectFromHost();
        return -1;
    }
}


int MqttOperate::Mqtt_oConnect()
{

    qDebug()<<"in Mqtt_oConnect";
    if (m_client->state() == QMqttClient::Disconnected) {
        m_client->setHostname(m_ip);
        m_client->setPort(m_port.toInt());
        m_client->setUsername(m_name);
        m_client->setPassword(m_password);
        m_client->setClientId(m_client_id);
        m_client->connectToHost();
        qDebug()<<"in connectToHost"<< m_client->state();
       // int i = Mqtt_Subscribe(QString("11121313123"));
       //  qDebug()<<"in " <<i;
        return 0;
    } else {
        m_client->disconnectFromHost();
        qDebug()<<"in disconnectFromHost";
        return -1;
    }
}

int MqttOperate::Mqtt_Publish(QString topic,QString message)
{

    if( m_client->publish(topic, message.toUtf8()) == -1 )
          return -1;
    return 0;
}



int MqttOperate::Mqtt_Subscribe(QString topic)
{
    qDebug()<<topic<<endl;
    auto subscription = m_client->subscribe(topic);
    if (!subscription) {
        qDebug()<<"in connectToHost"<< m_client->state();
          return -1;
    }
    return 0;
}


