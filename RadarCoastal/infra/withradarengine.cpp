#include "withradarengine.h"

RadarEngine::RadarEngine* WithRadarEngine::m_instance_re{nullptr};

WithRadarEngine::WithRadarEngine(RadarEngine::RadarEngine *re, QString caller)
{
    if (re == nullptr) {
        qDebug()<<Q_FUNC_INFO<<"RadarEngine cannot be null. caller "<<caller;
        exit(1);
    }

    if(m_instance_re == nullptr) m_instance_re = re;

}
