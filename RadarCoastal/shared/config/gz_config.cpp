#include "gz_config.h"

#include <QDebug>
#include <QString>
#include <QFile>
#include <QSettings>
#include <QDateTime>

const QString GZ_ALARM_ENABLE = "GZ/alarm/enable";
const QString GZ1_ALARM_ENABLE = "GZ1/alarm/enable";
const QString GZ_NOTIF_THRESHOLD = "GZ/notif/threshold";
const QString GZ1_NOTIF_THRESHOLD = "GZ1/notif/threshold";
const QString GZ_TIMEOUT = "GZ/timeout";
const QString GZ1_TIMEOUT = "GZ1/timeout";

GZConfig* GZConfig::config = nullptr;

GZConfig::GZConfig()
    : BaseConfig()
{

}

int GZConfig::getTimeout() const
{
    return timeout;
}

void GZConfig::setTimeout(int newTimeout)
{
    timeout = newTimeout;
}

int GZConfig::getTimeout1() const
{
    return timeout1;
}

void GZConfig::setTimeout1(int newTimeout1)
{
    timeout1 = newTimeout1;
}

bool GZConfig::getConfirmed() const
{
    return confirmed;
}

void GZConfig::setConfirmed(bool newConfirmed)
{
    confirmed = newConfirmed;
}

bool GZConfig::getConfirmed1() const
{
    return confirmed1;
}

void GZConfig::setConfirmed1(bool newConfirmed1)
{
    confirmed1 = newConfirmed1;
}

int GZConfig::getTime() const
{
    return time;
}

void GZConfig::setTime(int newTime)
{
    time = newTime;
}

int GZConfig::getTime1() const
{
    return time1;
}

void GZConfig::setTime1(int newTime1)
{
    time1 = newTime1;
}

bool GZConfig::getEnableAlarm() const
{
    return enableAlarm;
}

void GZConfig::setEnableAlarm(bool newEnableAlarm)
{
    enableAlarm = newEnableAlarm;
}

bool GZConfig::getEnableAlarm1() const
{
    return enableAlarm1;
}

void GZConfig::setEnableAlarm1(bool newEnableAlarm1)
{
    enableAlarm1 = newEnableAlarm1;
}

quint16 GZConfig::getNotifThreshold() const
{
    return notifThreshold;
}

void GZConfig::setNotifThreshold(quint16 newNotifThreshold)
{
    notifThreshold = newNotifThreshold;
}

quint16 GZConfig::getNotifThreshold1() const
{
    return notifThreshold1;
}

void GZConfig::setNotifThreshold1(quint16 newNotifThreshold1)
{
    notifThreshold1 = newNotifThreshold1;
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
    timeout = configFile.value(GZ_TIMEOUT, 90).toUInt();
    timeout1 = configFile.value(GZ1_TIMEOUT, 90).toUInt();
    time = QDateTime::currentSecsSinceEpoch();
    time1 = time;
}

void GZConfig::save(const QString path)
{

}

GZConfig::~GZConfig()
{

}
