#include "withconfig.h"

WithConfig::WithConfig(RadarEngine::RadarConfig *cfg): m_instance_cfg(cfg)
{
    if (cfg == nullptr) {
        qDebug()<<Q_FUNC_INFO<<"RadarConfig cannot be null";
        exit(1);
    }
}
