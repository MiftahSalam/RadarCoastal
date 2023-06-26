#include "framecontrol3.h"
#include "ui_framecontrol3.h"
#include "shared/utils.h"

#include <RadarEngine/radarconfig.h>
#include <RadarEngine/shared/global.h>

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

    ui->comboBoxMotion->setCurrentIndex(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_HEADING_UP).toBool() ? 1 : 0);
    ui->checkBoxShowCompass->setChecked(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_COMPASS).toBool());
    ui->checkBoxShowHM->setChecked(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_HEADING_MARKER).toBool());
    ui->checkBoxShowRSweep->setChecked(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_SWEEP).toBool());
    ui->comboBoxDisplayUnit->setCurrentIndex(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_UNIT).toInt());

    prev_unit_idx = ui->comboBoxDisplayMode->currentIndex();
}

FrameControl3::~FrameControl3()
{
    delete ui;
}

void FrameControl3::on_comboBoxMotion_currentIndexChanged(int index)
{
    RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_HEADING_UP,index>0 ? true : false);
//    radar_settings.headingUp = index>0 ? true : false;
}

void FrameControl3::on_checkBoxShowCompass_clicked(bool checked)
{
    RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_COMPASS,checked);
//    radar_settings.show_compass = checked;
}

void FrameControl3::on_checkBoxShowHM_clicked(bool checked)
{
    RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_HEADING_MARKER,checked);
//    radar_settings.show_heading_marker = checked;
}


void FrameControl3::on_checkBoxShowRSweep_clicked(bool checked)
{
    RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_SWEEP,checked);
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

    RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::VOLATILE_DISPLAY_PRESET_COLOR,index);
}


void FrameControl3::on_comboBoxDisplayUnit_currentIndexChanged(int index)
{
    RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_UNIT,index);
    Utils::unit = static_cast<quint8>(index);

    const int cur_range = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toInt();
    int cur_zoom_lvl = distanceList.indexOf(cur_range);

    switch (prev_unit_idx) {
    case 0:
        cur_zoom_lvl = distanceList.indexOf(cur_range);
        break;
    case 1:
        cur_zoom_lvl = distanceListNautical.indexOf(cur_range);
        break;
    default:
        break;
    }

    if(cur_zoom_lvl < 0) cur_zoom_lvl = 0;

    switch (index) {
    case 0:
        RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE, distanceList.at(cur_zoom_lvl));
        break;
    case 1:
        RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE, distanceListNautical.at(cur_zoom_lvl));
        break;
    default:
        break;
    }

    prev_unit_idx = index;
}

