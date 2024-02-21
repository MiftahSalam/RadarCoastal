#include <qmath.h>

#include "shared/utils.h"
#include "usecase/ppi/arpa.h"

PPIArpa::PPIArpa(QObject *parent, RadarEngine::RadarEngine *re, RadarEngine::RadarConfig *config): QObject(parent)
{
    if (re == nullptr) {
        qDebug()<<Q_FUNC_INFO<<"RadarEngine cannot be null";
        exit(1);
    }
    if (config == nullptr) {
        qDebug()<<Q_FUNC_INFO<<"RadarConfig cannot be null";
        exit(1);
    }

    m_re = re;
    m_config_instance = config;
}

void PPIArpa::createMARPA(const QPoint& pos, const int vp_width, const int vp_height)
{
#ifdef DISPLAY_ONLY_MODE
    return;
#endif
    qDebug()<<Q_FUNC_INFO<<pos;

    if(!m_config_instance->getConfig(RadarEngine::NON_VOLATILE_ARPA_CONTROL_CREATE_ARPA_BY_CLICK).toBool()) return;

    double const curLat = m_config_instance->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
    double const curLon = m_config_instance->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();
    double curRange = m_config_instance->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toDouble();
//    const quint8 unit = static_cast<quint8>(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_UNIT).toUInt());

//    switch (unit) {
//    case 1:
//        curRange *= KM_TO_NM;
//        break;
//    default:
//        break;
//    }

    QPoint screen_middle(vp_width/2,vp_height/2);
    QPointF gps = Utils::PixToGPS(pos.x()-screen_middle.x(),-pos.y()+screen_middle.y(),vp_width,vp_height,curRange,curLat,curLon);

    RadarEngine::Position target_pos;
    target_pos.lat = gps.y();
    target_pos.lon = gps.x();

    qDebug()<<Q_FUNC_INFO<<target_pos.lat<<target_pos.lon;

    m_re->radarArpa->AcquireNewMARPATarget(target_pos);
}
