#include "arpa_sender_model.h"

#include "shared/utils.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>

ArpaSenderDecoder::ArpaSenderDecoder(long long ts,
                                     int id,
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
    data->timestamp = ts;
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
    model->timestamp = data.timestamp;
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
        model->timestamp = m->timestamp;
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
        long long ts,
        int id,
        double lat,
        double lon,
        double alt,
        double rng,
        double brn,
        double spd,
        double crs) : ArpaSenderDecoder(ts, id, lat, lon, alt, rng, brn, spd, crs)
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

            obj["timestamp"] = m->timestamp;
            obj["track_id"] = m->id;
            obj["latitude"] = m->lat;
            obj["longitude"] = m->lon;
            obj["range"] = m->rng;
            obj["bearing"] = m->brn;
            obj["speed"] = m->spd;
            obj["course"] = m->crs;

            array.append(obj);
        }

        doc = QJsonDocument(array);
    } else {
        if(isArray)
        {
            QJsonArray array;

            foreach (auto m, m_data) {
                QJsonObject obj;

                obj["timestamp"] = m->timestamp;
                obj["track_id"] = m->id;
                obj["latitude"] = m->lat;
                obj["longitude"] = m->lon;
                obj["alt"] = m->alt;
                obj["range"] = m->rng;
                obj["bearing"] = m->brn;
                obj["speed"] = m->spd;
                obj["course"] = m->crs;

                array.append(obj);
            }

            doc = QJsonDocument(array);
        }
        else
        {
            QJsonObject obj;

            obj["timestamp"] = m_data[0]->timestamp;
            obj["track_id"] = m_data[0]->id;
            obj["latitude"] = m_data[0]->lat;
            obj["longitude"] = m_data[0]->lon;
            obj["alt"] = m_data[0]->alt;
            obj["range"] = m_data[0]->rng;
            obj["bearing"] = m_data[0]->brn;
            obj["speed"] = m_data[0]->spd;
            obj["course"] = m_data[0]->crs;

            doc = QJsonDocument(obj);
        }
    }

    return doc;
}

ArpaSenderDecoderNMEA::ArpaSenderDecoderNMEA(
        long long ts,
        int id,
        double lat,
        double lon,
        double alt,
        double rng,
        double brn,
        double spd,
        double crs) : ArpaSenderDecoder(ts, id, lat, lon, alt, rng, brn, spd, crs)
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
