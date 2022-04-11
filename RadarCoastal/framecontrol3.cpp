#include "framecontrol3.h"
#include "ui_framecontrol3.h"

#include <radarconfig.h>
#include <radarengine_global.h>

#include <QMessageBox>

FrameControl3::FrameControl3(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameControl3)
{
    ui->setupUi(this);

    ui->comboBoxMotion->setCurrentIndex(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_HEADING_UP).toBool() ? 1 : 0);
    ui->checkBoxShowCompass->setChecked(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_COMPASS).toBool());
    ui->checkBoxShowHM->setChecked(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_HEADING_MARKER).toBool());
}

FrameControl3::~FrameControl3()
{
    delete ui;
}

void FrameControl3::on_comboBoxMotion_currentIndexChanged(int index)
{
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_HEADING_UP,index>0 ? true : false);
//    radar_settings.headingUp = index>0 ? true : false;
}

void FrameControl3::on_checkBoxShowCompass_clicked(bool checked)
{
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_COMPASS,checked);
//    radar_settings.show_compass = checked;
}

void FrameControl3::on_checkBoxShowHM_clicked(bool checked)
{
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_HEADING_MARKER,checked);
//    radar_settings.show_heading_marker = checked;
}
