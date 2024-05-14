#ifndef APPLICATIONCONFIG_H
#define APPLICATIONCONFIG_H

#include "arpa_config.h"
#include "gz_config.h"
#include "navigation_config.h"
#include "ppi_config.h"

class ApplicationConfig
{
public:    
    ApplicationConfig(ApplicationConfig &other) = delete;
    void operator=(const ApplicationConfig&) = delete;
    ~ApplicationConfig();

    static ApplicationConfig* getInstance();

    NavigationConfig *getNavConfig() const;
    ArpaConfig *getArpaConfig() const;
    GZConfig *getGzConfig() const;
    PPIConfig *getPpiConfig() const;

protected:
    ApplicationConfig();

private:
    static ApplicationConfig *config;
    NavigationConfig *navConfig;
    ArpaConfig *arpaConfig;
    GZConfig *gzConfig;
    PPIConfig *ppiConfig;
};

#endif // APPLICATIONCONFIG_H
