#include "gz_config.h"

#include <QDebug>
#include <QString>
#include <QFile>
#include <QSettings>

const QString GZ_ALARM_ENABLE = "gz/alarm/enable";
const QString GZ1_ALARM_ENABLE = "gz1/alarm/enable";
const QString GZ_NOTIF_THRESHOLD = "gz/notif/threshold";
const QString GZ1_NOTIF_THRESHOLD = "gz1/notif/threshold";

GZConfig* GZConfig::config = nullptr;

GZConfig::GZConfig()
{

}

GZConfig* GZConfig::getInstance(const QString path) {
    qDebug()<<Q_FUNC_INFO<<"path"<<path;

    if(config == nullptr) {
        if (!QFile::exists(path)) {
//            throw ErrFileNotFound();
            qFatal("file not found: %s", path.toUtf8().constData());
        }

        config = new GZConfig();
        config->setup(path);
    }

    return config;
}


void GZConfig::setup(const QString path)
{
    QSettings configFile(path,QSettings::IniFormat);

    enableAlarm = configFile.value(GZ_ALARM_ENABLE, true).toBool();
    enableAlarm1 = configFile.value(GZ1_ALARM_ENABLE, true).toBool();
    notifThreshold = configFile.value(GZ_NOTIF_THRESHOLD, 10).toUInt();
    notifThreshold1 = configFile.value(GZ1_NOTIF_THRESHOLD, 10).toUInt();
}

GZConfig::~GZConfig()
{

}
