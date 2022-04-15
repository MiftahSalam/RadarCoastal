#include "framecontrol1.h"
#include "ui_framecontrol1.h"

#include <radarengine_global.h>
#include <radarconfig.h>


FrameControl1::FrameControl1(QWidget *parent) :
    QFrame(parent), ui(new Ui::FrameControl1)
{
    ui->setupUi(this);

    RadarConfig::RadarConfig* rcInstance = RadarConfig::RadarConfig::getInstance("");

    ui->checkBoxShowRing->setChecked(rcInstance->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_RING).toBool());
    on_checkBoxShowRing_clicked(rcInstance->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_RING).toBool());
    handleRingRangeChange();
    stateChange(RadarEngine::RADAR_OFF);

    connect(rcInstance,&RadarConfig::RadarConfig::configValueChange,this,&FrameControl1::trigger_radarConfigChange);
}

void FrameControl1::setRadarEnginePtr(RadarEngine::RadarEngine *re)
{
    m_re = re;
}

void FrameControl1::handleRingRangeChange()
{
    double rng = static_cast<double>(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toInt())/1000.;
    double rng_ring = rng/RING_COUNT;
    if(rng < 1)
    {
        rng *= 1000.;
        ui->labelRange->setText(QString::number(static_cast<int>(rng))+" m");
    }
    else
        ui->labelRange->setText(QString::number(rng,'f',1)+" km");

    ringValue = QString::number(rng_ring,'f',2);
    if(rng_ring < 1)
    {
        rng_ring *= 1000.;
        ringValue = QString::number(static_cast<int>(rng_ring))+" m";
    }
    else
        ringValue = QString::number(rng_ring,'f',1)+" km";

    if(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_RING).toBool())
        ui->labelRingRange->setText("Rings "+ringValue);
}

void FrameControl1::trigger_radarConfigChange(QString key, QVariant val)
{
//    qDebug()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val;

    if(key == RadarConfig::VOLATILE_RADAR_STATUS) stateChange(val.toInt());
    else if(key == RadarConfig::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE) handleRingRangeChange();
}

void FrameControl1::stateChange(int state)
{
    if(static_cast<RadarEngine::RadarState>(state) == RadarEngine::RADAR_STANDBY)
    {
        ui->pushButtonTxStnb->setEnabled(true);
        ui->pushButtonTxStnb->setText("Transmit");
    }
    else if(static_cast<RadarEngine::RadarState>(state) == RadarEngine::RADAR_TRANSMIT)
    {
        ui->pushButtonTxStnb->setEnabled(true);
        ui->pushButtonTxStnb->setText("Standby");

        //comment for radar offline mode
        /*
        const uint cur_range = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::VOLATILE_RADAR_PARAMS_RANGE_DATA_RANGE).toUInt();
        const uint cur_scale = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toUInt();
        if(cur_scale != cur_range) m_re->trigger_ReqRangeChange(static_cast<int>(cur_scale));
        */
    }
    else
    {
        ui->pushButtonTxStnb->setEnabled(false);
        ui->pushButtonTxStnb->setText("Tx/Stby");
    }
    /*
    if((RadarState)state == RADAR_STANDBY)
    {
        ui->pushButtonTxStnb->setEnabled(true);
        if(state_radar1 == RADAR_STANDBY)
            ui->pushButtonTxStnb->setText("Transmit");
        else if(state_radar1 == RADAR_TRANSMIT)
            ui->pushButtonTxStnb->setText("Standby");
    }
    else if((RadarState)state == RADAR_TRANSMIT)
    {
        ui->pushButtonTxStnb->setEnabled(true);
        ui->pushButtonTxStnb->setText("Standby");
    }
    else
    {
        ui->pushButtonTxStnb->setEnabled(false);
        ui->pushButtonTxStnb->setText("Tx/Stby");
    }
    */
}
/*
void FrameControl1::setRangeRing(double ringWidth)
{
    ringValue = QString::number(ringWidth,'f',2);
    if(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_RING).toBool())
        ui->labelRingRange->setText("Rings "+ringValue+" km");
    else
        ui->labelRingRange->setText("Rings off");
}
*/
FrameControl1::~FrameControl1()
{
    delete ui;
}

void FrameControl1::on_pushButtonZoomIn_clicked()
{
    const int cur_range = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toInt();
    int cur_zoom_lvl = RadarEngine::distanceList.indexOf(cur_range)+1;

    if(cur_zoom_lvl > RadarEngine::distanceList.size()-1) cur_zoom_lvl -=1;
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE, RadarEngine::distanceList.at(cur_zoom_lvl));
    /*
    int g;
    QString rngName = ui->labelRange->text();
    for (g = 0; g < ARRAY_SIZE(g_ranges_metric); g++)
    {
        if (QString(g_ranges_metric[g].name )== rngName)
            break;
    }
    g++;
    if(g >= ARRAY_SIZE(g_ranges_metric))
        g--;

    ui->labelRange->setText(g_ranges_metric[g].name);
    emit signal_req_range(g_ranges_metric[g].meters);
    */
}

void FrameControl1::on_pushButtonZoomOut_clicked()
{
    const int cur_range = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toInt();
    int cur_zoom_lvl = RadarEngine::distanceList.indexOf(cur_range)-1;

    if(cur_zoom_lvl < 0) cur_zoom_lvl = 0;
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE, RadarEngine::distanceList.at(cur_zoom_lvl));
    /*
    int g;
    QString rngName = ui->labelRange->text();
    for (g = 0; g < ARRAY_SIZE(g_ranges_metric); g++)
    {
        if (QString(g_ranges_metric[g].name )== rngName)
            break;
    }
    if(g != 0)
        g--;

    ui->labelRange->setText(g_ranges_metric[g].name);
    emit signal_req_range(g_ranges_metric[g].meters);
    */
}

void FrameControl1::on_pushButtonTxStnb_clicked()
{
    if(ui->pushButtonTxStnb->text().contains("Transmit"))
//        emit signal_req_Tx();
        m_re->trigger_ReqTx();
    if(ui->pushButtonTxStnb->text().contains("Standby"))
//        emit signal_req_Stby ();
        m_re->trigger_ReqStby();
}

void FrameControl1::on_checkBoxShowRing_clicked(bool checked)
{
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_RING,checked);
//    radar_settings.show_rings = checked;
    if(checked)
        ui->labelRingRange->setText("Rings "+ringValue);
    else
        ui->labelRingRange->setText("Rings off");

}

void FrameControl1::on_pushButtonShutdown_clicked()
{
    emit signal_req_shutdown();
}
