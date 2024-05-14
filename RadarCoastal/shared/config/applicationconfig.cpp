#include "applicationconfig.h"
#include "qsettings.h"

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
    changeConfig(APP_UNIT, newUnit);
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

EchoSenderConfig *ApplicationConfig::getEchoConfig() const
{
    return echoConfig;
}

ApplicationConfig* ApplicationConfig::getInstance() {
    if(config == nullptr) {
        config = new ApplicationConfig();
        config->setup(COMMON_CONFIG_PATH);
    }

    return config;
}


void ApplicationConfig::setup(const QString path)
{
    navConfig = NavigationConfig::getInstance(path);
    arpaConfig = ArpaConfig::getInstance(path);
    gzConfig = GZConfig::getInstance(path);
    ppiConfig = PPIConfig::getInstance(path);
    echoConfig = EchoSenderConfig::getInstance(path);

    QSettings configFile(path, QSettings::IniFormat);

    unit = configFile.value(APP_UNIT, 0).toInt();
    language = configFile.value(APP_LANGUAGE, "en").toString();
    openGLSoftware = configFile.value(APP_USE_OPENGL_SOFTWARE, false).toBool();
}

void ApplicationConfig::save(const QString path)
{
}

ApplicationConfig::~ApplicationConfig()
{

}
