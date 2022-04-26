#include "framestatus.h"
#include "ui_framestatus.h"
#include "utils.h"

FrameStatus::FrameStatus(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameStatus)
{
    ui->setupUi(this);
    ui->labelRadarStatus->setText("No Radar");
    ui->labelRadarStatus->setStyleSheet("color: rgb(164,0,0);");

    m_re = RadarEngine::RadarEngine::getInstance();

    connect(RadarConfig::RadarConfig::getInstance(""), &RadarConfig::RadarConfig::configValueChange, this, &FrameStatus::trigger_statusChange);
}

void FrameStatus::trigger_statusChange(const QString& key, const QVariant& val)
{
    if(key == RadarConfig::VOLATILE_RADAR_STATUS)
    {
        RadarEngine::RadarState status = static_cast<RadarEngine::RadarState>(val.toInt());
        status = RadarEngine::RADAR_WAKING_UP;
        RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_RADAR_WAKINGUP_TIME,30);
        switch (status) {
        case RadarEngine::RADAR_OFF:
            ui->labelRadarStatus->setText("No Radar");
            ui->labelRadarStatus->setStyleSheet("color: rgb(164,0,0);");
            break;
        case RadarEngine::RADAR_WAKING_UP:
        {
            quint8 tick = static_cast<quint8>(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::VOLATILE_RADAR_WAKINGUP_TIME).toInt());
            ui->labelRadarStatus->setText("Waking up\n"+tickToTime(tick));
            ui->labelRadarStatus->setStyleSheet("color: rgb(196, 160, 0);");
        }
            break;
        case RadarEngine::RADAR_STANDBY:
            ui->labelRadarStatus->setText("Standby");
            ui->labelRadarStatus->setStyleSheet("color: color: rgb(196, 160, 0);");
            break;
        case RadarEngine::RADAR_TRANSMIT:
            ui->labelRadarStatus->setText("Transmiting");
            ui->labelRadarStatus->setStyleSheet("color: rgb(78, 154, 6);");
            break;
        case RadarEngine::RADAR_NO_SPOKE:
            ui->labelRadarStatus->setText("No Spoke");
            ui->labelRadarStatus->setStyleSheet("color: rgb(164,0,0);");
            break;
        }
    }
}

FrameStatus::~FrameStatus()
{
    delete ui;
}
