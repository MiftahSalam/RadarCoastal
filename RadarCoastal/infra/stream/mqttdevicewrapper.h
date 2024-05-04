#ifndef MQTTDEVICEWRAPPER_H
#define MQTTDEVICEWRAPPER_H

#include <QObject>
#include <QTimer>

#include "qmqtt.h"
#include "devicewrapper.h"

const QString MQTT_MESSAGE_SEPARATOR = "@";

class MqttClient : public QMQTT::Client
{
    Q_OBJECT
public:
    explicit MqttClient(QObject *parent = nullptr,
                       const QHostAddress& host = QHostAddress::LocalHost,
                       const quint16 port = 1883, QString topic = "");
    explicit MqttClient(QObject *parent = nullptr,
                       const QString& hostname = "",
                       const quint16 port = 1883, QString topic = "");

    void AddTopic(const QString topic);
    void RemoveTopic(const QString topic);

protected slots:
    void onConnected();
    void onDisconnected();
    void onSubscribed(const QString& topic, const quint8 qos);

private:
    QHostAddress m_host;
    quint16 m_port;
    QStringList m_topic_list;
};

class Publisher : public MqttClient
{
    Q_OBJECT
public:
    explicit Publisher(QObject *parent = nullptr,
                       const QHostAddress& host = QHostAddress::LocalHost,
                       const quint16 port = 1883, QString topic = "");
    explicit Publisher(QObject *parent = nullptr,
                       const QString& hostname = "",
                       const quint16 port = 1883, QString topic = "");

    void PublishData(QMQTT::Message message);
};

class Subscriber : public MqttClient
{
    Q_OBJECT
public:
    explicit Subscriber(QObject *parent = nullptr,
                       const QHostAddress& host = QHostAddress::LocalHost,
                       const quint16 port = 1883, QString topic = "");
    explicit Subscriber(QObject *parent = nullptr,
                       const QString& hostname = "",
                       const quint16 port = 1883, QString topic = "");

signals:
    void SignalOnReceived(QMQTT::Message message);
};

class MqttDeviceWrapper : public DeviceWrapper
{
    Q_OBJECT
public:
    MqttDeviceWrapper(MqttDeviceWrapper& other) = delete;
    void operator=(const MqttDeviceWrapper&) = delete;
    ~MqttDeviceWrapper() override;

    static MqttDeviceWrapper* GetInstance(const QString config);

    void Write(const QString data) override;
    DeviceStatus GetStatus() override;
    void Reconnect() override;
    void ChangeConfig(const QString command) override;
    bool InitConfig(const QString config = "127.0.0.1:1883") override;

protected:
    MqttDeviceWrapper(QObject *parent = nullptr);

private slots:
    void receiveData(QMQTT::Message message);

private:
    struct MqttConfig {
        QHostAddress host;
        quint16 port;
        QString user;
        QString password;
    } m_mqttConfig;
    quint16 m_idCounter;
    QString m_defaultTopic;

    Publisher* m_publisher;
    Subscriber* m_subsciber;
    static QMap<QString, MqttDeviceWrapper*> m_wrappers;

    static QString extractBrokerUrl(QString config);
};


#endif // MQTTDEVICEWRAPPER_H
