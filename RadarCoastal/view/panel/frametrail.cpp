#include "frametrail.h"
#include "ui_frametrail.h"

#include <radarconfig.h>

FrameTrail::FrameTrail(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameTrail)
{
    ui->setupUi(this);
    m_re = RadarEngine::RadarEngine::getInstance();

    RadarConfig::RadarConfig* instance = RadarConfig::RadarConfig::getInstance("");

    int curInx = instance->getConfig(RadarConfig::NON_VOLATILE_RADAR_TRAIL_TIME).toInt();
    ui->comboBoxTargetTail->setCurrentIndex(curInx);
    ui->checkBoxTrailOff->setChecked(instance->getConfig(RadarConfig::NON_VOLATILE_RADAR_TRAIL_ENABLE).toBool());
}

FrameTrail::~FrameTrail()
{
    delete ui;
}

void FrameTrail::on_checkBoxTrailOff_clicked(bool checked)
{
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_RADAR_TRAIL_ENABLE,checked);
}

void FrameTrail::on_pushButtonClearTrail_clicked()
{
    m_re->trigger_clearTrail();
}


void FrameTrail::on_comboBoxTargetTail_currentIndexChanged(int index)
{
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_RADAR_TRAIL_TIME,index);
}

