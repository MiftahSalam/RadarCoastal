#include "framecontrol3.h"
#include "ui_framecontrol3.h"
#include "shared/utils.h"

#include <RadarEngine/global.h>

#include <QMessageBox>
#include <QFile>

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, FrameControl3)
#else
#include <QDebug>
#endif

FrameControl3::FrameControl3(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameControl3)
{
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");

    ui->setupUi(this);
//    ui->label->hide();
    ui->label_2->hide();
//    ui->comboBoxMotion->hide();
    ui->comboBoxDisplayMode->hide();

    initConfig();
}

void FrameControl3::initConfig()
{
    ui->comboBoxMotion->setCurrentIndex(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_HEADING_UP).toBool() ? 1 : 0);
    ui->checkBoxShowCompass->setChecked(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_COMPASS).toBool());
    ui->checkBoxShowHM->setChecked(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_HEADING_MARKER).toBool());
    ui->checkBoxShowRSweep->setChecked(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_SWEEP).toBool());

    int cur_idx = ui->comboBoxDisplayUnit->currentIndex();
    int cfg_idx = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_UNIT).toInt();

    prev_unit_idx = cfg_idx;

    if(cur_idx == cfg_idx) on_comboBoxDisplayUnit_currentIndexChanged(cur_idx);
    else ui->comboBoxDisplayUnit->setCurrentIndex(cfg_idx);

}

FrameControl3::~FrameControl3()
{
    delete ui;
}

void FrameControl3::on_comboBoxMotion_currentIndexChanged(int index)
{
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_HEADING_UP,index>0 ? true : false);
}

void FrameControl3::on_checkBoxShowCompass_clicked(bool checked)
{
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_COMPASS,checked);
}

void FrameControl3::on_checkBoxShowHM_clicked(bool checked)
{
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_HEADING_MARKER,checked);
}

void FrameControl3::on_checkBoxShowRSweep_clicked(bool checked)
{
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_SWEEP,checked);
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
#ifdef USE_LOG4QT
        logger()->trace()<<Q_FUNC_INFO<< "loading stylesheet file...";
#else
    qDebug()<<Q_FUNC_INFO<< "loading stylesheet file...";
#endif
        style = QLatin1String( styleFile.readAll() );
        qApp->setStyleSheet(style);
    }
    else
    {
        qWarning() << ":/css/HMI_style_night.css" << ": css file not found!";
    }

    m_instance_cfg->setConfig(RadarEngine::VOLATILE_DISPLAY_PRESET_COLOR,index);
}

void FrameControl3::on_comboBoxDisplayUnit_currentIndexChanged(int index)
{
    Utils::unit = static_cast<quint8>(index);
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_UNIT,index);

    const int cur_range = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toInt();
    int cur_zoom_lvl = distanceList.indexOf(cur_range);

#ifndef USE_LOG4QT
    qDebug()<<Q_FUNC_INFO<<"distanceList"<<distanceList;
    qDebug()<<Q_FUNC_INFO<<"distanceListNautical"<<distanceListNautical;
#endif

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
        m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE, distanceList.at(cur_zoom_lvl));
        break;
    case 1:
        m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE, distanceListNautical.at(cur_zoom_lvl));
        break;
    default:
        break;
    }

    prev_unit_idx = index;
}

