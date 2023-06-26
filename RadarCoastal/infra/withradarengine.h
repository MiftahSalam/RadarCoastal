#ifndef WITHRADARENGINE_H
#define WITHRADARENGINE_H

#include <RadarEngine/radarengine.h>

class WithRadarEngine
{
public:
    WithRadarEngine(RadarEngine::RadarEngine *re);
    RadarEngine::RadarEngine* m_instance_re;
};

#endif // WITHRADARENGINE_H
