#ifndef WITHCONFIG_H
#define WITHCONFIG_H

#include <RadarEngine/radarconfig.h>

class WithConfig
{
public:
    WithConfig(RadarEngine::RadarConfig* cfg = nullptr);

protected:
    RadarEngine::RadarConfig* m_instance_cfg;

    void initConfig(){}
};

#endif // WITHCONFIG_H
