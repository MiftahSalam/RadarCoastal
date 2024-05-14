#ifndef APPLICATIONCONFIG_H
#define APPLICATIONCONFIG_H

#include "arpa_config.h"
#include "gz_config.h"
#include "navigation_config.h"
#include "ppi_config.h"
#include "shared/config/echo_sender_config.h"

const QString APP_UNIT = "App/unit";
const QString APP_LANGUAGE = "App/language";
const QString APP_USE_OPENGL_SOFTWARE = "App/use_opengl_software";

class ApplicationConfig: public BaseConfig
{
public:    
    ApplicationConfig(ApplicationConfig &other) = delete;
    void operator=(const ApplicationConfig&) = delete;
    ~ApplicationConfig();

    static ApplicationConfig* getInstance();

    // BaseConfig interface
    void setup(const QString path) override;
    void save(const QString path) override;

    NavigationConfig *getNavConfig() const;
    ArpaConfig *getArpaConfig() const;
    GZConfig *getGzConfig() const;
    PPIConfig *getPpiConfig() const;
    EchoSenderConfig *getEchoConfig() const;

    quint8 getUnit() const;
    void setUnit(quint8 newUnit);

    QString getLanguage() const;
    bool getOpenGLSoftware() const;

protected:
    ApplicationConfig();

private:
    static ApplicationConfig *config;
    NavigationConfig *navConfig;
    ArpaConfig *arpaConfig;
    GZConfig *gzConfig;
    PPIConfig *ppiConfig;
    EchoSenderConfig *echoConfig;
    QString language;
    uint unit;
    bool openGLSoftware;
};

#endif // APPLICATIONCONFIG_H
