#include "applicationconfig.h"
#include "qsettings.h"

#include <QDir>
#include <QDebug>

const QString COMMON_CONFIG_PATH = QDir::homePath()+QDir::separator()+".hypernet"+QDir::separator()+"app.conf";

ApplicationConfig* ApplicationConfig::config = nullptr;

ApplicationConfig::ApplicationConfig()
    : BaseConfig()
{
}

void ApplicationConfig::checkConfig()
{
    QSettings config(COMMON_CONFIG_PATH, QSettings::IniFormat);

    QStringList config_keys;
    QStringList desired_config_keys =
            QStringList()<<APP_UNIT
                        <<APP_LANGUAGE
                       <<APP_USE_OPENGL_SOFTWARE
                      <<ARPA_NET_SPASI
                     <<ARPA_NET_INTERNAL
                    <<ARPA_CREATE_ARPA_BY_CLICK
                   <<ECHO_SPASI_NET
                  <<GZ_ALARM_ENABLE
                 <<GZ1_ALARM_ENABLE
                <<GZ_NOTIF_THRESHOLD
               <<GZ1_NOTIF_THRESHOLD
              <<GZ_TIMEOUT
             <<GZ1_TIMEOUT
            <<NAV_MODE_GPS
           <<NAV_MODE_HEADING
        <<NAV_INTERNAL_NET
       <<NAV_SPASI_NET
       <<PPI_ARPA_SHOW
      <<PPI_COMPASS_SHOW
     <<PPI_HM_SHOW
    <<PPI_SWEEP_SHOW
    <<PPI_RINGS_SHOW
    <<PPI_EBL_SHOW
    <<PPI_VRM_SHOW
    <<PPI_EBL_VALUE
    <<PPI_VRM_VALUE
    <<PPI_OFFCENTER_ENABLE
      ;

    foreach (QString key, config.allKeys())
    {
        config_keys.append(key);
    }

    /*match desired keys*/
    int i = 0;
    while (i < desired_config_keys.size())
    {
        if (!config_keys.contains(desired_config_keys.at(i)))
            qWarning() << Q_FUNC_INFO << " undesired key: " << desired_config_keys.at(i);
        else
            config_keys.removeOne(desired_config_keys.at(i));
        i++;
    }
    foreach (QString key, config_keys)
    {
        qWarning() << Q_FUNC_INFO << " key not found: " << key;
    }

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

    checkConfig();
}

void ApplicationConfig::save(const QString path)
{
}

ApplicationConfig::~ApplicationConfig()
{

}
