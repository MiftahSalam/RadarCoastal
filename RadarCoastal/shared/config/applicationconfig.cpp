#include "applicationconfig.h"

#include <QDir>

const QString COMMON_CONFIG_PATH = QDir::homePath()+QDir::separator()+".hypernet"+QDir::separator()+"app.conf";

ApplicationConfig* ApplicationConfig::config = nullptr;

ApplicationConfig::ApplicationConfig()
{
    navConfig = NavigationConfig::getInstance(COMMON_CONFIG_PATH);
    arpaConfig = ArpaConfig::getInstance(COMMON_CONFIG_PATH);
    gzConfig = GZConfig::getInstance(COMMON_CONFIG_PATH);
    ppiConfig = PPIConfig::getInstance(COMMON_CONFIG_PATH);
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
