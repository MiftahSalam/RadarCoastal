#include "navsensor.h"
#include "usecase/stream/nav_data_model.h"

#include <RadarEngine/constants.h>

#include <QJsonObject>
#include <QJsonDocument>

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, NavSensor)
#else
#include <QDebug>
#endif

NavSensor::NavSensor(QObject *parent) : QObject(parent), m_stream_mqtt{nullptr}
{
#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO;
#else
    qDebug()<<Q_FUNC_INFO;
#endif
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");

    initConfigMqtt();

    decoder = new NavDataDecoderNMEA();
//    decoder = new NavDataDecoderCustom();

    m_no_osd_count = 11;

    connect(m_instance_cfg,&RadarEngine::RadarConfig::configValueChange,
            this,&NavSensor::triggerConfigChange);
}

void NavSensor::initConfigMqtt()
{
    QString nav_config_str = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_NET_CONFIG).toString();
    QStringList nav_config_str_list = nav_config_str.split(":");

    if(nav_config_str_list.size() != 3)
    {
#ifdef USE_LOG4QT
        logger()->fatal()<<Q_FUNC_INFO<<"invalid config mqtt"<<nav_config_str;
#else
        qDebug()<<Q_FUNC_INFO<<"invalid config mqtt"<<nav_config_str;
        exit(1);
#endif
    }

    /*
    bool gps_auto = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_GPS_AUTO).toBool();
    if(!gps_auto)
    {
        nav_config_str_list.removeLast();
        m_topic = "gps_man";
        nav_config_str_list.append(m_topic);
    }
    else
        m_topic = nav_config_str_list.last();
    */

    m_no_osd_count = 11;

    if (!m_stream_mqtt) {
        m_stream_mqtt = new Stream(this,nav_config_str_list.join(":"));

        connect(m_stream_mqtt, &Stream::SignalReceiveData, this, &NavSensor::triggerReceivedData);
    }
    else m_stream_mqtt->ChangeConfig("subsciber:topic-rm:"+m_topic);

    m_topic = nav_config_str_list.last();
    m_stream_mqtt->ChangeConfig("subsciber:topic-add:"+m_topic);

}

void NavSensor::triggerConfigChange(const QString key, const QVariant val)
{
    //    qDebug()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val;
#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val.toString();
#endif
    if(key == RadarEngine::NON_VOLATILE_NAV_NET_CONFIG)
    {
        initConfigMqtt();
        m_stream_mqtt->SetConfig(val.toString());
    }
}

void NavSensor::UpdateStatus()
{
    m_no_osd_count++;
    if(m_no_osd_count > 110) m_no_osd_count = 11;
    if(m_no_osd_count < 11) return;

    switch (m_stream_mqtt->GetStreamStatus()) {
    case DeviceWrapper::NOT_AVAIL:
        m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING, 0); //offline
        m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS, 0); //offline
        break;
    case DeviceWrapper::NO_INPUT_DATA:
        m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING, 1); //no data
        m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS, 1); //no data
        break;
    default:
        break;
    }
}

void NavSensor::triggerReceivedData(QString data)
{
    decoder->update(data.toUtf8());

    NavDataModel model = decoder->decode();
    if(model.timestamp < 0)
    {
        return;
    }

    m_no_osd_count = 0;
    m_stream_mqtt->UpdateTimeStamp();

    const bool gps_auto = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_GPS_AUTO).toBool();
    const bool hdg_auto = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_HEADING_AUTO).toBool();

    if (gps_auto)
    {
        if (model.status_gps == 3)
        {
            m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE, model.lat);
            m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE, model.lon);
            m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS, model.status_gps);
        }
        else if (model.status_gps == 2) m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS, 2); //data not valid
    }

    if (hdg_auto)
    {
        if (model.status_hdg == 3)
        {
            m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING, model.hdg);
            m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING, model.status_hdg); //data valid
        }
        else if (model.status_hdg == 2) m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING, 2); //data not valid
    }
}

void NavSensor::Reconnect()
{
    m_stream_mqtt->Reconnect();
}
