#ifndef MQTTDEVICEWRAPPER_H
#define MQTTDEVICEWRAPPER_H

#include <QObject>
#include <QTimer>

#include "qmqtt.h"
#include "devicewrapper.h"

class MqttClient : public QMQTT::Client
{
    Q_OBJECT
public:
    explicit MqttClient(QObject *parent = nullptr,
                       const QHostAddress& host = QHostAddress::LocalHost,
                       const quint16 port = 1883, QString topic = "");

    void addTopic(const QString topic);
    void removeTopic(const QString topic);

protected slots:
    void onConnected();
    void onDisconnected();
    void onSubscribed(const QString& topic, const quint8 qos);

private:
    QHostAddress _host;
    quint16 _port;
    QStringList _topic_list;
};

class Publisher : public MqttClient
{
    Q_OBJECT
public:
    explicit Publisher(QObject *parent = nullptr,
                       const QHostAddress& host = QHostAddress::LocalHost,
                       const quint16 port = 1883, QString topic = "");

    void publishData(QMQTT::Message message);
};

class Subscriber : public MqttClient
{
    Q_OBJECT
public:
    explicit Subscriber(QObject *parent = nullptr,
                       const QHostAddress& host = QHostAddress::LocalHost,
                       const quint16 port = 1883, QString topic = "");

signals:
    void signal_onReceived(QMQTT::Message message);
};

class MqttDeviceWrapper : public DeviceWrapper
{
    Q_OBJECT
public:
    MqttDeviceWrapper(MqttDeviceWrapper& other) = delete;
    void operator=(const MqttDeviceWrapper&) = delete;
    ~MqttDeviceWrapper() override;

    static MqttDeviceWrapper* getInstance(const QString config);

    void write(const QString data) override;
    DeviceStatus getStatus() override;
    void reconnect() override;
    void changeConfig(const QString command) override;
    bool initConfig(const QString config = "127.0.0.1:1883") override;

protected:
    MqttDeviceWrapper(QObject *parent = nullptr);

private slots:
    void receiveData(QMQTT::Message message);

private:
    struct MqttConfig {
        QHostAddress host;
        quint16 port;
    } _mqttConfig;
    quint16 _idCounter;
    QString _defaultTopic;

    Publisher* _publisher;
    Subscriber* _subsciber;
    static QMap<QString, MqttDeviceWrapper*> _wrappers;

};


#endif // MQTTDEVICEWRAPPER_H
