#include "withconfig.h"

RadarEngine::RadarConfig* WithConfig::m_instance_cfg{nullptr};

WithConfig::WithConfig(RadarEngine::RadarConfig *cfg, QString caller)
{
    if (cfg == nullptr) {
        qDebug()<<Q_FUNC_INFO<<"RadarConfig cannot be null. caller "<<caller;
        exit(1);
    }

    if(m_instance_cfg == nullptr) m_instance_cfg = RadarEngine::RadarConfig::getInstance("");
}
