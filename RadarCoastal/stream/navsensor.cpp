#include "navsensor.h"

NavSensor::NavSensor(QObject *parent) : QObject(parent)
{
    qDebug()<<Q_FUNC_INFO;
    QString nav_config_str = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_NET_CONFIG).toString();
    QStringList nav_config_str_list = nav_config_str.split(":");

    if(nav_config_str_list.size() != 3)
    {
        qDebug()<<Q_FUNC_INFO<<"invalid config"<<nav_config_str;
        exit(1);
    }

    topic = nav_config_str_list.last();
    append_data_osd = "";
    no_osd_count = 11;

    stream = new Stream(this,nav_config_str);
    connect(stream,&Stream::signal_receiveData,this,&NavSensor::trigger_receivedData);
    connect(RadarConfig::RadarConfig::getInstance(""),&RadarConfig::RadarConfig::configValueChange,
            this,&NavSensor::trigger_configChange);
}

void NavSensor::trigger_configChange(const QString key, const QVariant val)
{
//    qDebug()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val;
    if(key == RadarConfig::NON_VOLATILE_NAV_NET_CONFIG)
    {
        stream->setConfig(val.toString());
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

void NavSensor::updateStatus()
{
    no_osd_count++;
    if(no_osd_count > 110) no_osd_count = 11;
    if(no_osd_count < 11) return;

    switch (stream->getStreamStatus()) {
    case DeviceWrapper::NOT_AVAIL:
        RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_NAV_STATUS_HEADING, 0); //offline
        RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_NAV_STATUS_GPS, 0); //offline
        break;
    case DeviceWrapper::NO_INPUT_DATA:
        RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_NAV_STATUS_HEADING, 1); //no data
        RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_NAV_STATUS_GPS, 1); //no data
        break;
    default:
        break;
    }
}

void NavSensor::trigger_receivedData(const QString data)
{
    qDebug()<<Q_FUNC_INFO<<data;
    QString msg = data;
    if(msg.contains("gps") )
        {
            no_osd_count = 0;

            qDebug()<<Q_FUNC_INFO<<"osd"<<msg.remove("gps");
            append_data_osd.append(msg);

            int index_hdr = append_data_osd.indexOf("?");
            if(index_hdr >= 0)
            {
                int index_end = append_data_osd.indexOf("!");
                if(index_end >= 0)
                {
                    if(index_end > index_hdr)
                    {
                        //?-6.939176#107.632770#31!
                        append_data_osd = append_data_osd.mid(index_hdr,index_end-index_hdr);
                        qDebug()<<Q_FUNC_INFO<<"filter"<<append_data_osd.remove("!").remove("?").remove("\r").remove("\n");
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
                        QStringList msg_list = append_data_osd.split("#",QString::SkipEmptyParts);
#else
                        QStringList msg_list = append_data_osd.split("#",Qt::SkipEmptyParts);
#endif

                        if(msg_list.size() == 3)
                        {
                            if(isGPSDataValid(msg_list.at(0),msg_list.at(1)))
                            {
                                RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_LATITUDE,msg_list.at(0).toDouble());
                                RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE,msg_list.at(1).toDouble());
                                RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_NAV_STATUS_GPS, 3); //data valid
                            }
                            else RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_NAV_STATUS_GPS, 2); //data not valid

                            if(isHDGDataValid(msg_list.at(2)))
                            {
                                RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_HEADING,msg_list.at(2).toDouble());
                                RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_NAV_STATUS_HEADING, 3); //data valid
                            }
                            else RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_NAV_STATUS_HEADING, 2); //data not valid
                        }
                        else
                        {
                            qDebug()<<Q_FUNC_INFO<<"osd invalid";
                            RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_NAV_STATUS_HEADING, 2); //data not valid
                            RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_NAV_STATUS_GPS, 2); //data not valid
                        }

                        append_data_osd.clear();
                    }
                    else
                    {
                        append_data_osd.remove(0,index_hdr);
                    }
                }
                qDebug()<<Q_FUNC_INFO<<index_end;
            }
            qDebug()<<Q_FUNC_INFO<<index_hdr;
        }}

void NavSensor::reconnect()
{
    stream->reconnect();
}
