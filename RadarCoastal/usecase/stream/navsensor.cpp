#include "navsensor.h"

#include <RadarEngine/constants.h>

#include <QJsonObject>
#include <QJsonDocument>

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, NavSensor)
#else
#include <QDebug>
#endif

NavSensor::NavSensor(QObject *parent) : QObject(parent)
{
#ifdef USE_LOG4QT
    logger()->trace() << Q_FUNC_INFO;
#else
    qDebug() << Q_FUNC_INFO;
#endif
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");

    initConfig();

    decoder = new NavDataDecoderNMEA();
    //    decoder = new NavDataDecoderCustom();

    m_no_osd_count = 11;

    connect(m_instance_cfg, &RadarEngine::RadarConfig::configValueChange,
            this, &NavSensor::triggerConfigChange);
}

void NavSensor::initConfig()
{
    QString nav_config_str = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_NET_CONFIG).toString();
    QStringList nav_config_str_list = nav_config_str.split(":");

    if (nav_config_str_list.size() != 3)
    {
#ifdef USE_LOG4QT
        logger()->fatal() << Q_FUNC_INFO << "invalid config mqtt" << nav_config_str;
#else
        qDebug() << Q_FUNC_INFO << "invalid config" << nav_config_str;
        exit(1);
#endif
    }

    bool gps_auto = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_GPS_AUTO).toBool();
    if (!gps_auto)
    {
        nav_config_str_list.removeLast();
        m_topic = "gps_man";
        nav_config_str_list.append(m_topic);
    }
    else
        m_topic = nav_config_str_list.last();

    m_no_osd_count = 11;

    m_stream = new Stream(this, nav_config_str_list.join(":"));

    m_stream->ChangeConfig("subsciber:topic-add:gps_man");
    m_stream->ChangeConfig("subsciber:topic-add:gps");

    connect(m_stream, &Stream::SignalReceiveData, this, &NavSensor::triggerReceivedData);
    connect(m_instance_cfg, &RadarEngine::RadarConfig::configValueChange,
            this, &NavSensor::triggerConfigChange);
}

void NavSensor::triggerConfigChange(const QString key, const QVariant val)
{
#ifdef USE_LOG4QT
    logger()->trace() << Q_FUNC_INFO << "key" << key << "val" << val.toString();
#endif
    if (key == RadarEngine::NON_VOLATILE_NAV_NET_CONFIG)
    {
        m_stream->SetConfig(val.toString());
    }
#ifndef DISPLAY_ONLY_MODE
    else if (key == RadarEngine::NON_VOLATILE_NAV_CONTROL_GPS_AUTO)
    {
        if (val.toBool())
        {
            QString nav_config_str = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_NET_CONFIG).toString();
            QStringList nav_config_str_list = nav_config_str.split(":");

            if (nav_config_str_list.size() != 3)
            {
#ifdef USE_LOG4QT
                logger()->warn() << Q_FUNC_INFO << "invalid config" << nav_config_str;
#else
                qDebug() << Q_FUNC_INFO << "invalid config" << nav_config_str;
#endif
                return;
            }

            m_topic = nav_config_str_list.last();
            m_stream->SetConfig(nav_config_str);
        }
        else
        {
            QString nav_config_str = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_NET_CONFIG).toString();
            QStringList nav_config_str_list = nav_config_str.split(":");

            if (nav_config_str_list.size() != 3)
            {
#ifdef USE_LOG4QT
                logger()->warn() << Q_FUNC_INFO << "invalid config" << nav_config_str;
#else
                qDebug() << Q_FUNC_INFO << "invalid config" << nav_config_str;
#endif
                return;
            }

            nav_config_str_list.removeLast();
            m_topic = "gps_man";
            nav_config_str_list.append(m_topic);

            m_stream->SetConfig(nav_config_str_list.join(":"));
        }
    }
#endif
}

void NavSensor::sendMqtt(NavDataEncoder *encoder)
{
    QString mq_data = m_topic + MQTT_MESSAGE_SEPARATOR + encoder->encode();

    if (m_stream->GetStreamStatus() == DeviceWrapper::NOT_AVAIL)
        m_stream->Reconnect();
    else
        m_stream->SendData(mq_data);
}

void NavSensor::SendData(QString lat, QString lon, QString hdt)
{
    //    QString m_data = m_topic + ":" + "?" + lat + "#" + lon + "#" + hdt + "!";
    //    m_stream->SendData(m_data);

    NavDataEncoder *encoder = dynamic_cast<NavDataEncoder *>(new NavDataEncoderCustom(
                                                                 QDateTime::currentMSecsSinceEpoch(),
                                                                 lat.toDouble(),
                                                                 lon.toDouble(),
                                                                 hdt.toDouble()
                                                                 )
                                                             );
    sendMqtt(encoder);

    delete encoder;

}

void NavSensor::UpdateStatus()
{
    m_no_osd_count++;
    if (m_no_osd_count > 110)
        m_no_osd_count = 11;
    if (m_no_osd_count < 11)
        return;

    switch (m_stream->GetStreamStatus())
    {
    case DeviceWrapper::NOT_AVAIL:
        m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING, 0); // offline
        m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS, 0);     // offline
        break;
    case DeviceWrapper::NO_INPUT_DATA:
        m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING, 1); // no data
        m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS, 1);     // no data
        break;
    default:
        break;
    }
}

void NavSensor::triggerReceivedData(const QString data)
{
#ifdef USE_LOG4QT
    logger()->debug() << Q_FUNC_INFO << "data: " << data;
#else
    qDebug() << Q_FUNC_INFO << data;
#endif


#ifdef DISPLAY_ONLY_MODE
    if (msg.contains("_man"))
    {
        m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_GPS_AUTO, false);
        m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_HEADING_AUTO, false);
    }
    else
    {
        m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_GPS_AUTO, true);
        m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_HEADING_AUTO, true);
    }
#else
    if (data.contains("_man"))
    {
        return;
    }
#endif

    decoder->update(data.toUtf8());

    NavDataModel model = decoder->decode();
    if (model.timestamp < 0)
    {
        return;
    }

    m_no_osd_count = 0;

    if (model.status_gps == 3)
    {
        m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE, model.lat);
        m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE, model.lon);
        m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS, model.status_gps);
    }
    else if (model.status_gps == 2) m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS, 2); //data not valid

    if (model.status_hdg == 3)
    {
        m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING, model.hdg);
        m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING, model.status_hdg); //data valid
    }
    else if (model.status_hdg == 2) m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING, 2); //data not valid


}
void NavSensor::Reconnect()
{
    m_stream->Reconnect();
}
