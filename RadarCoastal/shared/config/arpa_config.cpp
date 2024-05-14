#include <QDebug>
#include <QString>
#include <QFile>
#include <QSettings>

#include "arpa_config.h"

ArpaConfig* ArpaConfig::config = nullptr;

ArpaConfig::ArpaConfig()
    : BaseConfig()
{

}

QString ArpaConfig::getMqttInternal() const
{
    return mqttInternal;
}

void ArpaConfig::setMqttInternal(const QString &newMqttInternal)
{
    mqttInternal = newMqttInternal;
    changeConfig(ARPA_NET_INTERNAL, newMqttInternal);
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

    mqttSpasi = configFile.value(ARPA_NET_SPASI, "mqtt;InOut;127.0.0.1:1883:user:pass:arpa:5").toString();
    mqttInternal = configFile.value(ARPA_NET_INTERNAL, "mqtt;InOut;127.0.0.1:1883:radar").toString();
    createArpaByClick = configFile.value(ARPA_CREATE_ARPA_BY_CLICK, true).toBool();
}

void ArpaConfig::save(const QString path)
{
    qDebug() << Q_FUNC_INFO << path;

    QSettings config(path, QSettings::IniFormat);

    config.setValue(ARPA_NET_INTERNAL, mqttInternal);
    config.setValue(ARPA_CREATE_ARPA_BY_CLICK, createArpaByClick);
}

ArpaConfig::~ArpaConfig()
{

}
