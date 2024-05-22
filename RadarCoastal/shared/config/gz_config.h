#ifndef GZCONFIG_H
#define GZCONFIG_H

#include "base_config.h"

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

protected:
    GZConfig();

private:
    static GZConfig *config;
    bool enableAlarm;
    bool enableAlarm1;
    quint16 notifThreshold;
    quint16 notifThreshold1;

};

#endif // GZCONFIG_H
