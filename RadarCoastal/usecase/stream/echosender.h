#ifndef ECHOSENDER_H
#define ECHOSENDER_H

#include <QObject>

#include <RadarEngine/radarconfig.h>
#include <RadarEngine/radarengine.h>
#include <RadarEngine/radarimagecapture.h>

class EchoSender : public QObject
{
    Q_OBJECT
public:
    explicit EchoSender(QObject *parent = nullptr);

    RadarEngine::RadarImageCapture *m_ppi_grabber;
    RadarEngine::RadarEngine* m_re;

signals:

private slots:
    void triggerSendData(const QString echoStr, const int vp_width, const int vp_height);

private:
    RadarEngine::RadarConfig* m_instance_cfg;

    struct BoundingBoxGps
    {
        BoundingBoxGps() {}

        double topLeftLat;
        double topLeftLon;
        double bottomLeftLat;
        double bottomLeftLon;
        double topRightLat;
        double topRightLon;
        double bottomRightLat;
        double bottomRightLon;
    };

    BoundingBoxGps calculateBoundingBox(const int vp_width, const int vp_height);
    QJsonObject buildJsonPackage(const QString data, const quint64 ts, const BoundingBoxGps box, double curRange);
    void saveJsonDataToFile(QByteArray data);
    void initFile();
};

#endif // ECHOSENDER_H
