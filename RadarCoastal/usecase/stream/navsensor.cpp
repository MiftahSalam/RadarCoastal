#include "navsensor.h"
#include "shared/config/applicationconfig.h"

#include <RadarEngine/constants.h>

#include <QJsonObject>
#include <QJsonDocument>

#ifndef DISPLAY_ONLY_MODE
#include "QtConcurrent/qtconcurrentrun.h"
#endif

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, NavSensor)
#else
#include <QDebug>
#endif

//const QString SITE_DATA_TOPIC = "site_data";

NavSensor::NavSensor(QObject *parent)
    : QObject(parent),
      #ifndef DISPLAY_ONLY_MODE
      m_stream_mqtt_private{nullptr},
      #endif
      m_stream_mqtt_public{nullptr}
{
#ifdef USE_LOG4QT
    logger()->trace() << Q_FUNC_INFO;
#else
    qDebug() << Q_FUNC_INFO;
#endif
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");
    navConfig = ApplicationConfig::getInstance()->getNavConfig();

#ifndef DISPLAY_ONLY_MODE
    initConfigMqttPrivate();
#endif
    initConfigMqttPublic();

#ifdef DISPLAY_ONLY_MODE
    decoder = new NavDataDecoderJson();
#else
    decoder = new NavDataDecoderNMEA();

    connect(&watcherCapture, &QFutureWatcher<NavDataModel>::finished, this, &NavSensor::triggerParseData);
#endif
    //    decoder = new NavDataDecoderCustom();

    m_no_osd_count = 11;
}

void NavSensor::initConfigMqttPublic()
{
    QString config_str = navConfig->getMqttPublic();
    QStringList config_str_list = config_str.split(":");

    if(config_str_list.size() != 6)
    {
#ifdef USE_LOG4QT
        logger()->fatal()<<Q_FUNC_INFO<< "invalid config mqtt public: "<<config_str;
#else
        qDebug()<<Q_FUNC_INFO<<"invalid config mqtt public"<<config_str;
        exit(1);
#endif
    }

    m_topic_public = config_str_list.at(4);
    if (!m_stream_mqtt_public) {
        m_stream_mqtt_public = new Stream(this,config_str);
#ifdef DISPLAY_ONLY_MODE
        connect(m_stream_mqtt_public, &Stream::SignalReceiveData, this, &NavSensor::triggerReceivedData);
#endif
    }
}

#ifndef DISPLAY_ONLY_MODE
void NavSensor::initConfigMqttPrivate()
{
    QString nav_config_str = navConfig->getMqttInternal();
    QStringList nav_config_str_list = nav_config_str.split(":");

    if (nav_config_str_list.size() != 3)
    {
#ifdef USE_LOG4QT
        logger()->fatal() << Q_FUNC_INFO << " invalid config mqtt private: " << nav_config_str;
#else
        qDebug() << Q_FUNC_INFO << "invalid config private:" << nav_config_str;
        exit(1);
#endif
    }

    m_topic_private = nav_config_str_list.last();

    /*
    bool gps_auto = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_GPS_AUTO).toBool();
    if (!gps_auto)
    {
        nav_config_str_list.removeLast();
        m_topic = "gps_man";
        nav_config_str_list.append(m_topic);
    }
    else
        m_topic = nav_config_str_list.last();
    */

    m_no_osd_count = 11;

    if (!m_stream_mqtt_private) {
        m_stream_mqtt_private = new Stream(this,nav_config_str);
        connect(m_stream_mqtt_private, &Stream::SignalReceiveData, this, &NavSensor::triggerReceivedData);
        navConfig->attach(this);
    }
}

void NavSensor::configChange(const QString key, const QVariant val)
{
    if(key == NAV_NET_INTERNAL)
    {
        initConfigMqttPrivate();
        m_stream_mqtt_private->SetConfig(val.toString());
    }
}
#endif

#ifndef DISPLAY_ONLY_MODE
void NavSensor::sendMqtt(NavDataEncoder *encoder)
{
    QString mq_data = m_topic_public + MQTT_MESSAGE_SEPARATOR + encoder->encode();

#ifdef USE_LOG4QT
    logger()->debug()<<Q_FUNC_INFO<<" mq_data: "<<mq_data;
#else
    qDebug()<<Q_FUNC_INFO<<" mq_data: "<<mq_data;
#endif

    if (m_stream_mqtt_public->GetStreamStatus() == DeviceWrapper::NOT_AVAIL)
        m_stream_mqtt_public->Reconnect();
    else
        m_stream_mqtt_public->SendData(mq_data);

    if (m_stream_mqtt_private->GetStreamStatus() == DeviceWrapper::NOT_AVAIL)
        m_stream_mqtt_private->Reconnect();
    else
        m_stream_mqtt_private->SendData(mq_data);
}

void NavSensor::SendData()
{
    //    QString m_data = m_topic + ":" + "?" + lat + "#" + lon + "#" + hdt + "!";
    //    m_stream->SendData(m_data);

    double lat = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
    double lon = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();
    double hdt = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();
    double gps_man = !navConfig->getGPSModeAuto();
    double hdt_man = !navConfig->getHeadingModeAuto();
    quint8 gps_status = navConfig->getGPSStatus();
    quint8 hdt_status = navConfig->getHeadingStatus();

    /*
    NavDataEncoder *encoder = dynamic_cast<NavDataEncoder *>(new NavDataEncoderCustom(
                                                                 QDateTime::currentMSecsSinceEpoch(),
                                                                 lat,
                                                                 lon,
                                                                 hdt,
                                                                 gps_man,
                                                                 hdt_man
                                                                 )
                                                             );
    */
    NavDataEncoder *encoder = dynamic_cast<NavDataEncoder *>(new NavDataEncoderJson(
                                                                 QDateTime::currentMSecsSinceEpoch(),
                                                                 lat,
                                                                 lon,
                                                                 hdt,
                                                                 gps_man,
                                                                 hdt_man,
                                                                 gps_status,
                                                                 hdt_status
                                                                 )
                                                             );
    sendMqtt(encoder);

    delete encoder;

}
#endif

void NavSensor::UpdateStatus()
{
    m_no_osd_count++;
    if (m_no_osd_count > 110)
        m_no_osd_count = 11;
    if (m_no_osd_count < 11)
        return;

#ifndef DISPLAY_ONLY_MODE
    switch (m_stream_mqtt_private->GetStreamStatus())
#else
    switch (m_stream_mqtt_public->GetStreamStatus())
#endif
    {
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

void NavSensor::processNavData(QString data)
{
    decoder->update(data.toUtf8());

#ifndef DISPLAY_ONLY_MODE
    QFuture<NavDataModel> future = QtConcurrent::run(decoder, &NavDataDecoder::decode);
    watcherCapture.setFuture(future);
#else
    NavDataModel model = decoder->decode();
    if (model.timestamp < 0)
    {
        return;
    }

    m_no_osd_count = 0;

    m_stream_mqtt_public->UpdateTimeStamp();
    if (model.status_gps == 3)
    {
        {
            m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE, model.lat);
            m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE, model.lon);
        }
        navConfig->setGpsStatus(model.status_gps);
    }
    else if (model.status_gps == 2) navConfig->setGpsStatus(2); //data not valid

    if (model.status_hdg == 3)
    {
        {
            m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING, model.hdg);
        }
        navConfig->setHeadingStatus(model.status_hdg);  //data valid
    }
    else if (model.status_hdg == 2) navConfig->setHeadingStatus(2); //data not valid

    navConfig->setGpsModeAuto(!model.gps_man);
    navConfig->setHeadingModeAuto(!model.hdg_man);
#endif
}

#ifndef DISPLAY_ONLY_MODE
void NavSensor::triggerParseData()
{
    QFutureWatcher<NavDataModel> *watcher;
    watcher = reinterpret_cast<QFutureWatcher<NavDataModel> *>(sender());
    NavDataModel model = watcher->result();
    if (model.timestamp < 0)
    {
        return;
    }

    m_no_osd_count = 0;

    m_stream_mqtt_private->UpdateTimeStamp();
    if (model.status_gps == 3)
    {
        const bool gps_auto = navConfig->getGPSModeAuto();
        if (gps_auto)
        {
            m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE, model.lat);
            m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE, model.lon);
        }
        navConfig->setGpsStatus(model.status_gps);
    }
    else if (model.status_gps == 2) navConfig->setGpsStatus(2); //data not valid

    if (model.status_hdg == 3)
    {
        const bool hdg_auto = navConfig->getHeadingModeAuto();
        if (hdg_auto)
        {
            m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING, model.hdg);
        }
        navConfig->setHeadingStatus(model.status_hdg);  //data valid
    }
    else if (model.status_hdg == 2) navConfig->setHeadingStatus(2); //data not valid
}
#endif

void NavSensor::triggerReceivedData(QString data)
{
#ifdef USE_LOG4QT
    logger()->debug() << Q_FUNC_INFO << "data: " << data;
#else
    qDebug() << Q_FUNC_INFO << data;
#endif

#ifdef DISPLAY_ONLY_MODE
    if (!data.contains(m_topic_public))
    {
        return;
    }

    data.remove(m_topic_public+MQTT_MESSAGE_SEPARATOR);
#else
    if (!data.contains(m_topic_private))
    {
        return;
    }

    data.remove(m_topic_private+MQTT_MESSAGE_SEPARATOR);
#endif

    processNavData(data);
}

void NavSensor::Reconnect()
{
#ifndef DISPLAY_ONLY_MODE
    m_stream_mqtt_private->Reconnect();
#endif
    m_stream_mqtt_public->Reconnect();
}
