#include <QDebug>
#include <QString>
#include <QFile>
#include <QSettings>

#include "navigation_config.h"
//#include "src/shared/common/errors/err_open_file.h"

const QString NAV_MODE_GPS = "nav/mode/gps";

NavigationConfig* NavigationConfig::config = nullptr;

NavigationConfig::NavigationConfig()
{

}

NavigationConfig::~NavigationConfig()
{

}

quint8 NavigationConfig::getGPSStatus() const
{
    return gpsStatus;
}

bool NavigationConfig::getGPSModeAuto() const
{
    return gpsModeAuto;
}

NavigationConfig* NavigationConfig::getInstance(const QString path) {
    qDebug()<<Q_FUNC_INFO<<"path"<<path;

    if(config == nullptr) {
        if (!QFile::exists(path)) {
//            throw ErrFileNotFound();
            qFatal("file not found: %s", path.toUtf8().constData());
        }

        config = new NavigationConfig();
        config->setup(path);
    }

    return config;
}


void NavigationConfig::setup(const QString path)
{
    QSettings configFile(path,QSettings::IniFormat);

    gpsStatus = 0; //offline
    gpsModeAuto = configFile.value(NAV_MODE_GPS, false).toBool();
}
