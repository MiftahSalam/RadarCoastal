#include "mqttdevicewrapper.h"

QMap<QString, MqttDeviceWrapper*> MqttDeviceWrapper::_wrappers;

MqttDeviceWrapper::MqttDeviceWrapper(QObject *parent, QString config) :
    DeviceWrapper(parent,config), _idCounter(0)
{
    qDebug()<<Q_FUNC_INFO<<"config"<<config;

    initConfig(config);

    _publisher = new Publisher(this,_mqttConfig.host,_mqttConfig.port, _defaultTopic);
    _subsciber = new Subscriber(this,_mqttConfig.host,_mqttConfig.port, _defaultTopic);
    connect(_subsciber,&Subscriber::signal_onReceived, this, &MqttDeviceWrapper::receiveData);
}

MqttDeviceWrapper::~MqttDeviceWrapper()
{
    delete _publisher;
    delete  _subsciber;
    _wrappers.clear();
}
MqttDeviceWrapper* MqttDeviceWrapper::getInstance(const QString config)
{
    qDebug()<<Q_FUNC_INFO<<"config"<<config;
    if(!_wrappers.contains(config))
    {
        _wrappers.insert(config,new MqttDeviceWrapper(nullptr, config));
    }
    else
    {
        if(config.isEmpty()) return nullptr;
    }

    return _wrappers.value(config);
}

void MqttDeviceWrapper::initConfig(const QString config)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
    QStringList config_list = config.split(":",QString::SkipEmptyParts);
#else
    QStringList config_list = config.split(":",Qt::SkipEmptyParts);
#endif
    if(config_list.size() == 3)
    {
        _mqttConfig.host = QHostAddress(config_list.at(0));
        _mqttConfig.port = config_list.at(1).toUShort();
        _defaultTopic = config_list.at(2);
    }
    else qDebug()<<Q_FUNC_INFO<<"invalid config"<<config;
}

DeviceWrapper::DeviceStatus MqttDeviceWrapper::getStatus()
{
    if(_publisher->isConnectedToHost() || _subsciber->isConnectedToHost())
    {
        qint64 now = QDateTime::currentSecsSinceEpoch();

        if((now-last_data_time) > 10) _currentStatus = DeviceWrapper::NO_INPUT_DATA;
        else _currentStatus = DeviceWrapper::INPUT_AVAIL;
    }
    else _currentStatus = DeviceWrapper::NOT_AVAIL;

    return  _currentStatus;
}

void MqttDeviceWrapper::receiveData(QMQTT::Message message)
{
    QString payload = QString::fromUtf8(message.payload());
    QString topic = message.topic();
//    _currentData = payload;
    qWarning()<<Q_FUNC_INFO<<"payload"<<payload<<"topic"<<topic;
    last_data_time = QDateTime::currentSecsSinceEpoch();
    emit readyRead(topic+":"+payload);
}

void MqttDeviceWrapper::changeConfig(const QString command)
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
            _publisher->addTopic(cmd_list.at(2));
        }
        else if(cmd_list.at(1) == "topic-rm")
        {
            _publisher->removeTopic(cmd_list.at(2));
        }
        else qWarning()<<Q_FUNC_INFO<<"invalid command item"<<cmd_list.at(1);
    }
    else if(cmd_list.at(0) == "subsciber")
    {
        if(cmd_list.at(1) == "topic-add")
        {
            _subsciber->addTopic(cmd_list.at(2));
        }
        else if(cmd_list.at(1) == "topic-rm")
        {
            _subsciber->removeTopic(cmd_list.at(2));
        }
        else qWarning()<<Q_FUNC_INFO<<"invalid command item"<<cmd_list.at(1);
    }
    else qWarning()<<Q_FUNC_INFO<<"invalid command instance"<<cmd_list.at(0);
}

void MqttDeviceWrapper::write(const QString data)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
    QStringList format = data.split(":",QString::SkipEmptyParts);
#else
    QStringList format = data.split(":",Qt::SkipEmptyParts);
#endif
    if(format.size() == 2)
    {
        QMQTT::Message message(_idCounter,format.at(0),format.at(1).toUtf8());
        _publisher->publishData(message);
    } else qDebug()<<Q_FUNC_INFO<<"invalid mqtt data"<<data;
}

Subscriber::Subscriber(QObject *parent, const QHostAddress& host, const quint16 port, QString topic) :
    MqttClient(parent,host,port,topic)
{
    connect(this,&Subscriber::received,this,&Subscriber::signal_onReceived);
}

Publisher::Publisher(QObject *parent, const QHostAddress& host, const quint16 port, QString topic) :
    MqttClient (parent,host,port,topic)
{
}

void Publisher::publishData(QMQTT::Message message)
{
    publish(message);
}

MqttClient::MqttClient(QObject *parent,
                                        const QHostAddress& host,
                                        const quint16 port, QString topic) :
    QMQTT::Client(host,port,parent), _host(host), _port(port)
{    
    connect(this,&MqttClient::connected,this,&MqttClient::onConnected);
    connect(this,&MqttClient::disconnected,this,&MqttClient::onDisconnected);
    connect(this,&MqttClient::subscribed,this,&MqttClient::onSubscribed);

    connectToHost();

    qDebug()<<Q_FUNC_INFO<<topic;
    if(!topic.isEmpty()) addTopic(topic);
    else _topic_list.clear();
}

void MqttClient::addTopic(const QString topic)
{
    if(!_topic_list.contains(topic))
    {
        qDebug()<<Q_FUNC_INFO<<topic;
        _topic_list.append(topic);
        subscribe(topic);
    }
}

void MqttClient::removeTopic(const QString topic)
{
    _topic_list.removeOne(topic);
    unsubscribe(topic);
}

void MqttClient::onConnected()
{
    foreach (QString topic, _topic_list)
    {
        subscribe(topic);
    }
    qDebug()<<Q_FUNC_INFO;
}

void MqttClient::onDisconnected()
{
    foreach (QString topic, _topic_list)
    {
        unsubscribe(topic);
    }
    qDebug()<<Q_FUNC_INFO;
}

void MqttClient::onSubscribed(const QString& topic, const quint8 qos)
{
    qDebug()<<Q_FUNC_INFO<<"topic"<<topic<<"qos"<<qos;
}
