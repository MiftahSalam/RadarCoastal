#include "navsensor.h"

#include <RadarEngine/constants.h>

#include <QJsonObject>
#include <QJsonDocument>

NavSensor::NavSensor(QObject *parent) : QObject(parent)
{
    qDebug()<<Q_FUNC_INFO;
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");

    initConfig();
}

void NavSensor::initConfig()
{
    QString nav_config_str = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_NET_CONFIG).toString();
    QString config_ws_str = "ws;Out;127.0.0.1:8884"; //todo use config
    QStringList nav_config_str_list = nav_config_str.split(":");
    QStringList config_ws_str_list = config_ws_str.split(";");

    if(nav_config_str_list.size() != 3)
    {
        qDebug()<<Q_FUNC_INFO<<"invalid config"<<nav_config_str;
        exit(1);
    }
    if(config_ws_str_list.size() != 3)
    {
        qDebug()<<Q_FUNC_INFO<<"invalid config ws"<<config_ws_str;
        exit(1);
    }

    bool gps_auto = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_GPS_AUTO).toBool();
    if(!gps_auto)
    {
        nav_config_str_list.removeLast();
        m_topic = "gps_man";
        nav_config_str_list.append(m_topic);
    }
    else
        m_topic = nav_config_str_list.last();

    m_append_data_osd = "";
    m_no_osd_count = 11;
    m_site_data_count = 0;

    m_stream_mqtt = new Stream(this,nav_config_str_list.join(":"));
    m_stream_ws = new Stream(this,config_ws_str); //temporary

    connect(m_stream_mqtt,&Stream::SignalReceiveData,this,&NavSensor::triggerReceivedData);
    connect(m_instance_cfg,&RadarEngine::RadarConfig::configValueChange,
            this,&NavSensor::triggerConfigChange);
}

void NavSensor::triggerConfigChange(const QString key, const QVariant val)
{
//    qDebug()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val;
    if(key == RadarEngine::NON_VOLATILE_NAV_NET_CONFIG)
    {
        m_stream_mqtt->SetConfig(val.toString());
    }
    else if(key == RadarEngine::NON_VOLATILE_NAV_CONTROL_GPS_AUTO)
    {
        if(val.toBool())
        {
            QString nav_config_str = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_NET_CONFIG).toString();
            QStringList nav_config_str_list = nav_config_str.split(":");

            if(nav_config_str_list.size() != 3)
            {
                qDebug()<<Q_FUNC_INFO<<"invalid config"<<nav_config_str;
                return;
            }

            m_topic = nav_config_str_list.last();
            m_stream_mqtt->SetConfig(nav_config_str);
        }
        else
        {
            QString nav_config_str = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_NET_CONFIG).toString();
            QStringList nav_config_str_list = nav_config_str.split(":");

            if(nav_config_str_list.size() != 3)
            {
                qDebug()<<Q_FUNC_INFO<<"invalid config"<<nav_config_str;
                return;
            }

            nav_config_str_list.removeLast();
            m_topic = "gps_man";
            nav_config_str_list.append(m_topic);

            m_stream_mqtt->SetConfig(nav_config_str_list.join(":"));
        }
    }
    else if(key == "RadarEngine::NON_VOLATILE_ARPA_NET_CONFIG") //todo use config
    {
        m_stream_ws->SetConfig(val.toString());
    }
}

void NavSensor::SendData(QString lat, QString lon, QString hdt)
{
    QString m_data = m_topic+":"+"?"+lat+"#"+lon+"#"+hdt+"!";
    m_stream_mqtt->SendData(m_data);
}

void NavSensor::SendSiteData(bool manual, QString lat, QString lon, QString hdt)
{
    m_site_data_count++;
    if(m_site_data_count > 10)
    {
        m_site_data_count = 0;

        QJsonObject obj;

        obj["mode"] = manual ? "manual" : "auto";
        obj["latitude"] = lat;
        obj["longitude"] = lon;
        obj["heading"] = hdt;
        obj["radar_min_range"] = distanceList.last();
        obj["radar_max_range"] = distanceList.first();

        QJsonDocument doc(obj);

        m_stream_ws->SendData(doc.toJson(QJsonDocument::Compact));
    }
}

bool NavSensor::isGPSDataValid(const QString lat_str, const QString lon_str)
{
    bool ok;
    double lat = lat_str.toDouble(&ok);
    if(!ok) return false;
    double lon = lon_str.toDouble(&ok);
    if(!ok) return false;

    if(fabs(lat) > 90.) return false;
    if(fabs(lon) > 180.) return false;

    return true;
}

bool NavSensor::isHDGDataValid(const QString hdg_str)
{
    bool ok;
    double hdg = hdg_str.toDouble(&ok);
    if(!ok) return false;

    if(hdg >= 0. && hdg <= 360.) return true;
    else return false;
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

void NavSensor::triggerReceivedData(const QString data)
{
    qDebug()<<Q_FUNC_INFO<<data;
    QString msg = data;
    if(msg.contains("gps:") )
        {
            m_no_osd_count = 0;

            qDebug()<<Q_FUNC_INFO<<"osd"<<msg.remove("gps");
            m_append_data_osd.append(msg);

            int index_hdr = m_append_data_osd.indexOf("?");
            if(index_hdr >= 0)
            {
                int index_end = m_append_data_osd.indexOf("!");
                if(index_end >= 0)
                {
                    if(index_end > index_hdr)
                    {
                        //?-6.939176#107.632770#31!
                        m_append_data_osd = m_append_data_osd.mid(index_hdr,index_end-index_hdr);
                        qDebug()<<Q_FUNC_INFO<<"filter"<<m_append_data_osd.remove("!").remove("?").remove("\r").remove("\n");
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
                        QStringList msg_list = m_append_data_osd.split("#",QString::SkipEmptyParts);
#else
                        QStringList msg_list = m_append_data_osd.split("#",QString::SkipEmptyParts);
#endif

                        if(msg_list.size() == 3)
                        {
                            if(isGPSDataValid(msg_list.at(0),msg_list.at(1)))
                            {
                                m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE,msg_list.at(0).toDouble());
                                m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE,msg_list.at(1).toDouble());
                                m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS, 3); //data valid
                            }
                            else m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS, 2); //data not valid

                            if(isHDGDataValid(msg_list.at(2)))
                            {
                                m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING,msg_list.at(2).toDouble());
                                m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING, 3); //data valid
                            }
                            else m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING, 2); //data not valid
                        }
                        else
                        {
                            qDebug()<<Q_FUNC_INFO<<"osd invalid";
                            m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING, 2); //data not valid
                            m_instance_cfg->setConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS, 2); //data not valid
                        }

                        m_append_data_osd.clear();
                    }
                    else
                    {
                        m_append_data_osd.remove(0,index_hdr);
                    }
                }
                qDebug()<<Q_FUNC_INFO<<index_end;
            }
            qDebug()<<Q_FUNC_INFO<<index_hdr;
        }}

void NavSensor::Reconnect()
{
    m_stream_mqtt->Reconnect();
    if(m_stream_ws->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream_ws->Reconnect();
}
