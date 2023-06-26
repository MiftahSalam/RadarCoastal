#ifndef WITHRADARENGINE_H
#define WITHRADARENGINE_H

#include <RadarEngine/radarengine.h>

class WithRadarEngine
{
public:
    WithRadarEngine(RadarEngine::RadarEngine *re, QString caller);
    static RadarEngine::RadarEngine* m_instance_re;

    virtual void foo() {} // polymorphic
};

#endif // WITHRADARENGINE_H
