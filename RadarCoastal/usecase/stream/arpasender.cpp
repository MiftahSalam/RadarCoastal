#include "arpasender.h"
#include "baseresponsejson.h"

#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "shared/utils.h"

ArpaSenderDecoder::ArpaSenderDecoder(int id,
                                     double lat,
                                     double lon,
                                     double alt,
                                     double rng,
                                     double brn,
                                     double spd,
                                     double crs)
{
    isArray = false;

    ArpaSenderModel *data = new ArpaSenderModel();
    data->id = id;
    data->lat = lat;
    data->lon = lon;
    data->alt = alt;
    data->rng = rng;
    data->brn = brn;
    data->spd = spd;
    data->crs = crs;

    m_data.append(data);
}

ArpaSenderDecoder::ArpaSenderDecoder(TrackModel data)
{
    isArray = false;

    ArpaSenderModel *model = new ArpaSenderModel();
    model->id = data.id;
    model->lat = data.lat;
    model->lon = data.lon;
    model->alt = data.alt;
    model->rng = data.rng;
    model->brn = data.brn;
    model->spd = data.spd;
    model->crs = data.crs;

    m_data.append(model);
}

ArpaSenderDecoder::ArpaSenderDecoder(QList<TrackModel*> data)
{
    if (data.size() <= 0) {
        qWarning()<<Q_FUNC_INFO<<"invalid parameter input. input must at least 1 size. "<<data;
        return;
    }

    isArray = true;

    foreach (auto m, data) {
        ArpaSenderModel *model = new ArpaSenderModel();
        model->id = m->id;
        model->lat = m->lat;
        model->lon = m->lon;
        model->alt = m->alt;
        model->rng = m->rng;
        model->brn = m->brn;
        model->spd = m->spd;
        model->crs = m->crs;

        m_data.append(model);
    }
}

ArpaSenderDecoderJson::ArpaSenderDecoderJson(
        int id,
        double lat,
        double lon,
        double alt,
        double rng,
        double brn,
        double spd,
        double crs) : ArpaSenderDecoder(id, lat, lon, alt, rng, brn, spd, crs)
{}

ArpaSenderDecoderJson::ArpaSenderDecoderJson(TrackModel data): ArpaSenderDecoder(data)
{}

ArpaSenderDecoderJson::ArpaSenderDecoderJson(QList<TrackModel*> data): ArpaSenderDecoder(data)
{}

QString ArpaSenderDecoderJson::decode()
{
    QJsonDocument doc = decodeJsonDoc();

    return QString(doc.toJson(QJsonDocument::Compact));
}

QJsonDocument ArpaSenderDecoderJson::decodeJsonDoc()
{
    QJsonDocument doc;

    if (m_data.size() > 1) {
        QJsonArray array;

        foreach (auto m, m_data) {
            QJsonObject obj;
            obj["id"] = m->id;
            obj["lat"] = m->lat;
            obj["lon"] = m->lon;
            obj["alt"] = m->alt;
            obj["rng"] = m->rng;
            obj["brn"] = m->brn;
            obj["spd"] = m->spd;
            obj["crs"] = m->crs;

            array.append(obj);
        }

        doc = QJsonDocument(array);
    } else {
        if(isArray)
        {
            QJsonArray array;

            foreach (auto m, m_data) {
                QJsonObject obj;
                obj["id"] = m->id;
                obj["lat"] = m->lat;
                obj["lon"] = m->lon;
                obj["alt"] = m->alt;
                obj["rng"] = m->rng;
                obj["brn"] = m->brn;
                obj["spd"] = m->spd;
                obj["crs"] = m->crs;

                array.append(obj);
            }

            doc = QJsonDocument(array);
        }
        else
        {
            QJsonObject obj;

            obj["id"] = m_data[0]->id;
            obj["lat"] = m_data[0]->lat;
            obj["lon"] = m_data[0]->lon;
            obj["alt"] = m_data[0]->alt;
            obj["rng"] = m_data[0]->rng;
            obj["brn"] = m_data[0]->brn;
            obj["spd"] = m_data[0]->spd;
            obj["crs"] = m_data[0]->crs;

            doc = QJsonDocument(obj);
        }
    }

    return doc;
}

ArpaSenderDecoderNMEA::ArpaSenderDecoderNMEA(
        int id,
        double lat,
        double lon,
        double alt,
        double rng,
        double brn,
        double spd,
        double crs) : ArpaSenderDecoder(id, lat, lon, alt, rng, brn, spd, crs)
{}

ArpaSenderDecoderNMEA::ArpaSenderDecoderNMEA(TrackModel data): ArpaSenderDecoder(data)
{}

ArpaSenderDecoderNMEA::ArpaSenderDecoderNMEA(QList<TrackModel*> data): ArpaSenderDecoder(data)
{}

QString ArpaSenderDecoderNMEA::decode()
{
    QString decodedData;
    foreach (auto m, m_data) {
        QString id_str = QString::number(m->id);
        QString lat_str = QString::number(m->lat,'f',Utils::FIVE_PRECISION);
        QString lon_str = QString::number(m->lon,'f',Utils::FIVE_PRECISION);
        QString rng_str = QString::number(m->rng,'f',Utils::ONE_PRECISION);
        QString brn_str = QString::number(m->brn,'f',Utils::ONE_PRECISION);
        QString spd_str = QString::number(m->spd,'f',Utils::ONE_PRECISION);
        QString crs_str = QString::number(m->crs,'f',Utils::ONE_PRECISION);
    //    QString alt_str =  QString::number(m_data.alt,'f',Utils::ONE_PRECISION);
        QString populate = id_str+"#"+rng_str+"#"+brn_str+"#"+lat_str+"#"+lon_str+"#"+spd_str+"#"+crs_str+"\r\n";

        //    lat_str.replace(".",",");
        //    lon_str.replace(".",",");
        //    alt_str.replace(".",",");
        //    spd_str.replace(".",",");
        //    crs_str.replace(".",",");

        decodedData.append(populate);
    }

    return decodedData;
}

ArpaSender::ArpaSender(QObject *parent)
    : QObject{parent}
{
    qDebug()<<Q_FUNC_INFO;

    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");

    initConfigMqtt();
    initConfigWS();
}

void ArpaSender::sendMqtt(ArpaSenderDecoder *decoder)
{
    QString mq_data = m_topic+MQTT_MESSAGE_SEPARATOR+decoder->decode();

    if(m_stream_mqtt->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream_mqtt->Reconnect();
    else m_stream_mqtt->SendData(mq_data);
}

void ArpaSender::sendWS(ArpaSenderDecoder *decoder)
{
    ArpaSenderDecoderJson *decoderJson = dynamic_cast<ArpaSenderDecoderJson*>(decoder);
    QJsonDocument doc;
    auto decoderDoc = decoderJson->decodeJsonDoc();

    if(decoderDoc.isArray())
    {
        QJsonArray array(decoderDoc.array());
        BaseResponseJson<QJsonArray> resp(0, "ok", &array);
        doc = QJsonDocument(resp.build());
    }
    else if(decoderDoc.isObject())
    {
        QJsonObject obj(decoderDoc.object());
        BaseResponseJson<QJsonObject> resp(0, "ok", &obj);
        doc = QJsonDocument(resp.build());
    }

    if(m_stream_ws->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream_ws->Reconnect();
    m_stream_ws->SendData(doc.toJson(QJsonDocument::Compact));
}

void ArpaSender::SendManyData(QList<TrackModel *> data)
{
    foreach (auto m, data) {
        QPointF gpsCorrection = Utils::GpsAbsolute(m->lat,m->lon);

        m->lat = gpsCorrection.y();
        m->lon = gpsCorrection.x();
    }

    ArpaSenderDecoder *decoder = dynamic_cast<ArpaSenderDecoder*>(new ArpaSenderDecoderJson(data));
    sendMqtt(decoder);
    sendWS(decoder);

    delete decoder;
}

void ArpaSender::SendOneData(TrackModel data)
{
    QPointF gpsCorrection = Utils::GpsAbsolute(data.lat,data.lon);

    data.lat = gpsCorrection.y();
    data.lon = gpsCorrection.x();

    ArpaSenderDecoder *decoder = dynamic_cast<ArpaSenderDecoder*>(new ArpaSenderDecoderJson(data));
    sendMqtt(decoder);
    sendWS(decoder);

    delete decoder;
}

void ArpaSender::SendOneData(int id,
                          double lat,
                          double lon,
                          double alt,
                          double rng,
                          double brn,
                          double spd,
                          double crs
                          )
{
    QPointF gpsCorrection = Utils::GpsAbsolute(lat,lon);

    lat = gpsCorrection.y();
    lon = gpsCorrection.x();

    ArpaSenderDecoder *decoder = dynamic_cast<ArpaSenderDecoder*>(new ArpaSenderDecoderJson(
                                                                      id,
                                                                      lat,
                                                                      lon,
                                                                      alt,
                                                                      rng,
                                                                      brn,
                                                                      spd,
                                                                      crs));
    sendMqtt(decoder);
    sendWS(decoder);

    delete decoder;
}

void ArpaSender::initConfigWS()
{
    QString config_ws_str = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_ARPA_NET_CONFIG_WS).toString();
    QStringList config_ws_str_list = config_ws_str.split(";");

    if(config_ws_str_list.size() != 3)
    {
        qDebug()<<Q_FUNC_INFO<<"invalid config ws main"<<config_ws_str;
        exit(1);
    }

    m_stream_ws = new Stream(this,config_ws_str);
}

void ArpaSender::initConfigMqtt()
{
    QString config_str = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_ARPA_NET_CONFIG).toString();
    QStringList config_str_list = config_str.split(":");

    if(config_str_list.size() != 3)
    {
        qDebug()<<Q_FUNC_INFO<<"invalid config mqtt"<<config_str;
        exit(1);
    }

    m_topic = config_str_list.last();
    m_stream_mqtt = new Stream(this,config_str);
    connect(RadarEngine::RadarConfig::getInstance(""),&RadarEngine::RadarConfig::configValueChange,
            this,&ArpaSender::triggerConfigChange);
}

void ArpaSender::triggerConfigChange(const QString key, const QVariant val)
{
    //    qDebug()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val;
    if(key == RadarEngine::NON_VOLATILE_ARPA_NET_CONFIG)
    {
        m_stream_mqtt->SetConfig(val.toString());
    }
}
