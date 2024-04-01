#include "baseresponsejson.h"
#include "sitedatasender.h"

#include <QJsonObject>
#include <QJsonDocument>

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, SiteDataSender)
#else
#include <QDebug>
#endif

SiteDataSender::SiteDataSender(QObject *parent)
    : QObject(parent), m_stream_mqtt_spasi{nullptr}, m_stream_mqtt{nullptr}
{
#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO;
#else
    qDebug()<<Q_FUNC_INFO;
#endif
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");

    initConfigMqtt();
    initConfigMqttSpasi();

    m_site_data_count = 0;

    connect(m_instance_cfg,&RadarEngine::RadarConfig::configValueChange,
            this,&SiteDataSender::triggerConfigChange);
}

void SiteDataSender::initConfigMqttSpasi()
{
    QString config_str = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_NET_CONFIG_SPASI).toString();
    QStringList config_str_list = config_str.split(":");

    if(config_str_list.size() != 6)
    {
#ifdef USE_LOG4QT
        logger()->fatal()<<Q_FUNC_INFO<<"invalid config mqtt spasi"<<config_str;
#else
        qDebug()<<Q_FUNC_INFO<<"invalid config mqtt spasi"<<config_str;
        exit(1);
#endif
    }

    bool ok;
    max_site_data_count = config_str_list.last().toInt(&ok);
    if (!ok) {
        max_site_data_count = 10;
        qWarning()<<Q_FUNC_INFO<<"invalid max_site_data_count"<<config_str_list.last()<<". will use default 10";
    }

    m_topic_spasi = config_str_list.at(4);
    if (!m_stream_mqtt_spasi) {
        m_stream_mqtt_spasi = new Stream(this,config_str);
    }
}

void SiteDataSender::initConfigMqtt()
{
    QString config_str = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_NAV_NET_CONFIG).toString();
    QStringList config_str_list = config_str.split(":");

    if(config_str_list.size() != 3)
    {
#ifdef USE_LOG4QT
        logger()->fatal()<<Q_FUNC_INFO<<"invalid config mqtt"<<config_str;
#else
        qDebug()<<Q_FUNC_INFO<<"invalid config mqtt"<<config_str;
        exit(1);
#endif
    }

    m_topic = "site_data";
    config_str_list.removeLast();
    config_str_list.append(m_topic);
    if (!m_stream_mqtt) {
        m_stream_mqtt = new Stream(this,config_str_list.join(":"));
    }
    m_stream_mqtt->ChangeConfig("subsciber:topic-add:"+m_topic);
}

void SiteDataSender::triggerConfigChange(const QString key, const QVariant val)
{
#ifdef USE_LOG4QT
        logger()->trace()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val.toString();
#endif
    if(key == RadarEngine::NON_VOLATILE_NAV_NET_CONFIG_SPASI)
    {
        m_stream_mqtt_spasi->SetConfig(val.toString());
    }
}

void SiteDataSender::SendSiteData()
{
    const bool gps_auto = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_GPS_AUTO).toBool();
    const bool hdt_auto = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_HEADING_AUTO).toBool();

    m_site_data_count++;
    if(m_site_data_count >= max_site_data_count)
    {
        m_site_data_count = 0;

        QJsonObject obj;
        QJsonObject objPos;
        QJsonObject objHdt;
        QJsonObject objRadar;

        objRadar["radar_min_range"] = distanceList.last();
        objRadar["radar_max_range"] = distanceList.first();
        objRadar["status"] = radarStatusCode(static_cast<RadarEngine::RadarState>(m_instance_cfg->getConfig(RadarEngine::VOLATILE_RADAR_STATUS).toInt()));

        objPos["mode"] = gps_auto ? "auto" : "manual";
        objPos["status"] = navStatusCode(m_instance_cfg->getConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS).toInt());
        objPos["latitude"] = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
        objPos["longitude"] = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();

        objHdt["mode"] = hdt_auto ? "auto" : "manual";
        objHdt["status"] = navStatusCode(m_instance_cfg->getConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING).toInt());
        objHdt["heading"] = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();

        obj["position"] = objPos;
        obj["heading"] = objHdt;
        obj["radar"] = objRadar;

        BaseResponseJson<QJsonObject> resp(0, "ok", &obj);

        QJsonDocument docWs(resp.build());
        QJsonDocument docMqtt(obj);
        auto dataSendWs = docWs.toJson(QJsonDocument::Compact);
        auto dataSendMqtt = docMqtt.toJson(QJsonDocument::Compact);

        sendMqtt(dataSendMqtt);
        sendMqttSpasi(dataSendWs);
    }
}

void SiteDataSender::sendMqttSpasi(QString data)
{
    QString mq_data = m_topic_spasi+MQTT_MESSAGE_SEPARATOR+data;

    if(m_stream_mqtt_spasi->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream_mqtt_spasi->Reconnect();
    m_stream_mqtt_spasi->SendData(mq_data);
}

void SiteDataSender::sendMqtt(QString data)
{
    QString mq_data = m_topic+MQTT_MESSAGE_SEPARATOR+data;

    if(m_stream_mqtt->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream_mqtt->Reconnect();
    else m_stream_mqtt->SendData(mq_data);
}

void SiteDataSender::Reconnect()
{
    if(m_stream_mqtt->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream_mqtt->Reconnect();
    if(m_stream_mqtt_spasi->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream_mqtt_spasi->Reconnect();
}

int SiteDataSender::navStatusCode(const int status) const
{
    switch(status)
    {
    case 0:
        return 3;
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 0;
    }
}

int SiteDataSender::radarStatusCode(const RadarEngine::RadarState status) const
{
    switch(status)
    {
    case RadarEngine::RADAR_OFF:
        return 3;
    case RadarEngine::RADAR_STANDBY:
        return 1;
    case RadarEngine::RADAR_NO_SPOKE:
        return 2;
    case RadarEngine::RADAR_TRANSMIT:
        return 0;
    }
}
