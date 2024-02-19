#include "baseresponsejson.h"
#include "sitedatasender.h"

#include <QJsonObject>
#include <QJsonDocument>

SiteDataSender::SiteDataSender(QObject *parent) : QObject(parent)
{
    qDebug()<<Q_FUNC_INFO;
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");

    initConfigWS();

    m_site_data_count = 0;

    connect(m_instance_cfg,&RadarEngine::RadarConfig::configValueChange,
            this,&SiteDataSender::triggerConfigChange);
}

void SiteDataSender::initConfigWS()
{
    QString config_ws_str = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_NET_CONFIG_WS).toString();
    QStringList config_ws_str_list = config_ws_str.split(";");

    if(config_ws_str_list.size() != 3)
    {
        qDebug()<<Q_FUNC_INFO<<"invalid config ws main"<<config_ws_str;
        exit(1);
    }

    QStringList config_ws_str_list$ = config_ws_str.split("$");
    if(config_ws_str_list$.size() != 2)
    {
        qDebug()<<Q_FUNC_INFO<<"invalid config ws site period"<<config_ws_str;
        exit(1);
    }

    bool ok;
    max_site_data_count = config_ws_str_list$.at(1).toInt(&ok);
    if (!ok) {
        max_site_data_count = 10;
        qWarning()<<Q_FUNC_INFO<<"invalid max_site_data_count"<<config_ws_str_list$.at(1)<<". will use default 10";
    }

    m_stream_ws = new Stream(this,config_ws_str);
}

void SiteDataSender::triggerConfigChange(const QString key, const QVariant val)
{
//    qDebug()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val;
    if(key == RadarEngine::NON_VOLATILE_NAV_NET_CONFIG_WS) //todo use config
    {
        m_stream_ws->SetConfig(val.toString());
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

        BaseResponseJson<QJsonObject> resp(0, "ok");

        QJsonDocument doc(resp.build().toObject());

        m_stream_ws->SendData(doc.toJson(QJsonDocument::Compact));
    }
}

void SiteDataSender::Reconnect()
{
    if(m_stream_ws->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream_ws->Reconnect();
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
