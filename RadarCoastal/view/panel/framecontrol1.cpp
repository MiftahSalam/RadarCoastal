#include "shared/utils.h"
#include "framecontrol1.h"
#include "ui_framecontrol1.h"

#include <RadarEngine/shared/constants.h>
#include <RadarEngine/shared/global.h>
#include <RadarEngine/radarconfig.h>

#include <unistd.h>


FrameControl1::FrameControl1(QWidget *parent) :
    QFrame(parent), ui(new Ui::FrameControl1)
{
    ui->setupUi(this);
    m_re = RadarEngine::RadarEngine::GetInstance();

    RadarEngine::RadarConfig* rcInstance = RadarEngine::RadarConfig::getInstance("");

    ui->checkBoxShowRing->setChecked(rcInstance->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_RING).toBool());
    on_checkBoxShowRing_clicked(rcInstance->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_RING).toBool());
    handleRingRangeChange();
    stateChange(RadarEngine::RADAR_OFF);

    connect(rcInstance,&RadarEngine::RadarConfig::configValueChange,this,&FrameControl1::trigger_radarConfigChange);
}

void FrameControl1::handleRingRangeChange()
{
    double rng = static_cast<double>(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toInt())/1000.;
    double rng_ring = rng/RING_COUNT;
    QString unit_str;

    switch (Utils::unit) {
    case 0:
        unit_str = " Km";
        break;
    case 1:
        unit_str = " NM";
        break;
    default:
        break;
    }

    if(rng < 1)
    {
        ui->labelRange->setText(QString::number(static_cast<int>(rng*1000.))+" m");
    }
    else
    {
        switch (Utils::unit) {
        case 1:
            rng *= KM_TO_NM;
            break;
        default:
            break;
        }
        ui->labelRange->setText(QString::number(rng,'f',1)+unit_str);
    }

    if(rng_ring < 1)
    {
        ringValue = QString::number(static_cast<int>(rng_ring*1000.))+" m";
    }
    else
    {
        switch (Utils::unit) {
        case 1:
            rng_ring *= KM_TO_NM;
            break;
        default:
            break;
        }
        ringValue = QString::number(rng_ring,'f',1)+unit_str;
    }

    if(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_RING).toBool())
        ui->labelRingRange->setText("Rings "+ringValue);
}

void FrameControl1::trigger_radarConfigChange(QString key, QVariant val)
{
    if(key == RadarEngine::VOLATILE_RADAR_STATUS) stateChange(val.toInt());
    else if(key == RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE) handleRingRangeChange();
}

void FrameControl1::stateChange(int state)
{
    if(static_cast<RadarEngine::RadarState>(state) == RadarEngine::RADAR_STANDBY)
    {
        ui->pushButtonTxStnb->setEnabled(true);
        ui->pushButtonTxStnb->setText("Transmit");
    }
    else if((static_cast<RadarEngine::RadarState>(state) == RadarEngine::RADAR_TRANSMIT &&
            static_cast<RadarEngine::RadarState>(state) == RadarEngine::RADAR_NO_SPOKE) ||
            static_cast<RadarEngine::RadarState>(state) == RadarEngine::RADAR_TRANSMIT)
    {
        ui->pushButtonTxStnb->setEnabled(true);
        ui->pushButtonTxStnb->setText("Standby");

        //comment for radar offline mode
        /*
        const uint cur_range = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::VOLATILE_RADAR_PARAMS_RANGE_DATA_RANGE).toUInt();
        const uint cur_scale = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toUInt();
        if(cur_scale != cur_range) m_re->trigger_ReqRangeChange(static_cast<int>(cur_scale));
        */
    }
    else
    {
        ui->pushButtonTxStnb->setEnabled(false);
        ui->pushButtonTxStnb->setText("Tx/Stby");
    }
}

FrameControl1::~FrameControl1()
{
    delete ui;
}

void FrameControl1::on_pushButtonZoomIn_clicked()
{
    const int cur_range = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toInt();
    int cur_zoom_lvl = distanceList.indexOf(cur_range)+1;

    switch (Utils::unit) {
    case 0:
        cur_zoom_lvl = distanceList.indexOf(cur_range)+1;
        if(cur_zoom_lvl > distanceList.size()-1) cur_zoom_lvl -=1;
        RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE, distanceList.at(cur_zoom_lvl));
        break;
    case 1:
        cur_zoom_lvl = distanceListNautical.indexOf(cur_range)+1;
        if(cur_zoom_lvl > distanceListNautical.size()-1) cur_zoom_lvl -=1;
        RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE, distanceListNautical.at(cur_zoom_lvl));
        break;
    default:
        break;
    }
}

void FrameControl1::on_pushButtonZoomOut_clicked()
{
    const int cur_range = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toInt();
    int cur_zoom_lvl = distanceList.indexOf(cur_range)-1;

    switch (Utils::unit) {
    case 0:
        cur_zoom_lvl = distanceList.indexOf(cur_range)-1;
        if(cur_zoom_lvl < 0) cur_zoom_lvl = 0;
        RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE, distanceList.at(cur_zoom_lvl));
        break;
    case 1:
        cur_zoom_lvl = distanceListNautical.indexOf(cur_range)-1;
        if(cur_zoom_lvl < 0) cur_zoom_lvl = 0;
        RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE, distanceListNautical.at(cur_zoom_lvl));
        break;
    default:
        break;
    }
}

void FrameControl1::on_pushButtonTxStnb_clicked()
{
    if(ui->pushButtonTxStnb->text().contains("Transmit"))
    {
        const int cur_range = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toInt();

        switch (Utils::unit) {
        case 0:
        {
            int cur_zoom_lvl = distanceList.indexOf(cur_range);
            if(cur_zoom_lvl < 0) cur_zoom_lvl = 0;
            RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE, distanceList.at(cur_zoom_lvl));
            break;
        }
        case 1:
        {
            int cur_zoom_lvl = distanceListNautical.indexOf(cur_range);
            if(cur_zoom_lvl < 0) cur_zoom_lvl = 0;
            RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE, distanceListNautical.at(cur_zoom_lvl));
            break;
        }
        default:
            break;
        }

        m_re->TriggerReqTx();
    }
    if(ui->pushButtonTxStnb->text().contains("Standby"))
    {
        m_re->TriggerReqStby();
        sleep(1);
    }
}

void FrameControl1::on_checkBoxShowRing_clicked(bool checked)
{
    RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_RING,checked);
    if(checked)
        ui->labelRingRange->setText("Rings "+ringValue);
    else
        ui->labelRingRange->setText("Rings off");

}

void FrameControl1::on_pushButtonShutdown_clicked()
{
    emit signal_req_shutdown();
}
