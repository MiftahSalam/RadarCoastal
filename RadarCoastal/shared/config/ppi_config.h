#ifndef PPICONFIG_H
#define PPICONFIG_H

#include "base_config.h"

class PPIConfig: public BaseConfig
{
public:    
    PPIConfig(PPIConfig &other) = delete;
    void operator=(const PPIConfig&) = delete;
    ~PPIConfig();

    static PPIConfig* getInstance(const QString path);

    // BaseConfig interface
    void setup(const QString path) override;
    void save(const QString path) override;

    bool getShowArpa() const;
    void setShowArpa(bool newShowArpa);

protected:
    PPIConfig();

private:
    static PPIConfig *config;
    bool showArpa;
};

#endif // PPICONFIG_H
