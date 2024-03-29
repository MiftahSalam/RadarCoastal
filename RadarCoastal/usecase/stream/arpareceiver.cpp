#include "arpareceiver.h"
#include "qjsonarray.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, GZAlarm)
#else
#include <QDebug>
#endif

ArpaReceiverDecoder::ArpaReceiverDecoder(QString data)
{
    m_data = data;
    m_parse_error = true;
}

ArpaReceiverDecoderJson::ArpaReceiverDecoderJson(QString data): ArpaReceiverDecoder(data)
{}

bool ArpaReceiverDecoderJson::isDataValid()
{
    QJsonParseError *error = new QJsonParseError();
    QJsonDocument::fromJson(m_data.toUtf8(), error);

    m_parse_error = error->error == QJsonParseError::NoError;

#ifdef USE_LOG4QT
    logger()->warn()<<Q_FUNC_INFO<<" m_parse_error "<<error->errorString();
#else
    qDebug()<<Q_FUNC_INFO<<"m_parse_error"<<error->errorString();
#endif

    return m_parse_error;
}

QList<TrackModel*> ArpaReceiverDecoderJson::decode()
{
    QList<TrackModel*> tracks;
    QJsonParseError *error = new QJsonParseError();
    auto json = QJsonDocument::fromJson(m_data.toUtf8(), error);

#ifdef USE_LOG4QT
    logger()->warn()<<Q_FUNC_INFO<<" m_parse_error "<<error->errorString();
#else
    qDebug()<<Q_FUNC_INFO<<"m_parse_error"<<error->errorString();
#endif

    /*
     * [{"brn":270.87890625,"crs":0,"lat":-7.345193089478958,"lon":106.40887640667856,"rng":2363.28125,"spd":0,"id":1},{"brn":283.0078125,"crs":0,"lat":-7.341604694320778,"lon":106.41296307660397,"rng":1953.125,"spd":0,"id":2}]
     */
    m_parse_error = error->error != QJsonParseError::NoError;
    if (!m_parse_error) {
       if(json.isArray())
       {
           auto trackArray = json.array();
           for (auto track : trackArray) {
               if (track.isObject()) {
                   auto obj = track.toObject();
                   TrackModel *trk = new TrackModel();

                   trk->timestamp = QDateTime::currentMSecsSinceEpoch();
                   trk->id = obj["id"].toInt();
                   trk->lat = obj["lat"].toDouble();
                   trk->lon = obj["lon"].toDouble();
                   trk->alt = obj["alt"].toDouble();
                   trk->rng = obj["rng"].toDouble();
                   trk->brn = obj["brn"].toDouble();
                   trk->spd = obj["spd"].toDouble();
                   trk->crs = obj["crs"].toDouble();

                   tracks.append(trk);
               }
#ifdef USE_LOG4QT
               else logger()->warn()<<Q_FUNC_INFO<<" not object";
#else
               else qWarning()<<Q_FUNC_INFO<<"not object";
#endif
           }
       }
#ifdef USE_LOG4QT
               else logger()->warn()<<Q_FUNC_INFO<<" not array";
#else
       else qWarning()<<Q_FUNC_INFO<<"not array";
#endif
    }

    return tracks;
}

ArpaReceiver::ArpaReceiver(QObject *parent)
    : QObject{parent}
{
    qDebug()<<Q_FUNC_INFO;
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");

    initConfig();
}

void ArpaReceiver::initConfig()
{
    QString config_str = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_ARPA_NET_CONFIG).toString();
    QStringList config_str_list = config_str.split(":");

    if(config_str_list.size() != 3)
    {
#ifdef USE_LOG4QT
    logger()->fatal()<<Q_FUNC_INFO<<" invalid config "<<config_str;
#else
        qDebug()<<Q_FUNC_INFO<<"invalid config"<<config_str;
        exit(1);
#endif
    }

    m_topic = config_str_list.last();
    m_stream = new Stream(this,config_str);
    connect(m_stream,&Stream::SignalReceiveData,this,&ArpaReceiver::triggerReceivedData);
}

void ArpaReceiver::triggerReceivedData(const QString data)
{
    qDebug()<<Q_FUNC_INFO<<data;
    QString msg = data;
    if(msg.contains("radar"))
    {
        msg.remove("radar@");

#ifdef USE_LOG4QT
        logger()->trace()<<Q_FUNC_INFO<<" arpa raw data: "<<msg;
#else
        qDebug()<<Q_FUNC_INFO<<"arpa raw data"<<msg;
#endif

        m_append_data.append(msg);

        ArpaReceiverDecoderJson decode(m_append_data);
        if (decode.isDataValid()) {
            emit signalNewTrack(decode.decode());
            m_append_data.clear();
        }
#ifdef USE_LOG4QT
        else logger()->trace()<<Q_FUNC_INFO<<" m_append_data: "<<m_append_data;
#else
        else qDebug()<<Q_FUNC_INFO<<"m_append_data"<<m_append_data;
#endif

        if (m_append_data.size() > 1000) {
            m_append_data.clear();
        }
    }
#ifdef USE_LOG4QT
    else logger()->trace()<<Q_FUNC_INFO<<" not arpa data format";
#else
    else qDebug()<<Q_FUNC_INFO<<"not arpa data format";
#endif

}

void ArpaReceiver::Reconnect()
{
    m_stream->Reconnect();
}
