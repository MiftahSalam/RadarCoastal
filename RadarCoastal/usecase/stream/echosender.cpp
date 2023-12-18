#include "qapplication.h"
#include "qdatetime.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qpoint.h"
#include "qfile"
#include "qdir"

#include "echosender.h"
#include "shared/utils.h"

EchoSender::EchoSender(QObject *parent)
    : QObject{parent}
{
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");
    m_ppi_grabber = PPIGrabber::GetInstance(this);

    initFile();

    connect(m_ppi_grabber,&PPIGrabber::signalSendEcho,this,&EchoSender::triggerSendData);
}

void EchoSender::triggerSendData(const QString echoStr, const int vp_width, const int vp_height)
{
    EchoSender::BoundingBoxGps box = calculateBoundingBox(vp_width, vp_height);
    double curRange = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toDouble();
    auto timestamp = QDateTime::currentMSecsSinceEpoch();
    QJsonDocument json(buildJsonPackage(echoStr, timestamp, box, curRange));

    saveJsonDataToFile(json.toJson());
    /*
    qDebug()<<Q_FUNC_INFO<<"base64"<<echoStr;
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
    */
}

QJsonObject EchoSender::buildJsonPackage(const QString data, const quint64 ts, const BoundingBoxGps box, double curRange)
{
    QJsonObject obj;

    obj["timestamp"] = static_cast<qint64>(ts);
    obj["range"] = curRange;
    obj["top_left_lat"] = box.topLeftLat;
    obj["top_left_lon"] = box.topLeftLon;
    obj["top_right_lat"] = box.topRightLat;
    obj["top_right_lon"] = box.topRightLon;
    obj["bottom_left_lat"] = box.bottomLeftLat;
    obj["bottom_left_lon"] = box.bottomLeftLon;
    obj["bottom_right_lat"] = box.bottomRightLat;
    obj["bottom_right_lon"] = box.bottomRightLon;
    obj["raw"] = data;

    return obj;
}

void EchoSender::saveJsonDataToFile(QByteArray data)
{
    QString filename = qApp->applicationDirPath()+QDir::separator()+"record.json";
    QFile file(filename);
    if(file.open(QIODevice::Append | QIODevice::WriteOnly))
    {
        file.write(data);
        file.write(",");
    } else qWarning()<<Q_FUNC_INFO<<"cannot open file"<<filename;
}

void EchoSender::initFile()
{
    QString filename = qApp->applicationDirPath()+QDir::separator()+"record.json";
    QFile file(filename);
    if(QFile::exists(filename))
    {
        QFile::remove(filename);
    } else qWarning()<<Q_FUNC_INFO<<"cannot open file"<<filename;

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
