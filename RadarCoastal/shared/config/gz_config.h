#ifndef GZCONFIG_H
#define GZCONFIG_H

#include "base_config.h"

const QString GZ_ALARM_ENABLE = "GZ/alarm/enable";
const QString GZ1_ALARM_ENABLE = "GZ1/alarm/enable";
const QString GZ_NOTIF_THRESHOLD = "GZ/notif/threshold";
const QString GZ1_NOTIF_THRESHOLD = "GZ1/notif/threshold";
const QString GZ_TIMEOUT = "GZ/timeout";
const QString GZ1_TIMEOUT = "GZ1/timeout";

class GZConfig: public BaseConfig
{
public:
    GZConfig(GZConfig &other) = delete;
    void operator=(const GZConfig&) = delete;
    ~GZConfig();

    static GZConfig* getInstance(const QString path);

    // BaseConfig interface
    void setup(const QString path) override;
    void save(const QString path) override;

    bool getEnableAlarm() const;
    void setEnableAlarm(bool newEnableAlarm);
    bool getEnableAlarm1() const;
    void setEnableAlarm1(bool newEnableAlarm1);
    quint16 getNotifThreshold() const;
    void setNotifThreshold(quint16 newNotifThreshold);
    quint16 getNotifThreshold1() const;
    void setNotifThreshold1(quint16 newNotifThreshold1);

    bool getConfirmed() const;
    void setConfirmed(bool newConfirmed);
    bool getConfirmed1() const;
    void setConfirmed1(bool newConfirmed1);
    int getTime() const;
    void setTime(int newTime);
    int getTime1() const;
    void setTime1(int newTime1);

    int getTimeout() const;
    void setTimeout(int newTimeout);
    int getTimeout1() const;
    void setTimeout1(int newTimeout1);

protected:
    GZConfig();

private:
    static GZConfig *config;
    bool enableAlarm;
    bool enableAlarm1;
    bool confirmed;
    bool confirmed1;
    int time;
    int time1;
    int timeout;
    int timeout1;
    quint16 notifThreshold;
    quint16 notifThreshold1;

};

#endif // GZCONFIG_H
