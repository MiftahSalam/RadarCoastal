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

    quint8 getGPSStatus() const;
    void setGpsStatus(quint8 newGpsStatus);

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
