#ifndef NAVIGATIONCONFIG_H
#define NAVIGATIONCONFIG_H

#include <QString>

#include "base_config.h"

class NavigationConfig: public BaseConfig
{
public:    
    NavigationConfig(NavigationConfig &other) = delete;
    void operator=(const NavigationConfig&) = delete;
    ~NavigationConfig();

    static NavigationConfig* getInstance(const QString path);

    quint8 getGPSStatus() const;
    bool getGPSModeAuto() const;

protected:
    NavigationConfig();

private:
    static NavigationConfig *config;
    quint8 gpsStatus;
    bool gpsModeAuto;

    // BaseConfig interface
public:
    void setup(const QString path) override;
};

#endif // NAVIGATIONCONFIG_H
