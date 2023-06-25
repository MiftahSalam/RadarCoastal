#include "framecontrol3.h"
#include "ui_framecontrol3.h"
#include "utils.h"

#include <radarconfig.h>
#include <radarengine_global.h>

#include <QMessageBox>
#include <QFile>

FrameControl3::FrameControl3(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameControl3)
{
    ui->setupUi(this);
//    ui->label->hide();
    ui->label_2->hide();
//    ui->comboBoxMotion->hide();
    ui->comboBoxDisplayMode->hide();

    ui->comboBoxMotion->setCurrentIndex(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_HEADING_UP).toBool() ? 1 : 0);
    ui->checkBoxShowCompass->setChecked(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_COMPASS).toBool());
    ui->checkBoxShowHM->setChecked(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_HEADING_MARKER).toBool());
    ui->checkBoxShowRSweep->setChecked(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_SWEEP).toBool());
    ui->comboBoxDisplayUnit->setCurrentIndex(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_UNIT).toInt());

    prev_unit_idx = ui->comboBoxDisplayMode->currentIndex();
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


void FrameControl3::on_checkBoxShowRSweep_clicked(bool checked)
{
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_SWEEP,checked);
}


void FrameControl3::on_comboBoxDisplayMode_currentIndexChanged(int index)
{
    /*change style*/
    QFile styleFile;
    QString style;

    switch (index) {
    case 0:
        styleFile.setFileName( ":/css/HMI_style.css" );
        break;
    case 1:
        styleFile.setFileName( ":/css/HMI_style_night.css" );
        break;
    default:
        styleFile.setFileName( ":/css/HMI_style.css" );
        break;
    }

    if( styleFile.open( QFile::ReadOnly ) )
    {
        qDebug() << "loading stylesheet file...";
        style = QLatin1String( styleFile.readAll() );
        qApp->setStyleSheet(style);
    }
    else
    {
        qWarning() << ":/css/HMI_style_night.css" << ": css file not found!";
    }

    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_DISPLAY_PRESET_COLOR,index);
}


void FrameControl3::on_comboBoxDisplayUnit_currentIndexChanged(int index)
{
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_UNIT,index);
    unit = static_cast<quint8>(index);

    const int cur_range = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toInt();
    int cur_zoom_lvl = RadarEngine::distanceList.indexOf(cur_range);

    switch (prev_unit_idx) {
    case 0:
        cur_zoom_lvl = RadarEngine::distanceList.indexOf(cur_range);
        break;
    case 1:
        cur_zoom_lvl = RadarEngine::distanceListNautical.indexOf(cur_range);
        break;
    default:
        break;
    }

    if(cur_zoom_lvl < 0) cur_zoom_lvl = 0;

    switch (index) {
    case 0:
        RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE, RadarEngine::distanceList.at(cur_zoom_lvl));
        break;
    case 1:
        RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE, RadarEngine::distanceListNautical.at(cur_zoom_lvl));
        break;
    default:
        break;
    }

    prev_unit_idx = index;
}

