#ifndef APPLICATIONCONFIG_H
#define APPLICATIONCONFIG_H

#include "arpa_config.h"
#include "gz_config.h"
#include "navigation_config.h"
#include "ppi_config.h"
#ifdef DISPLAY_ONLY_MODE
#include "hear_beat_radar_config.h"
#endif

#include <QDir>

const QString COMMON_CONFIG_PATH = QDir::homePath()+QDir::separator()+".pjs"+QDir::separator()+"app.conf";

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

    quint8 getUnit() const;
    void setUnit(quint8 newUnit);

    QString getLanguage() const;
    bool getOpenGLSoftware() const;

#ifdef DISPLAY_ONLY_MODE
    HearBeatRadarConfig *getHbConfig() const;
#endif

protected:
    ApplicationConfig();

private:
    static ApplicationConfig *config;
    NavigationConfig *navConfig;
#ifdef DISPLAY_ONLY_MODE
    HearBeatRadarConfig *hbConfig;
#endif
    ArpaConfig *arpaConfig;
    GZConfig *gzConfig;
    PPIConfig *ppiConfig;
    QString language;
    uint unit;
    bool openGLSoftware;

    void checkConfig();
};

#endif // APPLICATIONCONFIG_H
