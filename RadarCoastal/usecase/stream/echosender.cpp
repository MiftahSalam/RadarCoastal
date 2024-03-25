#include <QApplication>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPoint>
#include <QFile>
#include <QDir>

#include "echosender.h"
#include "shared/utils.h"
#include "usecase/stream/baseresponsejson.h"

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, EchoSender)
#else
#include <QDebug>
#endif

EchoSender::EchoSender(QObject *parent)
    : QObject{parent}
{
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");
    m_re = RadarEngine::RadarEngine::GetInstance();
    m_ppi_grabber = m_re->m_radar_capture;

    initConfigWS();

#ifdef SAVE_CAPTURE
    initFile();
#endif

    connect(m_re->m_radar_capture,&RadarEngine::RadarImageCapture::signalSendEcho,this,&EchoSender::triggerSendData);
}

void EchoSender::initConfigWS()
{
    QString config_ws_str = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_ECHO_NET_CONFIG_WS).toString();
    QStringList config_ws_str_list = config_ws_str.split(";");

    if(config_ws_str_list.size() != 3)
    {
#ifdef USE_LOG4QT
    logger()->fatal()<<Q_FUNC_INFO<<"invalid config ws main"<<config_ws_str;
#else
        qDebug()<<Q_FUNC_INFO<<"invalid config ws main"<<config_ws_str;
        exit(1);
#endif
    }

    m_stream_ws = new Stream(this,config_ws_str);
}

void EchoSender::Reconnect()
{
    if(m_stream_ws->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream_ws->Reconnect();
}

void EchoSender::triggerSendData(const QString echoStr, const int vp_width, const int vp_height)
{
    EchoSender::BoundingBoxGps box = calculateBoundingBox(vp_width, vp_height);
    double curRange = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toDouble();
    auto timestamp = QDateTime::currentMSecsSinceEpoch();
    QJsonDocument json(buildJsonPackage(echoStr, timestamp, box, curRange));

    if(m_stream_ws->GetStreamStatus() == DeviceWrapper::INPUT_AVAIL)
        m_stream_ws->SendData(json.toJson(QJsonDocument::Compact));
#ifdef SAVE_CAPTURE
    saveJsonDataToFile(json.toJson());
#endif
#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO<<"base64"<<echoStr;
    logger()->debug()<<Q_FUNC_INFO<<"vp_width"<<vp_width;
    logger()->debug()<<Q_FUNC_INFO<<"vp_height"<<vp_height;
    logger()->debug()<<Q_FUNC_INFO<<"box.topLeftLat"<<box.topLeftLat;
    logger()->debug()<<Q_FUNC_INFO<<"box.topLeftLon"<<box.topLeftLon;
    logger()->debug()<<Q_FUNC_INFO<<"box.bottomLeftLat"<<box.bottomLeftLat;
    logger()->debug()<<Q_FUNC_INFO<<"box.bottomLeftLon"<<box.bottomLeftLon;
    logger()->debug()<<Q_FUNC_INFO<<"box.topRightLat"<<box.topRightLat;
    logger()->debug()<<Q_FUNC_INFO<<"box.topRightLon"<<box.topRightLon;
    logger()->debug()<<Q_FUNC_INFO<<"box.bottomRightLat"<<box.bottomRightLat;
    logger()->debug()<<Q_FUNC_INFO<<"box.bottomRightLon"<<box.bottomRightLon;
#else
    /*
    qDebug()<<Q_FUNC_INFO<<"base64"<<echoStr;
    */
    qDebug()<<Q_FUNC_INFO<<"vp_width"<<vp_width;
    qDebug()<<Q_FUNC_INFO<<"vp_height"<<vp_height;
    qDebug()<<Q_FUNC_INFO<<"box.topLeftLat"<<box.topLeftLat;
    qDebug()<<Q_FUNC_INFO<<"box.topLeftLon"<<box.topLeftLon;
    qDebug()<<Q_FUNC_INFO<<"box.bottomLeftLat"<<box.bottomLeftLat;
    qDebug()<<Q_FUNC_INFO<<"box.bottomLeftLon"<<box.bottomLeftLon;
    qDebug()<<Q_FUNC_INFO<<"box.topRightLat"<<box.topRightLat;
    qDebug()<<Q_FUNC_INFO<<"box.topRightLon"<<box.topRightLon;
    qDebug()<<Q_FUNC_INFO<<"box.bottomRightLat"<<box.bottomRightLat;
    qDebug()<<Q_FUNC_INFO<<"box.bottomRightLon"<<box.bottomRightLon;
#endif
}

void EchoSender::sendDataAsync(const RadarEngine::CaptureResult echo)
{
    if (echo.echo.isEmpty()) {
        qWarning()<<Q_FUNC_INFO<<"capture result isEmpty";
        return;
    }
    triggerSendData(echo.echo, echo.width, echo.height);
}

QJsonObject EchoSender::buildJsonPackage(const QString data, const quint64 ts, const BoundingBoxGps box, double curRange)
{
    QJsonObject obj;
    QJsonObject objBox;

    obj["timestamp"] = static_cast<qint64>(ts);
    obj["range"] = curRange;
    objBox["top_left_lat"] = box.topLeftLat;
    objBox["top_left_lon"] = box.topLeftLon;
    objBox["top_right_lat"] = box.topRightLat;
    objBox["top_right_lon"] = box.topRightLon;
    objBox["bottom_left_lat"] = box.bottomLeftLat;
    objBox["bottom_left_lon"] = box.bottomLeftLon;
    objBox["bottom_right_lat"] = box.bottomRightLat;
    objBox["bottom_right_lon"] = box.bottomRightLon;
    obj["bounding_box"] = objBox;
    obj["raw"] = data;

    BaseResponseJson<QJsonObject> resp(0, "ok", &obj);

    return resp.build();
}

void EchoSender::saveJsonDataToFile(QByteArray data)
{
    QString filename = qApp->applicationDirPath()+QDir::separator()+"record.json";
    QFile file(filename);
    if(file.open(QIODevice::Append | QIODevice::WriteOnly))
    {
        file.write(data);
        file.write(",");

        qInfo()<<Q_FUNC_INFO<<"filename"<<filename;
    } else qWarning()<<Q_FUNC_INFO<<"cannot open file"<<filename;
}

void EchoSender::initFile()
{
    QString filename = qApp->applicationDirPath()+QDir::separator()+"record.json";
    QFile file(filename);
    if(QFile::exists(filename))
    {
        QFile::remove(filename);
        qInfo()<<Q_FUNC_INFO<<"remove existing filename"<<filename;
    } else qWarning()<<Q_FUNC_INFO<<"cannot remove file"<<filename;

}

EchoSender::BoundingBoxGps EchoSender::calculateBoundingBox(const int vp_width, const int vp_height)
{
    double const curLat = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
    double const curLon = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();
    double curRange = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toDouble();
    const quint8 unit = static_cast<quint8>(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_UNIT).toUInt());

    switch (unit) {
    case 1:
        curRange *= KM_TO_NM;
        break;
    default:
        break;
    }

    QPoint boxLength(vp_width/2,vp_height/2);
    QPointF topLeft = Utils::PixToGPS(-boxLength.x(),boxLength.y(),vp_width,vp_height,curRange,curLat,curLon);
    QPointF bottomLeft = Utils::PixToGPS(-boxLength.x(),-boxLength.y(),vp_width,vp_height,curRange,curLat,curLon);
    QPointF topRight = Utils::PixToGPS(boxLength.x(),boxLength.y(),vp_width,vp_height,curRange,curLat,curLon);
    QPointF bottomRight = Utils::PixToGPS(boxLength.x(),-boxLength.y(),vp_width,vp_height,curRange,curLat,curLon);
    EchoSender::BoundingBoxGps box;

    box.topLeftLat = topLeft.y();
    box.topLeftLon = topLeft.x();
    box.bottomLeftLat = bottomLeft.y();
    box.bottomLeftLon = bottomLeft.x();
    box.topRightLat = topRight.y();
    box.topRightLon = topRight.x();
    box.bottomRightLat = bottomRight.y();
    box.bottomRightLon = topRight.x();

    return box;
}
