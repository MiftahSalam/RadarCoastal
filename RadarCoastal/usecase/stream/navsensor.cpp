#include "navsensor.h"
#include "usecase/stream/nav_data_model.h"
#include "shared/config/applicationconfig.h"

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
    navConfig = ApplicationConfig::getInstance()->getNavConfig();

    initConfigMqtt();

    decoder = new NavDataDecoderNMEA();
//    decoder = new NavDataDecoderCustom();

    m_no_osd_count = 11;

    navConfig->attach(this);
}

void NavSensor::initConfigMqtt()
{
    QString nav_config_str = navConfig->getMqttInternal();
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

void NavSensor::configChange(const QString key, const QVariant val)
{
    //    qDebug()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val;
#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val.toString();
#endif
    if(key == NAV_INTERNAL_NET)
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
        navConfig->setGpsStatus(0); //offline
        navConfig->setHeadingStatus(0); //offline
        break;
    case DeviceWrapper::NO_INPUT_DATA:
        navConfig->setGpsStatus(1); //no data
        navConfig->setHeadingStatus(1); //no data
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

    const bool gps_auto = navConfig->getGPSModeAuto();
    const bool hdg_auto = navConfig->getHeadingModeAuto();

    if (gps_auto)
    {
        if (model.status_gps == 3)
        {
            m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE, model.lat);
            m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE, model.lon);
            navConfig->setGpsStatus(model.status_gps);
        }
        else if (model.status_gps == 2) navConfig->setGpsStatus(2); //data not valid
    }

    if (hdg_auto)
    {
        if (model.status_hdg == 3)
        {
            m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING, model.hdg);
            navConfig->setHeadingStatus(model.status_hdg);  //data valid
        }
        else if (model.status_hdg == 2) navConfig->setHeadingStatus(2); //data not valid
    }
}

void NavSensor::Reconnect()
{
    m_stream_mqtt->Reconnect();
}
