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

    bool getShowRings() const;
    void setShowRings(bool newShowRings);

    bool getShowEBL() const;
    void setShowEBL(bool newShowEBL);
    bool getShowVRM() const;
    void setShowVRM(bool newShowVRM);

    double getEblValue() const;
    double getVrmValue() const;

    void setEblValue(double newEblValue);
    void setVrmValue(double newVrmValue);

    bool getShowCompass() const;
    void setShowCompass(bool newShowCompass);

    bool getShowHM() const;
    void setShowHM(bool newShowHM);

    bool getShowSweep() const;
    void setShowSweep(bool newShowSweep);

    bool getShowOffCenter() const;
    void setShowOffCenter(bool newShowOffCenter);

protected:
    PPIConfig();

private:
    static PPIConfig *config;
    double eblValue;
    double vrmValue;
    bool showArpa;
    bool showCompass;
    bool showHM;
    bool showSweep;
    bool showRings;
    bool showEBL;
    bool showVRM;
    bool showOffCenter;
};

#endif // PPICONFIG_H
