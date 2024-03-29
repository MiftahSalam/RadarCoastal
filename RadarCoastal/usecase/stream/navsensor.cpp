#include "navsensor.h"

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

    m_append_data_osd = "";
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
    //    qDebug()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val;
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

void NavSensor::SendData(QString lat, QString lon, QString hdt)
{
    QString m_data = m_topic + ":" + "?" + lat + "#" + lon + "#" + hdt + "!";
    m_stream->SendData(m_data);
}

bool NavSensor::isGPSDataValid(const QString lat_str, const QString lon_str)
{
    bool ok;
    double lat = lat_str.toDouble(&ok);
    if (!ok)
        return false;
    double lon = lon_str.toDouble(&ok);
    if (!ok)
        return false;

    if (fabs(lat) > 90.)
        return false;
    if (fabs(lon) > 180.)
        return false;

    return true;
}

bool NavSensor::isHDGDataValid(const QString hdg_str)
{
    bool ok;
    double hdg = hdg_str.toDouble(&ok);
    if (!ok)
        return false;

    if (hdg >= 0. && hdg <= 360.)
        return true;
    else
        return false;
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
    logger()->debug() << Q_FUNC_INFO << "data" << data;
#else
    qDebug() << Q_FUNC_INFO << data;
#endif
    QString msg = data;
    if (msg.contains("gps:"))
    {
        m_no_osd_count = 0;

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
        if (msg.contains("_man"))
        {
            return;
        }
#endif
        msg.remove("gps:");
        msg.remove("_man");

#ifdef USE_LOG4QT
        logger()->debug() << Q_FUNC_INFO << "osd" << msg;
#else
        qDebug() << Q_FUNC_INFO << "osd" << msg;
#endif

        m_append_data_osd.append(msg);

        int index_hdr = m_append_data_osd.indexOf("?");
        if (index_hdr >= 0)
        {
            int index_end = m_append_data_osd.indexOf("!");
            if (index_end >= 0)
            {
                if (index_end > index_hdr)
                {
                    //?-6.939176#107.632770#31!
                    m_append_data_osd = m_append_data_osd.mid(index_hdr, index_end - index_hdr);
#ifdef USE_LOG4QT
                    logger()->debug() << Q_FUNC_INFO << "filter" << m_append_data_osd.remove("!").remove("?").remove("\r").remove("\n");
#else
                    qDebug() << Q_FUNC_INFO << "filter" << m_append_data_osd.remove("!").remove("?").remove("\r").remove("\n");
#endif
#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
                    QStringList msg_list = m_append_data_osd.split("#", Qt::SkipEmptyParts);
#else
                    QStringList msg_list = m_append_data_osd.split("#", QString::SkipEmptyParts);
                    QStringList msg_list = m_append_data_osd.split("#", QString::SkipEmptyParts);
#endif
                    if (msg_list.size() == 3)
                    {
                        if (isGPSDataValid(msg_list.at(0), msg_list.at(1)))
                        {
                            m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE, msg_list.at(0).toDouble());
                            m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE, msg_list.at(1).toDouble());
                            m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS, 3); // data valid
                        }
                        else
                        {
#ifdef USE_LOG4QT
                        logger()->warn() << Q_FUNC_INFO << "gps invalid";
#else
                            qDebug() << Q_FUNC_INFO << "gps invalid";
#endif
                            m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS, 2); // data not valid
                        }

                        if (isHDGDataValid(msg_list.at(2)))
                        {
                            m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING, msg_list.at(2).toDouble());
                            m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING, 3); // data valid
                        }
                        else
                        {
#ifdef USE_LOG4QT
                        logger()->warn() << Q_FUNC_INFO << "hdt invalid";
#else
                            qDebug() << Q_FUNC_INFO << "hdt invalid";
#endif
                            m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING, 2); // data not valid
                        }
                    }
                    else
                    {
#ifdef USE_LOG4QT
                        logger()->warn() << Q_FUNC_INFO << "osd invalid";
#else
                        qDebug() << Q_FUNC_INFO << "osd invalid";
#endif
                        m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING, 2); // data not valid
                        m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS, 2);     // data not valid
                    }

                    m_append_data_osd.clear();
                }
                else
                {
                    m_append_data_osd.remove(0, index_hdr);
                }
            }
#ifdef USE_LOG4QT
            logger()->trace() << Q_FUNC_INFO << "index_end" << index_end;
#else
            qDebug() << Q_FUNC_INFO << index_end;
#endif
        }
#ifdef USE_LOG4QT
        logger()->trace() << Q_FUNC_INFO << "index_hdr" << index_hdr;
#else
        qDebug() << Q_FUNC_INFO << index_hdr;
#endif
    }
}

void NavSensor::Reconnect()
{
    m_stream->Reconnect();
}
