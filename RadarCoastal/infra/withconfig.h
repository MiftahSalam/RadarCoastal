#ifndef WITHCONFIG_H
#define WITHCONFIG_H

#include <RadarEngine/radarconfig.h>

class WithConfig
{
public:
    WithConfig(RadarEngine::RadarConfig* cfg, QString caller);

    virtual void foo() {} // polymorphic

    static RadarEngine::RadarConfig* m_instance_cfg;

    void initConfig(){}
};

#endif // WITHCONFIG_H
