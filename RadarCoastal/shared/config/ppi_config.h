#ifndef PPICONFIG_H
#define PPICONFIG_H

#include "base_config.h"

const QString PPI_ARPA_SHOW = "PPI/Display/arpa/show";
const QString PPI_COMPASS_SHOW = "PPI/Display/compass/show";
const QString PPI_HM_SHOW = "PPI/Display/heading_marker/show";
const QString PPI_SWEEP_SHOW = "PPI/Display/sweep/show";
const QString PPI_RINGS_SHOW = "PPI/Display/rings/show";
const QString PPI_EBL_SHOW = "PPI/Display/ebl/show";
const QString PPI_VRM_SHOW = "PPI/Display/vrm/show";
const QString PPI_EBL_VALUE = "PPI/Display/ebl/value";
const QString PPI_VRM_VALUE = "PPI/Display/vrm/value";
const QString PPI_OFFCENTER_ENABLE= "PPI/Display/offcenter/enable";

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
