#include "shared/utils.h"
#include "framestatus.h"
#include "ui_framestatus.h"

FrameStatus::FrameStatus(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameStatus)
{
    ui->setupUi(this);
    initStatus();

    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");
    m_instance_re = RadarEngine::RadarEngine::GetInstance(this);
    m_alarm_manager = AlarmManager::GetInstance();

    m_alarm_toggle = true;

    connect(m_instance_cfg, &RadarEngine::RadarConfig::configValueChange, this, &FrameStatus::trigger_statusChange);
    connect(m_alarm_manager, &AlarmManager::SignalAlarm, this, &FrameStatus::trigger_Alarm);
}

void FrameStatus::trigger_Alarm(const QString id, const QString msg)
{
    qDebug()<<Q_FUNC_INFO<<msg;

    if(id == "No Alarm")
    {
        ui->labelAlarmStatus->setStyleSheet("background-color: rgb(78, 154, 6);");
        ui->labelAlarmStatus->setText(id);
        return;
    }

    if(m_alarm_toggle)
    {
        ui->labelAlarmStatus->setStyleSheet("background-color: rgb(164,0,0);");
        ui->labelAlarmStatus->setText(id);
        m_alarm_toggle = false;
    }
    else
    {
        ui->labelAlarmStatus->setStyleSheet("background-color: rgb(0, 5, 83);");
        m_alarm_toggle = true;
    }
}

void FrameStatus::updateRadarStatus(const RadarEngine::RadarState status)
{
    /*tes
    RadarEngine::RadarState test_status = RadarEngine::RADAR_STANDBY;
    m_instance_cfg->setConfig(RadarEngine::VOLATILE_RADAR_WAKINGUP_TIME,30);
    */

    switch (status) {
    case RadarEngine::RADAR_OFF:
        ui->labelRadarStatus->setText("No Radar");
        ui->labelRadarStatus->setStyleSheet("color: rgb(164,0,0);");
        break;
    case RadarEngine::RADAR_WAKING_UP:
    {
        quint8 tick = static_cast<quint8>(m_instance_cfg->getConfig(RadarEngine::VOLATILE_RADAR_WAKINGUP_TIME).toInt());
        ui->labelRadarStatus->setText("Waking up\n"+Utils::TickToTime(tick));
        ui->labelRadarStatus->setStyleSheet("color: rgb(196, 160, 0);");
    }
        break;
    case RadarEngine::RADAR_STANDBY:
        ui->labelRadarStatus->setText("Standby");
        ui->labelRadarStatus->setStyleSheet("color: rgb(196, 160, 0);");
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

void FrameStatus::updateNavStatus(const int status)
{
    qDebug()<<Q_FUNC_INFO<<status;
    switch (status) {
    case 0:
        ui->labelNavStatus->setText("Offline");
        ui->labelNavStatus->setStyleSheet("color: rgb(164,0,0);");
        break;
    case 1:
        ui->labelNavStatus->setText("No Data");
        ui->labelNavStatus->setStyleSheet("color: rgb(196, 160, 0);");
        break;
    case 2:
        ui->labelNavStatus->setText("Invalid");
        ui->labelNavStatus->setStyleSheet("color: rgb(164,0,0);");
        break;
    case 3:
        ui->labelNavStatus->setText("Ok");
        ui->labelNavStatus->setStyleSheet("color: rgb(78, 154, 6);");
        break;
    }
}

void FrameStatus::trigger_statusChange(const QString& key, const QVariant& val)
{
    if(key == RadarEngine::VOLATILE_RADAR_STATUS)
    {
        RadarEngine::RadarState status = static_cast<RadarEngine::RadarState>(val.toInt());
        updateRadarStatus(status);
    }
    else if(key == RadarEngine::VOLATILE_NAV_STATUS_GPS || key == RadarEngine::VOLATILE_NAV_STATUS_HEADING)
    {
        updateNavStatus(val.toInt());
    }
}

FrameStatus::~FrameStatus()
{
    delete ui;
}

void FrameStatus::on_alarmStatus_clicked(const QPoint &p)
{
    Q_UNUSED(p)
    qDebug()<<Q_FUNC_INFO;
    m_alarm_manager->Confirm(ui->labelAlarmStatus->text());
}

void FrameStatus::initStatus()
{
    ui->labelRadarStatus->setText("No Radar");
    ui->labelRadarStatus->setStyleSheet("color: rgb(164,0,0);");
    ui->labelNavStatus->setText("Offline");
    ui->labelNavStatus->setStyleSheet("color: rgb(164,0,0);");
    ui->labelAlarmStatus->setText("No Alarm");
    ui->labelAlarmStatus->setStyleSheet("background-color: rgb(78, 154, 6);");

    m_alarm_event = new PPIEvent(ui->labelAlarmStatus);
    ui->labelAlarmStatus->installEventFilter(m_alarm_event);

    connect(m_alarm_event,&PPIEvent::send_leftButtonReleased,this,&FrameStatus::on_alarmStatus_clicked);
}

