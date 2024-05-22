#include "applicationconfig.h"

#include <QDir>

const QString COMMON_CONFIG_PATH = QDir::homePath()+QDir::separator()+".hypernet"+QDir::separator()+"app.conf";

ApplicationConfig* ApplicationConfig::config = nullptr;

ApplicationConfig::ApplicationConfig()
    : BaseConfig()
{
}

bool ApplicationConfig::getOpenGLSoftware() const
{
    return openGLSoftware;
}

QString ApplicationConfig::getLanguage() const
{
    return language;
}

quint8 ApplicationConfig::getUnit() const
{
    return unit;
}

void ApplicationConfig::setUnit(quint8 newUnit)
{
    unit = newUnit;
}

NavigationConfig *ApplicationConfig::getNavConfig() const
{
    return navConfig;
}

ArpaConfig *ApplicationConfig::getArpaConfig() const
{
    return arpaConfig;
}

GZConfig *ApplicationConfig::getGzConfig() const
{
    return gzConfig;
}

PPIConfig *ApplicationConfig::getPpiConfig() const
{
    return ppiConfig;
}

ApplicationConfig* ApplicationConfig::getInstance() {
    if(config == nullptr) {
        config = new ApplicationConfig();
    }

    return config;
}
