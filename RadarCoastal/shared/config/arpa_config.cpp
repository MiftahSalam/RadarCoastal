#include <QDebug>
#include <QString>
#include <QFile>
#include <QSettings>

#include "arpa_config.h"

const QString ARPA_SPASI_MQTT = "arpa/spasi/mqtt";
const QString ARPA_INTERNAL_MQTT = "arpa/internal/mqtt";

ArpaConfig* ArpaConfig::config = nullptr;

ArpaConfig::ArpaConfig()
{

}

QString ArpaConfig::getMqttInternal() const
{
    return mqttInternal;
}

void ArpaConfig::setMqttInternal(const QString &newMqttInternal)
{
    mqttInternal = newMqttInternal;
}

QString ArpaConfig::getMqttSpasi() const
{
    return mqttSpasi;
}

ArpaConfig* ArpaConfig::getInstance(const QString path) {
    qDebug()<<Q_FUNC_INFO<<"path"<<path;

    if(config == nullptr) {
        if (!QFile::exists(path)) {
//            throw ErrFileNotFound();
            qFatal("file not found: %s", path.toUtf8().constData());
        }

        config = new ArpaConfig();
        config->setup(path);
    }

    return config;
}


void ArpaConfig::setup(const QString path)
{
    QSettings configFile(path,QSettings::IniFormat);

    mqttSpasi = configFile.value(ARPA_SPASI_MQTT, "mqtt;InOut;127.0.0.1:1883:user:pass:arpa:5").toString();
    mqttInternal = configFile.value(ARPA_INTERNAL_MQTT, "mqtt;InOut;127.0.0.1:1883:radar").toString();
}

ArpaConfig::~ArpaConfig()
{

}
