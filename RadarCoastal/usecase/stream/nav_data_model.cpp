#include "nav_data_model.h"
#include "infra/stream/mqttdevicewrapper.h"
#include "shared/utils.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>

#include <math.h>

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, NavDataEncoder)
#else
#include <QDebug>
#endif

NavDataEncoder::NavDataEncoder(long long ts,
                               double lat,
                               double lon,
                               double hdg,
                               bool gps_man,
                               bool hdg_man,
                               quint8 status_gps,
                               quint8 status_hdg
                               )
{
    m_data.timestamp = ts;
    m_data.lat = lat;
    m_data.lon = lon;
    m_data.hdg = hdg;
    m_data.gps_man = gps_man;
    m_data.hdg_man = hdg_man;
    m_data.status_gps = status_gps;
    m_data.status_hdg = status_hdg;
}

NavDataEncoder::NavDataEncoder(NavDataModel data)
{
    m_data = data;
}

NavDataEncoderCustom::NavDataEncoderCustom(long long ts,
                                           double lat,
                                           double lon,
                                           double hdg,
                                           bool gps_man,
                                           bool hdg_man,
                                           quint8 status_gps,
                                           quint8 status_hdg
                                           )
    : NavDataEncoder(ts, lat, lon, hdg, gps_man, hdg_man, status_gps, status_hdg)
{
}

NavDataEncoderCustom::NavDataEncoderCustom(NavDataModel data): NavDataEncoder(data)
{
}

QString NavDataEncoderCustom::encode()
{
    QString decodedData = QString("?%1#%2#%3!")
            .arg(QString::number(m_data.lat, 'f', 6))
            .arg(QString::number(m_data.lon, 'f', 6))
            .arg(QString::number(m_data.hdg, 'f', 1))
            ;
    return decodedData;
}


NavDataEncoderJson::NavDataEncoderJson(long long ts,
                                       double lat,
                                       double lon,
                                       double hdg,
                                       bool gps_man,
                                       bool hdg_man,
                                       quint8 status_gps,
                                       quint8 status_hdg
                                       )
    : NavDataEncoder(ts, lat, lon, hdg, gps_man, hdg_man, status_gps, status_hdg)
{
}

NavDataEncoderJson::NavDataEncoderJson(NavDataModel data): NavDataEncoder(data)
{
}

QString NavDataEncoderJson::encode()
{
    QJsonObject obj;
    QJsonObject objPos;
    QJsonObject objHdt;
    QJsonObject objRadar;

    objPos["mode"] = m_data.gps_man ? "manual" : "auto";
    objPos["status"] = m_data.status_gps;
    objPos["latitude"] = m_data.lat;
    objPos["longitude"] = m_data.lon;

    objHdt["mode"] = m_data.hdg_man ? "manual" : "auto";
    objHdt["status"] = m_data.status_hdg;
    objHdt["heading"] = m_data.hdg;

    obj["position"] = objPos;
    obj["heading"] = objHdt;

    QJsonDocument doc(obj);

    return QString(doc.toJson(QJsonDocument::Compact));
}

NavDataDecoder::NavDataDecoder()
{
}

void NavDataDecoder::update(QByteArray data)
{
    if (m_data.size() > 500)
    {
        m_data.clear();
    }
    m_data.append(data);
}

void NavDataDecoder::reset()
{
    m_data.clear();
}

bool NavDataDecoder::isGPSDataValid(const double lat, const double lon)
{
    if(fabs(lat) > 90.) return false;
    if(fabs(lon) > 180.) return false;

    return true;
}

bool NavDataDecoder::isHDGDataValid(const double hdg)
{
    if(hdg >= 0. && hdg <= 360.) return true;
    else return false;
}

NavDataDecoderCustom::NavDataDecoderCustom(): NavDataDecoder()
{
}

NavDataDecoderNMEA::NavDataDecoderNMEA(): NavDataDecoder()
{
}

NavDataDecoderJson::NavDataDecoderJson(): NavDataDecoder()
{
}

void NavDataDecoderCustom::reset()
{
    m_append_data_osd.clear();
    NavDataDecoder::reset();
}

void NavDataDecoderNMEA::reset()
{
    m_append_data_osd.clear();
    NavDataDecoder::reset();
}

void NavDataDecoderJson::reset()
{
    m_append_data_osd.clear();
    NavDataDecoder::reset();
}

NavDataModel NavDataDecoderJson::decode()
{
    QString msg(m_data);
    NavDataModel result;

    QJsonParseError *error = new QJsonParseError();
    auto json = QJsonDocument::fromJson(m_data, error);

    /*
     * "{\"heading\":{\"heading\":31,\"mode\":\"auto\",\"status\":3},\"position\":{\"latitude\":63.256138,\"longitude\":10.21429,\"mode\":\"auto\",\"status\":3}}"
     */
    bool isError = error->error != QJsonParseError::NoError;
    if (!isError)
    {
        reset();
        if(json.isObject())
        {
            auto position = json["position"];
            auto hdg = json["heading"];
            result.timestamp = QDateTime::currentMSecsSinceEpoch();
            result.lat = position["latitude"].toDouble();
            result.lon = position["longitude"].toDouble();
            result.status_gps = position["status"].toInt();
            result.gps_man = position["mode"].toString() == "manual";
            result.hdg = hdg["heading"].toDouble();
            result.status_hdg = hdg["status"].toInt();
            result.hdg_man = hdg["mode"].toString() == "manual";
        }
#ifdef USE_LOG4QT
        else logger()->warn()<<Q_FUNC_INFO<<" not object";
#else
        else qWarning()<<Q_FUNC_INFO<<"not object";
#endif
    }
    else
    {
        result.timestamp = QDateTime::currentMSecsSinceEpoch();
        result.status_hdg = 2;
        result.status_gps = 2;
#ifdef USE_LOG4QT
        logger()->warn()<<Q_FUNC_INFO<<" parse_error "<<error->errorString();
        logger()->warn()<<Q_FUNC_INFO<<" msg "<<msg;
#else
        qDebug()<<Q_FUNC_INFO<<"m_parse_error"<<error->errorString();
#endif

    }
    return result;
}

NavDataModel NavDataDecoderNMEA::decode()
{
#ifdef USE_LOG4QT
    logger()->debug() << Q_FUNC_INFO << "m_data: " << m_data;
#else
    qDebug() << Q_FUNC_INFO << data;
#endif

    QString msg(m_data);
    NavDataModel result;
//    const QString NMEA_HEADER = "%"; //temp
//    const QString NMEA_TAIL1 = "#"; //temp for test
    const QString NMEA_HEADER = "$"; //not work in mqtt. why ???
    const QString NMEA_TAIL1 = "\r";
    const QString NMEA_TAIL2 = "\n";

    result.timestamp = QDateTime::currentSecsSinceEpoch();

    auto start = QDateTime::currentMSecsSinceEpoch();

    m_append_data_osd.append(msg);

    while (m_append_data_osd.size() > 0) {
        auto now = QDateTime::currentMSecsSinceEpoch();
        if (now - start > 2000) {
            logger()->warn() << Q_FUNC_INFO << " parse processing reach timeout ";
            m_append_data_osd.clear();
            break;
        }

        int index_hdr = m_append_data_osd.indexOf(NMEA_HEADER);
        if(index_hdr >= 0)
        {
            int index_end = m_append_data_osd.indexOf(NMEA_TAIL1);
            if(index_end >= 0)
            {
                if(index_end > index_hdr)
                {
                    auto m_append_data_osd_buf = m_append_data_osd.mid(index_hdr,index_end-index_hdr);
    #ifdef USE_LOG4QT
                    logger()->trace()<<Q_FUNC_INFO<<"filter: "<<m_append_data_osd_buf;
    #else
                    qDebug()<<Q_FUNC_INFO<<"filter: "<<m_append_data_osd_buf;
    #endif

    #if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
                    QStringList msg_list = m_append_data_osd_buf.split(",", Qt::SkipEmptyParts);
    #else
                    QStringList msg_list = m_append_data_osd_buf.split(",",QString::SkipEmptyParts);
    #endif
                    if(m_append_data_osd_buf.contains("GGA") && msg_list.size() > 8)
                    {
                        bool ok;

                        result.lat = msg_list.at(2).toDouble(&ok);
                        if(!ok)
                        {
    #ifdef USE_LOG4QT
                            logger()->warn()<<Q_FUNC_INFO<<" invalid latitude: "<<msg_list.at(2);
    #else
                            qDebug()<<Q_FUNC_INFO<<" invalid latitude: "<<msg_list.at(2);
    #endif
                            result.status_gps = 2; //data not valid
                        }
                        else
                        {
                            result.lon = msg_list.at(4).toDouble(&ok);
                            if(!ok)
                            {
    #ifdef USE_LOG4QT
                                logger()->warn()<<Q_FUNC_INFO<<" invalid longitude: "<<msg_list.at(4);
    #else
                                qDebug()<<Q_FUNC_INFO<<" invalid latitude: "<<msg_list.at(2);
    #endif
                                result.status_gps = 2; //data not valid
                            }
                            else
                            {
                                Utils::GPSDMM2Dec(msg_list.at(4)+","+msg_list.at(5),msg_list.at(2)+","+msg_list.at(3), &result.lon, &result.lat);

                                if (isGPSDataValid(result.lat, result.lon)) {
                                    result.status_gps = 3; //data valid
                                } else {
                                    result.status_gps = 2; //data not valid
                                }
                            }
                        }
                    }
                    else if(m_append_data_osd_buf.contains("HDT") && msg_list.size() > 2)
                    {
                        bool ok;

                        result.hdg = msg_list.at(1).toDouble(&ok);
                        if(!ok)
                        {
    #ifdef USE_LOG4QT
                            logger()->warn()<<Q_FUNC_INFO<<" invalid heading: "<<msg_list.at(1);
    #else
                            qDebug()<<Q_FUNC_INFO<<" invalid heading: "<<msg_list.at(2);
    #endif
                            result.status_hdg = 2; //data not valid
                        }
                        else
                        {
                            if (isHDGDataValid(result.hdg)) {
                                result.status_hdg = 3; //data valid
                            } else {
                                result.status_hdg = 2; //data not valid
                            }
                        }
                    }
                    else if(m_append_data_osd_buf.contains("COMPASS") && msg_list.size() > 1)
                    {
                        bool ok;
                        QString buf_str;
                        for (int var = 0; var < msg_list.at(1).size(); var++) {
                            if (msg_list.at(1).at(var).isDigit()) {
                                buf_str.append(msg_list.at(1).at(var));
                            }
                        }
                        buf_str = buf_str.trimmed();

                        result.hdg = buf_str.toDouble(&ok);
                        if(!ok)
                        {
    #ifdef USE_LOG4QT
                            logger()->warn()<<Q_FUNC_INFO<<" invalid heading: "<<buf_str;
    #else
                            qDebug()<<Q_FUNC_INFO<<" invalid heading: "<<msg_list.at(2);
    #endif
                            result.status_hdg = 2; //data not valid
                        }
                        else
                        {
                            if (isHDGDataValid(result.hdg)) {
                                result.status_hdg = 3; //data valid
                            } else {
                                result.status_hdg = 2; //data not valid
                            }
                        }
                    }
                    else if(m_append_data_osd_buf.contains("RMC"))
                    {
    #ifdef USE_LOG4QT
                        logger()->debug()<<Q_FUNC_INFO<<"RMC not implemented";
    #else
                        qDebug()<<Q_FUNC_INFO<<"osd invalid";
    #endif
                    }
                    else if(m_append_data_osd_buf.contains("GSA"))
                    {
    #ifdef USE_LOG4QT
                        logger()->debug()<<Q_FUNC_INFO<<"GSA not implemented";
    #else
                        qDebug()<<Q_FUNC_INFO<<"osd invalid";
    #endif
                    }
                    else if(m_append_data_osd_buf.contains("GSV"))
                    {
    #ifdef USE_LOG4QT
                        logger()->debug()<<Q_FUNC_INFO<<"GSV not implemented";
    #else
                        qDebug()<<Q_FUNC_INFO<<"osd invalid";
    #endif
                    }
                    else if(m_append_data_osd_buf.contains("GLL"))
                    {
    #ifdef USE_LOG4QT
                        logger()->debug()<<Q_FUNC_INFO<<"GLL not implemented";
    #else
                        qDebug()<<Q_FUNC_INFO<<"osd invalid";
    #endif
                    }
                    else
                    {
    #ifdef USE_LOG4QT
                        logger()->warn()<<Q_FUNC_INFO<<" osd invalid";
    #else
                        qDebug()<<Q_FUNC_INFO<<"osd invalid";
    #endif
                        result.status_gps = 2; //data not valid
                        result.status_hdg = 2; //data not valid
                    }

                    //                reset();
                    m_append_data_osd = m_append_data_osd.remove(index_hdr, index_end - index_hdr);
                    m_data = m_append_data_osd.toUtf8();
                }
                else
                {
                    m_append_data_osd.remove(0,index_hdr);
                }
            } else {
                m_append_data_osd.clear();
            }
    #ifdef USE_LOG4QT
            logger()->trace()<<Q_FUNC_INFO<<"index_end"<<index_end;
    #else
            qDebug()<<Q_FUNC_INFO<<index_end;
    #endif
        }

    }

    return result;
}

NavDataModel NavDataDecoderCustom::decode()
{
    QString msg(m_data);
    NavDataModel result;
    if(msg.contains("gps"+MQTT_MESSAGE_SEPARATOR) )
    {
        result.timestamp = QDateTime::currentSecsSinceEpoch();
#ifdef USE_LOG4QT
        logger()->trace()<<Q_FUNC_INFO<<"osd data: "<<msg.remove("gps");
#else
        qDebug()<<Q_FUNC_INFO<<"osd"<<msg.remove("gps");
#endif
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
#ifdef USE_LOG4QT
                    logger()->trace()<<Q_FUNC_INFO<<"filter: "<<m_append_data_osd.remove("!").remove("?").remove("\r").remove("\n");
#else
                    qDebug()<<Q_FUNC_INFO<<"filter"<<m_append_data_osd.remove("!").remove("?").remove("\r").remove("\n");
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
                    QStringList msg_list = m_append_data_osd.split("#",QString::SkipEmptyParts);
#else
                    QStringList msg_list = m_append_data_osd.split("#",QString::SkipEmptyParts);
#endif

                    if(msg_list.size() == 3)
                    {
                        if(isGPSDataValid(msg_list.at(0).toDouble(),msg_list.at(1).toDouble()))
                        {
                            result.lat = msg_list.at(0).toDouble();
                            result.lon = msg_list.at(1).toDouble();
                            result.status_gps = 3; //data valid
                        }
                        else
                        {
                            result.status_gps = 2; //data not valid
                        }
                        if(isHDGDataValid(msg_list.at(2).toDouble()))
                        {
                            result.hdg = msg_list.at(2).toDouble();
                            result.status_hdg = 3; //data valid
                        }
                        else
                        {
                            result.status_hdg = 2; //data not valid
                        }
                    }
                    else
                    {
#ifdef USE_LOG4QT
                        logger()->warn()<<Q_FUNC_INFO<<" osd invalid";
#else
                        qDebug()<<Q_FUNC_INFO<<"osd invalid";
#endif
                        result.status_gps = 2; //data not valid
                        result.status_hdg = 2; //data not valid
                    }

                    reset();
                }
                else
                {
                    m_append_data_osd.remove(0,index_hdr);
                }
            }
#ifdef USE_LOG4QT
            logger()->trace()<<Q_FUNC_INFO<<"index_end"<<index_end;
#else
            qDebug()<<Q_FUNC_INFO<<index_end;
#endif
        }
#ifdef USE_LOG4QT
        logger()->trace()<<Q_FUNC_INFO<<"index_hdr"<<index_hdr;
#else
        qDebug()<<Q_FUNC_INFO<<index_hdr;
#endif
    }

    return result;
}
