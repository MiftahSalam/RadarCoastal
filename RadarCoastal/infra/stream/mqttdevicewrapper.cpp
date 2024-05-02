#include "mqttdevicewrapper.h"

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, MqttDeviceWrapper)
#else
#include <QDebug>
#endif

QMap<QString, MqttDeviceWrapper*> MqttDeviceWrapper::m_wrappers;

MqttDeviceWrapper::MqttDeviceWrapper(QObject *parent):
    DeviceWrapper(parent), m_idCounter(0)
{
    m_publisher = nullptr;
    m_subsciber = nullptr;

#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO;
#else
    qDebug()<<Q_FUNC_INFO;
#endif
}

MqttDeviceWrapper::~MqttDeviceWrapper()
{
    delete m_publisher;
    delete  m_subsciber;
    m_wrappers.clear();
}
MqttDeviceWrapper* MqttDeviceWrapper::GetInstance(const QString config)
{
#ifdef USE_LOG4QT
    logger()->debug()<<Q_FUNC_INFO<<"config"<<config;
#else
    qDebug()<<Q_FUNC_INFO<<"config"<<config;
#endif
    if(!m_wrappers.contains(extractBrokerUrl(config)))
    {
        MqttDeviceWrapper* wrapper = new MqttDeviceWrapper(nullptr);
        bool wrap_init = wrapper->InitConfig(config);
        if(!wrap_init)
        {
            delete wrapper;
            return nullptr;
        }
        m_wrappers.insert(config,wrapper);
    }
    else
    {
        if(config.isEmpty()) return nullptr;
    }

    return m_wrappers.value(config);
}

bool MqttDeviceWrapper::InitConfig(const QString config)
{
    bool ret_val = false;
#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
    QStringList config_list = config.split(":",Qt::SkipEmptyParts);
#else
    QStringList config_list = config.split(":",QString::SkipEmptyParts);
#endif
    if(config_list.size() == 3)
    {
        ret_val = true;

        m_mqttConfig.host = QHostAddress(config_list.at(0));
        m_mqttConfig.port = config_list.at(1).toUShort();
        m_defaultTopic = config_list.at(2);

        if(m_publisher) delete m_publisher;
        if(m_subsciber) delete m_subsciber;

        m_publisher = new Publisher(this,m_mqttConfig.host,m_mqttConfig.port, m_defaultTopic);
        m_subsciber = new Subscriber(this,m_mqttConfig.host,m_mqttConfig.port, m_defaultTopic);
        connect(m_subsciber,&Subscriber::SignalOnReceived, this, &MqttDeviceWrapper::receiveData);
    }
    else
    {
#ifdef USE_LOG4QT
        logger()->warn()<<Q_FUNC_INFO<<"invalid config"<<config;
#else
        else qDebug()<<Q_FUNC_INFO<<"invalid config"<<config;
#endif
    }

    return ret_val;
}

void MqttDeviceWrapper::Reconnect()
{
    if(!m_publisher->isConnectedToHost()) m_publisher->connectToHost();
    if(!m_subsciber->isConnectedToHost()) m_subsciber->connectToHost();
}

DeviceWrapper::DeviceStatus MqttDeviceWrapper::GetStatus()
{
    if(m_publisher->isConnectedToHost() || m_subsciber->isConnectedToHost())
    {
        qint64 now = QDateTime::currentSecsSinceEpoch();

        if((now-m_last_data_time) > 10) m_currentStatus = DeviceWrapper::NO_INPUT_DATA;
        else m_currentStatus = DeviceWrapper::INPUT_AVAIL;
    }
    else m_currentStatus = DeviceWrapper::NOT_AVAIL;

    return  m_currentStatus;
}

void MqttDeviceWrapper::receiveData(QMQTT::Message message)
{
    QString payload = QString::fromUtf8(message.payload());
    QString topic = message.topic();

#ifdef USE_LOG4QT
        logger()->trace()<<Q_FUNC_INFO<<" topic: "<<topic<<", payload: "<<payload;
#else
    qDebug()<<Q_FUNC_INFO<<"payload"<<payload<<"topic"<<topic;
#endif

//    m_last_data_time = QDateTime::currentSecsSinceEpoch();
    emit ReadyRead(topic+MQTT_MESSAGE_SEPARATOR+payload);
}


QString MqttDeviceWrapper::extractBrokerUrl(QString config)
{
#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
    QStringList config_list = config.split(":",Qt::SkipEmptyParts);
#else
    QStringList config_list = config.split(":",QString::SkipEmptyParts);
#endif
    QStringList config_list_broker = config_list.mid(0,2);

    return config_list_broker.join(":");
}

void MqttDeviceWrapper::ChangeConfig(const QString command)
{
    QStringList cmd_list = command.split(":");
    if(cmd_list.size() != 3)
    {
        qWarning()<<Q_FUNC_INFO<<"invalid command data";
        return;
    }

    if(cmd_list.at(0) == "publisher")
    {
        if(cmd_list.at(1) == "topic-add")
        {
            m_publisher->AddTopic(cmd_list.at(2));
        }
        else if(cmd_list.at(1) == "topic-rm")
        {
            m_publisher->RemoveTopic(cmd_list.at(2));
        }
        else qWarning()<<Q_FUNC_INFO<<"invalid command item"<<cmd_list.at(1);
    }
    else if(cmd_list.at(0) == "subsciber")
    {
        if(cmd_list.at(1) == "topic-add")
        {
            m_subsciber->AddTopic(cmd_list.at(2));
        }
        else if(cmd_list.at(1) == "topic-rm")
        {
            m_subsciber->RemoveTopic(cmd_list.at(2));
        }
        else qWarning()<<Q_FUNC_INFO<<"invalid command item"<<cmd_list.at(1);
    }
    else qWarning()<<Q_FUNC_INFO<<"invalid command instance"<<cmd_list.at(0);
}

void MqttDeviceWrapper::Write(const QString data)
{
#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
    QStringList format = data.split(MQTT_MESSAGE_SEPARATOR, Qt::SkipEmptyParts);
#else
    QStringList format = data.split(MQTT_MESSAGE_SEPARATOR, QString::SkipEmptyParts);
#endif

    if(format.size() == 2)
    {
        QMQTT::Message message(m_idCounter,format.at(0),format.at(1).toUtf8());
        m_publisher->PublishData(message);
        return;
    }
#ifdef USE_LOG4QT
    else logger()->warn()<<Q_FUNC_INFO<<"invalid mqtt data"<<data;
#else
    else qDebug()<<Q_FUNC_INFO<<"invalid mqtt data"<<data;
#endif
}

Subscriber::Subscriber(QObject *parent, const QHostAddress& host, const quint16 port, QString topic) :
    MqttClient(parent,host,port,topic)
{
    connect(this,&Subscriber::received,this,&Subscriber::SignalOnReceived);
}

Publisher::Publisher(QObject *parent, const QHostAddress& host, const quint16 port, QString topic) :
    MqttClient (parent,host,port,topic)
{
}

void Publisher::PublishData(QMQTT::Message message)
{
    publish(message);
}

MqttClient::MqttClient(QObject *parent,
                       const QHostAddress& host,
                       const quint16 port, QString topic) :
    QMQTT::Client(host,port,parent), m_host(host), m_port(port)
{    
    connect(this,&MqttClient::connected,this,&MqttClient::onConnected);
    connect(this,&MqttClient::disconnected,this,&MqttClient::onDisconnected);
    connect(this,&MqttClient::subscribed,this,&MqttClient::onSubscribed);

    connectToHost();

#ifdef USE_LOG4QT
    logger()->debug()<<Q_FUNC_INFO<<"topic"<<topic;
#else
    qDebug()<<Q_FUNC_INFO<<"topic"<<topic;
#endif
    if(!topic.isEmpty()) AddTopic(topic);
    else m_topic_list.clear();
}

void MqttClient::AddTopic(const QString topic)
{
    if(!m_topic_list.contains(topic))
    {
#ifdef USE_LOG4QT
        logger()->debug()<<Q_FUNC_INFO<<"topic"<<topic;
#else
        qDebug()<<Q_FUNC_INFO<<"topic"<<topic;
#endif
        m_topic_list.append(topic);
        subscribe(topic);
    }
}

void MqttClient::RemoveTopic(const QString topic)
{
    m_topic_list.removeOne(topic);
    unsubscribe(topic);
}

void MqttClient::onConnected()
{
    foreach (QString topic, m_topic_list)
    {
        subscribe(topic);
    }
#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO;
#else
    qDebug()<<Q_FUNC_INFO;
#endif
}

void MqttClient::onDisconnected()
{
    foreach (QString topic, m_topic_list)
    {
        unsubscribe(topic);
    }
#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO;
#else
    qDebug()<<Q_FUNC_INFO;
#endif
}

void MqttClient::onSubscribed(const QString& topic, const quint8 qos)
{
#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO<<"topic"<<topic<<"qos"<<qos;
#else
    qDebug()<<Q_FUNC_INFO<<"topic"<<topic<<"qos"<<qos;
#endif
}
