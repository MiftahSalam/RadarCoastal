#include <QDebug>
#include <QString>
#include <QFile>
#include <QSettings>

#include "echo_sender_config.h"

EchoSenderConfig* EchoSenderConfig::config = nullptr;

EchoSenderConfig::EchoSenderConfig()
    : BaseConfig()
{

}

QString EchoSenderConfig::getMqttSpasi() const
{
    return mqttSpasi;
}

EchoSenderConfig* EchoSenderConfig::getInstance(const QString path) {
    qDebug()<<Q_FUNC_INFO<<"path"<<path;

    if(config == nullptr) {
        if (!QFile::exists(path)) {
//            throw ErrFileNotFound();
            qFatal("file not found: %s", path.toUtf8().constData());
        }

        config = new EchoSenderConfig();
        config->setup(path);
    }

    return config;
}


void EchoSenderConfig::setup(const QString path)
{
    QSettings configFile(path,QSettings::IniFormat);

    mqttSpasi = configFile.value(ECHO_SPASI_NET, "mqtt;InOut;127.0.0.1:1883:user:pass:echo").toString();
}

void EchoSenderConfig::save(const QString path)
{

}

EchoSenderConfig::~EchoSenderConfig()
{

}
