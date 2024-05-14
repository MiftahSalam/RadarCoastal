#include "ppi_config.h"

#include <QDebug>
#include <QString>
#include <QFile>
#include <QSettings>

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

PPIConfig* PPIConfig::config = nullptr;

PPIConfig::PPIConfig()
    : BaseConfig()
{

}

bool PPIConfig::getShowOffCenter() const
{
    return showOffCenter;
}

void PPIConfig::setShowOffCenter(bool newShowOffCenter)
{
    showOffCenter = newShowOffCenter;
}

bool PPIConfig::getShowSweep() const
{
    return showSweep;
}

void PPIConfig::setShowSweep(bool newShowSweep)
{
    showSweep = newShowSweep;
}

bool PPIConfig::getShowHM() const
{
    return showHM;
}

void PPIConfig::setShowHM(bool newShowHM)
{
    showHM = newShowHM;
}

bool PPIConfig::getShowCompass() const
{
    return showCompass;
}

void PPIConfig::setShowCompass(bool newShowCompass)
{
    showCompass = newShowCompass;
}

void PPIConfig::setEblValue(double newEblValue)
{
    eblValue = newEblValue;
}

void PPIConfig::setVrmValue(double newVrmValue)
{
    vrmValue = newVrmValue;
}

double PPIConfig::getEblValue() const
{
    return eblValue;
}

double PPIConfig::getVrmValue() const
{
    return vrmValue;
}

bool PPIConfig::getShowEBL() const
{
    return showEBL;
}

void PPIConfig::setShowEBL(bool newShowEBL)
{
    showEBL = newShowEBL;
}

bool PPIConfig::getShowVRM() const
{
    return showVRM;
}

void PPIConfig::setShowVRM(bool newShowVRM)
{
    showVRM = newShowVRM;
}

bool PPIConfig::getShowRings() const
{
    return showRings;
}

void PPIConfig::setShowRings(bool newShowRings)
{
    showRings = newShowRings;
}

bool PPIConfig::getShowArpa() const
{
    return showArpa;
}

void PPIConfig::setShowArpa(bool newShowArpa)
{
    showArpa = newShowArpa;
}

PPIConfig::~PPIConfig()
{

}

PPIConfig* PPIConfig::getInstance(const QString path) {
    qDebug()<<Q_FUNC_INFO<<"path"<<path;

    if(config == nullptr) {
        if (!QFile::exists(path)) {
//            throw ErrFileNotFound();
            qFatal("file not found: %s", path.toUtf8().constData());
        }

        config = new PPIConfig();
        config->setup(path);
    }

    return config;
}


void PPIConfig::setup(const QString path)
{
    QSettings configFile(path,QSettings::IniFormat);

    showArpa = configFile.value(PPI_ARPA_SHOW, true).toBool();
    showRings = configFile.value(PPI_RINGS_SHOW, true).toBool();
    showEBL = configFile.value(PPI_EBL_SHOW, true).toBool();
    showVRM = configFile.value(PPI_VRM_SHOW, true).toBool();
    eblValue = configFile.value(PPI_EBL_VALUE, 0).toInt();
    vrmValue = configFile.value(PPI_VRM_VALUE, 10).toInt();
}


void PPIConfig::save(const QString path)
{
}
