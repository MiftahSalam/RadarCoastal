#include "applicationconfig.h"

#include <QDir>

const QString COMMON_CONFIG_PATH = QDir::homePath()+QDir::separator()+".hypernet"+QDir::separator()+"app.conf";

ApplicationConfig::ApplicationConfig()
{
    navConfig = NavigationConfig::getInstance(COMMON_CONFIG_PATH);
}

NavigationConfig *ApplicationConfig::getNavConfig() const
{
    return navConfig;
}
