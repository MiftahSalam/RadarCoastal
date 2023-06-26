#include "withradarengine.h"

WithRadarEngine::WithRadarEngine(RadarEngine::RadarEngine *re): m_instance_re(re)
{
    if (re == nullptr) {
        qDebug()<<Q_FUNC_INFO<<"RadarEngine cannot be null";
        exit(1);
    }
}
